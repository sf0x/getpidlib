#pragma once

#define _WIN32_DCOM
#include "get_pid_wmi.h"
#include <Windows.h>
#include <Wbemidl.h>
#include <comutil.h>
#include <string>
#include <UserEnv.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

LPWSTR getCurrentSID();
DWORD getPidWmi(const wchar_t* targetProcName);