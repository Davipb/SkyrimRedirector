#include "StringUtils.h"
#include "Logging.h"
#include <locale.h>
#include <Windows.h>

static _locale_t InvariantLocale = NULL;

_locale_t SR_GetInvariantLocale()
{
	if (InvariantLocale == NULL)
		InvariantLocale = _create_locale(LC_ALL, "C");

	return InvariantLocale;
}

char* SR_Utf16ToUtf8(const wchar_t* utf16)
{
	int needed = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
	char* result = calloc(needed, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, utf16, -1, result, needed, NULL, NULL);

	return result;
}

const wchar_t* SR_GetFileNameW(const wchar_t* path)
{
	wchar_t* lastBack = wcsrchr(path, L'\\');
	wchar_t* lastForward = wcsrchr(path, L'/');

	if (lastBack != NULL && lastForward != NULL)
		return max(lastBack, lastForward) + 1;

	if (lastBack != NULL) return lastBack + 1;
	if (lastForward != NULL) return lastForward + 1;

	return path;
}

const char* SR_GetFileNameA(const char* path)
{
	char* lastBack = strrchr(path, '\\');
	char* lastForward = strrchr(path, '/');

	if (lastBack != NULL && lastForward != NULL)
		return max(lastBack, lastForward) + 1;

	if (lastBack != NULL) return lastBack + 1;
	if (lastForward != NULL) return lastForward + 1;

	return path;
}

const wchar_t* SR_ToUpperW(const wchar_t* input)
{
	size_t inputSize = wcslen(input) + 1;
	wchar_t* inputUpper = calloc(inputSize, sizeof(wchar_t));

	wcscpy_s(inputUpper, inputSize, input);
	_wcsupr_s_l(inputUpper, inputSize, SR_GetInvariantLocale());

	return inputUpper;
}

const char* SR_ToUpperA(const char* input)
{
	size_t inputSize = strlen(input) + 1;
	char* inputUpper = calloc(inputSize, sizeof(char));

	strcpy_s(inputUpper, inputSize, input);
	_strupr_s_l(inputUpper, inputSize, SR_GetInvariantLocale());

	return inputUpper;
}

bool SR_AreCaseInsensitiveEqualW(const wchar_t* first, const wchar_t* second)
{
	return _wcsicmp_l(first, second, SR_GetInvariantLocale()) == 0;
}

bool SR_AreCaseInsensitiveEqualA(const char* first, const char* second)
{
	return _stricmp_l(first, second, SR_GetInvariantLocale()) == 0;
}