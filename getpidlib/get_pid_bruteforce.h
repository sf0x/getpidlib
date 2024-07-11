#ifndef GET_PID_BRUTEFORCE_H
#define GET_PID_BRUTEFORCE_H
#pragma once

#include <Windows.h>
#include <psapi.h>
#include <shlwapi.h>

DWORD getPidBruteForce(wchar_t* procname);
#endif