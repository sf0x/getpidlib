# getpidlib - sliver extension
Proof of concept for techniques to find a PID to a process name in Windows.

# Index 

[[_TOC_]]
# Information

During my research in malware development I was annoyed that every time I wrote an injection the PID of a target process was required and to get that I had to remember how to do that properly with opsec in mind. Therefor this library was created to have a quick-to-use collection of all proper techniques, which can be applied to any project. The collection was made with evasion of enumeration detection on mind, but does currently not implement additional evasion techniques, since it is only meant to be able to make research on which is the proper one to use against a specific target.

For learning purposes I made this collection a [sliver](https://sliver.sh/) C2 extension one can dynamically load into a implant and monitor detection.

The methods will return **the first** occurrence of the searched process. Some of the methods are only able to return PIDs of the same user, whereas some others use Windows APIs that will return the PIDs of processes run by other users too. Maybe I will build a switch that let's one chose this behaviour.

# Techniques

### Brute Force

Iterates through 0x10000 PIDs (can be adjusted in the header file) and uses `OpenProcess()` and `K32GetProcessImageFileNameW()` to read the process name. This is not very stealthy, but also there are no critical monitored syscalls used, which can make it quite useful.

### EnumProcesses

 Kind of the official way to enumerate all running processes, suggested to be used by [Microsoft themselves](https://learn.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocesses). The usage of the functions will most likely be monitored by EDRs and alerted by static scans.

### WMI

Uses WMI to identify processes PIDs accessible for the current user.

### NtQueryInformation

In my opinion, this is the most noisy and non-stealth technique, it uses `NtQuerySystemInformation()`, `VirtualAlloc()` and even the super detection  triplet  `GetProcAddress(GetModuleHandleW(L"ntdll.dll"))`, which makes it from a opsec perspective the most unqualified technique of all.

- [x] Is able to retrieve PIDs of all users

### NtGetNextProcess

This one also uses the very noisy  direct load of ntdll via `GetProcAddress(GetModuleHandleW(L"ntdll.dll"))` and the `NtGetNextProcess()` syscall to retrieve the PID of a process.

### CreateToolhelp32Snapshot

For some year, this was the standard way to make process enumeration. This call stores a snapshot of all process information in a linked list where you can easily iterate through and search for the desired name. Today it is highly monitored and even static analysis by standard antivirus software is alerting its presence in a PE.

- [x] Is able to retrieve PIDs of all users

### WTSEnumerateProcesses

Makes use of the `WtsApi32` API call to list all processes. Requires `SeTcbPrivilege` to be enabled and direct linking against `wtsapi32.lib`.

- [x] Is able to retrieve PIDs of all users

### Discarded

List of (currently) discarded techniques:
| Method | Info | Reason |
|--------|------|--------|
|[process-enumeration-stealth](https://github.com/LloydLabs/process-enumeration-stealth) | Creates a NTFS handle to to `\\NTFS\\` and reads the PIDs from the NTFS file infos. | Queries [NtQueryInformationFile](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile) for `FileProcessIdsUsingFileInformation` but this is not a valid FILE_INFORMATION_CLASS value (anymore at least). Macro resolves to 0x2f (47) which is not listed in the values. |
| Process Enumeration by Named Pipes |I tried to open all named pipes, retrieve their owners PID and read their name. | Not applicable for all processes, since the opening of pipes mostly failes due to access privileges and there weren't pipes for all processes, which makes this feasible for only retrieving the PID of `LSASS`, see [this link](https://web.archive.org/web/20240623231705/https://www.mdsec.co.uk/2022/08/fourteen-ways-to-read-the-pid-for-the-local-security-authority-subsystem-service-lsass/) for reference. |
| [Windows kernel driver](https://github.com/danielkrupinski/KernelPID) |Driver to read out the PID of a process. | Requires admin privs - if I as an attacker have obtained these, there are hundreds of ways to get the PID more stealthy than with a driver. |

# Sliver Extension

**disclaimer**: The code is very sloppy. I do not perform proper string- or memory management and did not put any effort in evasion or anything else. This is just a pure POC code and should be seen as such. I only created it because I never build a sliver extension before and wanted to become familiar with the techniques required.

## Usage

Open the VS project and compile the code. Put the *.dll* and the *extension.json* into a folder on your sliver server and install the extension via `extensions install /path/to/ext`. More infos on loading the extension can be found [here](https://dominicbreuker.com/post/learning_sliver_c2_12_extensions/).

Execute the extension with `getpidlib "<process name>" "<method>"`:

```
[*] Active session SHARP_MESS (8e1b49e6-1daf-4dde-bb0c-d140100e97f1)

[server] sliver (SHARP_MESS) > getpidlib "sihost.exe" "wmi"

[*] Successfully executed getpidlib
[*] Got output:
[+] Arguments processed
[+] Using method wmi to search for process sihost.exe
[+] sihost.exe has the PID 4868
```

#### Methods

```
enum
wmi
ntqueryinfo
ntgetnext
wtsenum
thelp32
bruteforce
```



# References

* [Find PID via NtGetNextProcess](https://web.archive.org/web/20240623133838/https://cocomelonc.github.io/malware/2023/05/26/malware-tricks-30.html)
* [Find PID by NtQuerySystemInformation](https://web.archive.org/web/20240623134346/https://medium.com/@s12deff/find-pid-by-ntquerysysteminformation-e21f46bcea3d)
* [NtQuerySystemInformation (MS)](https://learn.microsoft.com/de-de/windows/win32/api/winternl/nf-winternl-ntquerysysteminformation)
* [Find PID by EnumProcesses](https://web.archive.org/web/20240623134538/https://medium.com/@s12deff/find-processes-with-enumprocesses-52ef3c07446a)
* [Enumerating Processes with WTSEnumerateProcesses](https://web.archive.org/web/20240623235702/https://dmfrsecurity.com/2021/04/17/enumerating-processes-with-wtsenumerateprocesses/)
* [Find PID by WTSEnumerateProcesses](https://web.archive.org/web/20240623210831/https://cocomelonc.github.io/malware/2023/07/07/malware-tricks-34.html)
* [Toolhelp32Snapshot](https://learn.microsoft.com/de-de/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot)
* [Find PID by Name](https://web.archive.org/web/20240623212544/https://cocomelonc.github.io/pentest/2021/09/29/findmyprocess.html)
* [Fourteen ways to read the PID of LSASS](https://web.archive.org/web/20240623234437/https://www.mdsec.co.uk/2022/08/fourteen-ways-to-read-the-pid-for-the-local-security-authority-subsystem-service-lsass/)
