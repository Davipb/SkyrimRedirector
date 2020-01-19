#include "SR_Base.h"
#include "StringUtils.h"
#include "Logging.h"
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>

static _locale_t InvariantLocale = NULL;

_locale_t SR_GetInvariantLocale()
{
	if (InvariantLocale == NULL)
		InvariantLocale = _create_locale(LC_ALL, "C");

	return InvariantLocale;
}

static char* Utf16ToCodepage(const wchar_t* utf16, UINT codePage)
{
	int needed = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
	char* result = calloc(needed, sizeof(char));
	WideCharToMultiByte(codePage, 0, utf16, -1, result, needed, NULL, NULL);

	return result;
}

char* SR_Utf16ToUtf8(const wchar_t* utf16) { return Utf16ToCodepage(utf16, CP_UTF8); }
char* SR_Utf16ToCodepage(const wchar_t* utf16) { return Utf16ToCodepage(utf16, CP_ACP); }

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

wchar_t* SR_Concat(size_t count, ...)
{
	size_t resultSize = 1;
	wchar_t* result = calloc(resultSize, sizeof(wchar_t));

	va_list args;
	va_start(args, count);

	while (count-- > 0)
	{
		const wchar_t* part = va_arg(args, const wchar_t*);
		size_t partLen = wcslen(part);

		resultSize += partLen;
		result = realloc(result, resultSize * sizeof(wchar_t));

		wcscat_s(result, resultSize, part);
	}

	va_end(args);

	return result;
}

bool SR_EndsWithW(const wchar_t* full, const wchar_t* component)
{
	// Get a pointer to the last character of each string (right before the null terminator)
	const wchar_t* fullCurrent = &full[wcslen(full) - 1];
	const wchar_t* componentCurrent = &component[wcslen(component) - 1];

	// Check each string backwards until one of them runs out
	// If any character is different, the path doesn't end with the expected component
	while (fullCurrent != full && componentCurrent != component)
	{
		if (*fullCurrent != *componentCurrent)
			return false;

		fullCurrent--;
		componentCurrent--;
	}

	// If the path ran out before the expected component, it is shorter than the expected component,
	// which means the path doesn't end with the component
	return componentCurrent == component;
}

bool SR_EndsWithA(const char* full, const char* component)
{
	// Get a pointer to the last character of each string (right before the null terminator)
	const char* fullCurrent = &full[strlen(full) - 1];
	const char* componentCurrent = &component[strlen(component) - 1];

	// Check each string backwards until one of them runs out
	// If any character is different, the path doesn't end with the expected component
	while (fullCurrent != full && componentCurrent != component)
	{
		if (*fullCurrent != *componentCurrent)
			return false;

		fullCurrent--;
		componentCurrent--;
	}

	// If the path ran out before the expected component, it is shorter than the expected component,
	// which means the path doesn't end with the component
	return componentCurrent == component;
}
