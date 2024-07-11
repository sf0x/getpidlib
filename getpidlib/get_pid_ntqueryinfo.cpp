#include "get_pid_ntqueryinfo.h"

DWORD getPidNtQueryInfo(wchar_t* procname) {
	DWORD pid = 0;
	PVOID data = nullptr;
	DWORD bufSize = 0;

	NtQuerySystemInformation_t pNtQuerySystemInformation = (NtQuerySystemInformation_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation");

	pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessInformation, 0, 0, &bufSize);
	if (bufSize == 0)
		return 0;

	if (data = VirtualAlloc(0, bufSize, MEM_COMMIT, PAGE_READWRITE)) {
		SYSTEM_PROCESS_INFORMATION* sysproc_info = (SYSTEM_PROCESS_INFORMATION*)data;
		if (!pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessInformation, data, bufSize, &bufSize)) {
			while (TRUE) {
				if (!lstrcmpiW(procname, sysproc_info->ImageName.Buffer)) {
					pid = (DWORD)sysproc_info->UniqueProcessId;
					VirtualFree(data, bufSize, MEM_RELEASE);
					return pid;
				}
				if (!sysproc_info->NextEntryOffset) {
					break;
				}
				sysproc_info = (SYSTEM_PROCESS_INFORMATION*)((ULONG_PTR)sysproc_info + sysproc_info->NextEntryOffset);
			}
		}
	}
	VirtualFree(data, bufSize, MEM_RELEASE);
	return pid;
}