#include "pch.h"
#include "helper.h"
#include "get_pid_enum.h"
#include "get_pid_wmi.h"
#include "get_pid_bruteforce.h"
#include "get_pid_ntqueryinfo.h"
#include "get_pid_ntgetnext.h"
#include "get_pid_wtsenum.h"
#include "get_pid_thelp32.h" 

#include <string>
#include <sstream>

typedef int (*goCallback)(const char*, int);

extern std::string msg;

void appendFormattedMessage(const char* format, ...);

struct Argss {
	std::string procname;
	std::string method;
};

std::string msg{ "" };

// taken from https://github.com/sliverarmory/winrmdll-sliver
void appendFormattedMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	// Determine the required size of the formatted string
	va_list args_copy;
	va_copy(args_copy, args);
	int size = vsnprintf(nullptr, 0, format, args_copy);
	va_end(args_copy);

	if (size < 0) {
		// Handle error
		return;
	}

	// Allocate the buffer with the required size (+2 for null-terminator and potential overflow)
	std::unique_ptr<char[]> outBuf(new char[size + 2]);

	// Format the string into the buffer
	vsnprintf(outBuf.get(), size + 1, format, args); // Use size + 1 here

	va_end(args);

	msg.append(outBuf.get());

}

Argss* parseArgs(const char* argumentString) {
	std::istringstream iss(argumentString);
	std::string arg;
	std::string value;
	Argss* arguments = new Argss();
	iss >> arguments->procname >> arguments->method;
	return arguments;
}

extern "C" {
    __declspec(dllexport) int __cdecl Execute(char* argsBuffer, uint32_t bufferSize, goCallback callback);
}

int Execute(char* argsBuffer, uint32_t bufferSize, goCallback callback) {
	
	msg = "";

    if (bufferSize < 2)
    {
		appendFormattedMessage("[-] You must provide the <Method> to use and <Process Name> to search."); 
        callback(msg.c_str(), msg.length());
		return 1;
    }

	Argss* args = parseArgs(argsBuffer);

	appendFormattedMessage("[+] Arguments processed\n");
	appendFormattedMessage("[+] Using method %s to search for process %s\n", args->method.c_str(), args->procname.c_str());

	// Search execution
	DWORD pid = 1; // 1 is an impossible pid, since pids are a multiple of 4

	if (!lstrcmpA(args->method.c_str(), "enum")) {
		pid = getPidEnumCall(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "wmi")) {
		pid = getPidWmi(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "bruteforce")) {
		pid = getPidBruteForce(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "ntqueryinfo")) {
		pid = getPidNtQueryInfo(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "ntgetnext")) {
		pid = getPidNtGetNextProc(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "wtsenum")) {
		pid = getPidWtsEnum(charToWchar(args->procname.c_str()));
	}
	else if (!lstrcmpA(args->method.c_str(), "thelp32")) {
		pid = getPidThelp32(charToWchar(args->procname.c_str()));
	}
	// Method not found
	else {
		appendFormattedMessage("[-] Could not find method %s", args->method.c_str());
		callback(msg.c_str(), msg.length());
		return 1;
	}

	// Process not found
	if (pid == 1) {
		appendFormattedMessage("[-] Could not find process %s", args->procname.c_str());
		callback(msg.c_str(), msg.length());
		return 1;
	}
	appendFormattedMessage("[+] %s has the PID %d", args->procname.c_str(), pid);
	callback(msg.c_str(), msg.length());
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

