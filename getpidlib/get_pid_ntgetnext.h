#ifndef GET_PID_NTGETNEXT_H
#define GET_PID_NTGETNEXT_H
#pragma once

#include <windows.h>
#include <stdio.h>
#include <winternl.h>
#include <psapi.h>
#include <shlwapi.h>
#include "helper.h"


#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "shlwapi.lib")

typedef NTSTATUS(NTAPI* fNtGetNextProcess)(
	_In_ HANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ ULONG HandleAttributes,
	_In_ ULONG Flags,
	_Out_ PHANDLE NewProcessHandle
);

DWORD getPidNtGetNextProc(wchar_t* procname);

#endif