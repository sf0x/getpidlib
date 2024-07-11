#include "get_pid_bruteforce.h"

DWORD getPidBruteForce(wchar_t* procname) {
    // PIDs are always a multiple of 4 and are created iteratly
	for (int i = 4; i < 20000; i=i+4) {
        HMODULE hMod;
        DWORD cbNeeded;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, i);
        TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
        DWORD pSize;
        K32GetProcessImageFileNameW(hProcess, szProcessName, MAX_PATH);
        if (!lstrcmpiW(procname, PathFindFileName(szProcessName))) {
            CloseHandle(hProcess);
            return i;
        }
        CloseHandle(hProcess);
	}
    return 0;
}