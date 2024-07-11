#ifndef GET_PID_ENUM_H
#define GET_PID_ENUM_H
#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

DWORD getPidEnumCall(const wchar_t* procname);
void printProcessAndID(DWORD);
#endif