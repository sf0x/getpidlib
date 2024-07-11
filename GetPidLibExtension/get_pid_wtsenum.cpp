#include "pch.h"
#include "get_pid_wtsenum.h"

DWORD getPidWtsEnum(wchar_t* procname) {
	DWORD pid;
	WTS_PROCESS_INFOW* pinfo;

	DWORD count = 0;

	if (!WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pinfo, &count))
		return 0;

	for (int i = 0; i < count; i++) {
		if (!lstrcmpiW(procname, pinfo[i].pProcessName)) {
			pid = pinfo[i].ProcessId;
			WTSFreeMemory(pinfo);
			return pid;
		}
	}
	WTSFreeMemory(pinfo);
	return 0;
}