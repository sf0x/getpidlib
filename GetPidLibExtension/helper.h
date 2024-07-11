#ifndef HELPER_H
#define HELPER_H
#pragma once
#include <Windows.h>
#include <string>
#include <stdlib.h>

wchar_t* charToWchar(const char* str);
char* wcharToChar(const wchar_t* wstr);

#endif