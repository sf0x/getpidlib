#ifndef HELPER_H
#define HELPER_H
#pragma once
#include <Windows.h>
#include <string>
#include <stdlib.h>

size_t strlenW(const wchar_t*);
size_t strlenA(const char*);
wchar_t* charToWchar(const char*);
char* wcharToChar(const wchar_t*);


#endif