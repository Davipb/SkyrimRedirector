#include "StringUtils.h"
#include <Windows.h>

char* SR_Utf16ToUtf8(const wchar_t* utf16)
{
	int needed = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
	char* result = calloc(needed, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, utf16, -1, result, needed, NULL, NULL);

	return result;
}