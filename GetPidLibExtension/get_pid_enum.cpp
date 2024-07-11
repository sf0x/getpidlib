#include "pch.h"
#include "get_pid_enum.h"

DWORD getPidEnumCall(const wchar_t* procname) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0)
        {
            HMODULE hMod;
            DWORD cbNeeded;
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                    if (!wcscmp(procname, szProcessName)) {
                        CloseHandle(hProcess);
                        return aProcesses[i];
                    }
            }
            CloseHandle(hProcess);
        }
    }
    return 0;
}