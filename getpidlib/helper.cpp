#include "helper.h"



wchar_t* charToWchar(const char* str) {
	size_t outSize = strlen(str) + 1;
	wchar_t* outStr = new wchar_t[outSize];
	size_t count = 0;
	mbstowcs_s(&count, outStr, outSize, str, outSize-1);
	return outStr;
}

char* wcharToChar(const wchar_t* wstr) {
	size_t outSize = wcslen(wstr) + 1;
	char* outStr = new char[outSize];
	size_t count = 0;
	wcstombs_s(&count, outStr, outSize, wstr, outSize-1);
	return outStr;
}