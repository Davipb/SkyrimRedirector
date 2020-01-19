#include "SR_Base.h"
#include "WindowsUtils.h"
#include "StringUtils.h"

#include <string.h>
#include <ShlObj.h>

wchar_t* SR_GetKnownFolder(const KNOWNFOLDERID* const rfid)
{
	wchar_t* folderPath;
	SHGetKnownFolderPath(rfid, 0, NULL, &folderPath);

	// Copy to a standard C-duplicated string to allow the caller to free this string with `free`
	// instead of CoTaskMemFree
	wchar_t* result = _wcsdup(folderPath);
	CoTaskMemFree(folderPath);

	return result;
}

wchar_t* SR_CanonicizePathW(const wchar_t* path)
{
	// GetFullPathName returns the required buffer size if the buffer is too small (in this case, size 0)
	const DWORD canonicizedSize = GetFullPathNameW(path, 0, NULL, NULL);
	wchar_t* canonicized = calloc(canonicizedSize, sizeof(wchar_t));
	GetFullPathNameW(path, canonicizedSize, canonicized, NULL);

	// In-place uppercase path
	_wcsupr_s_l(canonicized, canonicizedSize, SR_GetInvariantLocale());

	return canonicized;
}

char* SR_CanonicizePathA(const char* path)
{
	// GetFullPathName returns the required buffer size if the buffer is too small (in this case, size 0)
	const DWORD canonicizedSize = GetFullPathNameA(path, 0, NULL, NULL);
	char* canonicized = calloc(canonicizedSize, sizeof(char));
	GetFullPathNameA(path, canonicizedSize, canonicized, NULL);

	// In-place uppercase path
	_strupr_s_l(canonicized, canonicizedSize, SR_GetInvariantLocale());

	return canonicized;
}
