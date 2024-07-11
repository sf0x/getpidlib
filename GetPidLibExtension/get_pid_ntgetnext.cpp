#include "pch.h"
#include "get_pid_ntgetnext.h"

DWORD getPidNtGetNextProc(wchar_t* procname) {
	DWORD pid = 0;
	HANDLE hCurrent = NULL;
	wchar_t curProcName[MAX_PATH];

	fNtGetNextProcess pNtGetNextProcess = (fNtGetNextProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtGetNextProcess");

	while (!pNtGetNextProcess(hCurrent, MAXIMUM_ALLOWED, 0, 0, &hCurrent)) {
		GetProcessImageFileNameW(hCurrent, curProcName, MAX_PATH);
		if (!lstrcmpiW(procname, PathFindFileNameW(curProcName))) {
			pid = GetProcessId(hCurrent);
			CloseHandle(hCurrent);
			return pid;
		}
	}
	CloseHandle(hCurrent);
	return 0;
}