#include "get_pid_thelp32.h"

DWORD getPidThelp32(wchar_t* procname) {
	HANDLE hSnapshot;
	PROCESSENTRY32W pe;
	DWORD pid = 0;
	BOOL hResult;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	pe.dwSize = sizeof(PROCESSENTRY32W);

	hResult = Process32FirstW(hSnapshot, &pe);

	while (hResult) {
		if (!lstrcmpiW(procname, pe.szExeFile)) {
			pid = pe.th32ProcessID;
			CloseHandle(hSnapshot);
			return pid;
		}
		hResult = Process32NextW(hSnapshot, &pe);
	}

	CloseHandle(hSnapshot);
	return 0;
}