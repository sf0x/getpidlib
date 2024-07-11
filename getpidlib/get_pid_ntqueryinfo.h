#ifndef GET_PID_NTQUERYINFO_H
#define GET_PID_NTQUERYINFO_H
#pragma once

#include <Windows.h>
#include <winternl.h>

#define SystemProcessInformation 5

typedef NTSTATUS (WINAPI * NtQuerySystemInformation_t)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

DWORD getPidNtQueryInfo(wchar_t*);
#endif