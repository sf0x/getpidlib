// TODO: 

#include <iostream>
#include "get_pid_enum.h"
#include "get_pid_wmi.h"
#include "get_pid_bruteforce.h"
#include "get_pid_ntqueryinfo.h"
#include "get_pid_ntgetnext.h"
#include "get_pid_wtsenum.h"
#include "get_pid_thelp32.h" 

int main()
{
    wchar_t searchProcess[] = L"sihost.exe";
    DWORD pid;
    
    pid = getPidEnumCall(searchProcess);
    std::cout << "getPidEnumCall: " << pid << std::endl;

    pid = getPidWmi(searchProcess);
    std::cout << "getPidWmi: " << pid << std::endl;

    pid = getPidBruteForce(searchProcess);
    std::cout << "getPidBruteForce: " << pid << std::endl;

    pid = getPidNtQueryInfo(searchProcess);
    std::cout << "getPidNtQueryInfo: " << pid << std::endl;

    pid = getPidNtGetNextProc(searchProcess);
    std::cout << "getPidNGetNextProc: " << pid << std::endl;

    pid = getPidWtsEnum(searchProcess);
    std::cout << "getPidWtsEnum: " << pid << std::endl;

    pid = getPidThelp32(searchProcess);
    std::cout << "getPidThelp32: " << pid << std::endl;

    return 0;
}
