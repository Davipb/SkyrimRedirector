#include "Redirections.h"
#include "Logging.h"
#include "StringUtils.h"
#include "Config.h"
#include <ShlObj.h>
#include <stdbool.h>

// +==================================================================+
// |                         Redirect support                         |
// +==================================================================+

// The redirected file paths are stored in memory to prevent having to allocate and
// free memory on every call.

static wchar_t* NewIniW = NULL;
static wchar_t* NewPrefsIniW = NULL;
static wchar_t* NewPluginsW = NULL;

static char* NewIniA = NULL;
static char* NewPrefsIniA = NULL;
static char* NewPluginsA = NULL;


// Tries to redirect a wide path. If the path can't be redirected, it is returned unchanged.
// The returned string does not need to be freed.
static const wchar_t* TryRedirectW(const wchar_t* input)
{
	const wchar_t* fileName = SR_GetFileNameW(input);

	const wchar_t* result = input;
	if (SR_AreCaseInsensitiveEqualW(fileName, L"Skyrim.ini")) result = NewIniW;
	else if (SR_AreCaseInsensitiveEqualW(fileName, L"SkyrimPrefs.ini")) result = NewPrefsIniW;
	else if (SR_AreCaseInsensitiveEqualW(fileName, L"plugins.txt")) result = NewPluginsW;
	
	return result;
}

// Tries to redirect a narrow path. If the path can't be redirected, it is returned unchanged.
// The returned string does not need to be freed.
static const char* TryRedirectA(const char* input)
{
	const char* fileName = SR_GetFileNameA(input);

	const char* result = input;
	if (SR_AreCaseInsensitiveEqualA(fileName, "Skyrim.ini")) result = NewIniA;
	else if (SR_AreCaseInsensitiveEqualA(fileName, "SkyrimPrefs.ini")) result = NewPrefsIniA;
	else if (SR_AreCaseInsensitiveEqualA(fileName, "plugins.txt")) result = NewPluginsA;
	
	return result;
}
/*
+==================================================================+
|                        Redirect functions                        |
+==================================================================+
| The functions below are used by the game instead of the          |
| actual Windows API.                                              |
+------------------------------------------------------------------+
*/

/*
The following macro is be used as: REDIRECT(WinAPI function name, WinAPI return value, WinAPI arguments)
It creates three definitions:

  1. A typedef for a function pointer of the specified API, called (name)_t

  2. A static variable of the type just typedef'd, called SR_Original_(name)
	 This variable should store the original WinAPI being redirected, and can
	 be used to call the original API from inside the redirect.
  3. A function signature identical to the API being redirected, called
	 SR_Redirect_(name)
*/


#define REDIRECT(name, ret, ...) typedef ret(WINAPI *##name##_t)(__VA_ARGS__); \
	static name##_t SR_Original_##name; \
	ret WINAPI SR_Redirect_##name(__VA_ARGS__)

REDIRECT(CreateFileA, HANDLE, LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

REDIRECT(CreateFileW, HANDLE, LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

REDIRECT(OpenFile, HFILE, LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_OpenFile(lpFileName, lpReOpenBuff, uStyle);
}

REDIRECT(GetPrivateProfileStringA, DWORD, LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

REDIRECT(GetPrivateProfileStringW, DWORD, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

REDIRECT(GetPrivateProfileIntA, UINT, LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
}

REDIRECT(GetPrivateProfileIntW, UINT, LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, lpFileName);
}

REDIRECT(GetPrivateProfileSectionA, DWORD, LPCSTR lpAppName, LPSTR  lpReturnedString, DWORD  nSize, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetPrivateProfileSectionA(lpAppName, lpReturnedString, nSize, lpFileName);
}

REDIRECT(GetPrivateProfileSectionW, DWORD, LPCWSTR lpAppName, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetPrivateProfileSectionW(lpAppName, lpReturnedString, nSize, lpFileName);
}

REDIRECT(GetPrivateProfileStructA, BOOL, LPCSTR lpszSection, LPCSTR lpszKey, LPVOID lpStruct, UINT   uSizeStruct, LPCSTR szFile)
{
	szFile = TryRedirectA(szFile);
	return SR_Original_GetPrivateProfileStructA(lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
}

REDIRECT(GetPrivateProfileStructW, BOOL, LPCWSTR lpszSection, LPCWSTR lpszKey, LPVOID lpStruct, UINT uSizeStruct, LPCWSTR szFile)
{
	szFile = TryRedirectW(szFile);
	return SR_Original_GetPrivateProfileStructW(lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
}

REDIRECT(GetPrivateProfileSectionNamesA, DWORD, LPSTR  lpszReturnBuffer, DWORD  nSize, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetPrivateProfileSectionNamesA(lpszReturnBuffer, nSize, lpFileName);
}

REDIRECT(GetPrivateProfileSectionNamesW, DWORD, LPWSTR  lpszReturnBuffer, DWORD   nSize, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetPrivateProfileSectionNamesW(lpszReturnBuffer, nSize, lpFileName);
}

REDIRECT(WritePrivateProfileSectionA, BOOL, LPCSTR lpAppName, LPCSTR lpString, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_WritePrivateProfileSectionA(lpAppName, lpString, lpFileName);
}

REDIRECT(WritePrivateProfileSectionW, BOOL, LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_WritePrivateProfileSectionW(lpAppName, lpString, lpFileName);
}

REDIRECT(WritePrivateProfileStringA, BOOL, LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

REDIRECT(WritePrivateProfileStringW, BOOL, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_WritePrivateProfileStringW(lpAppName, lpKeyName, lpString, lpFileName);
}

REDIRECT(WritePrivateProfileStructA, BOOL, LPCSTR lpszSection, LPCSTR lpszKey, LPVOID lpStruct, UINT   uSizeStruct, LPCSTR szFile)
{
	szFile = TryRedirectA(szFile);
	return SR_Original_WritePrivateProfileStructA(lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
}

REDIRECT(WritePrivateProfileStructW, BOOL, LPCWSTR lpszSection, LPCWSTR lpszKey, LPVOID  lpStruct, UINT    uSizeStruct, LPCWSTR szFile)
{
	szFile = TryRedirectW(szFile);
	return SR_Original_WritePrivateProfileStructW(lpszSection, lpszKey, lpStruct, uSizeStruct, szFile);
}

REDIRECT(GetFileAttributesA, DWORD, LPCSTR lpFileName)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetFileAttributesA(lpFileName);
}

REDIRECT(GetFileAttributesW, DWORD, LPCWSTR lpFileName)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetFileAttributesW(lpFileName);
}

REDIRECT(GetFileAttributesExA, BOOL, LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_GetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation);
}

REDIRECT(GetFileAttributesExW, BOOL, LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
}

REDIRECT(SetFileAttributesA, BOOL, LPCSTR lpFileName, DWORD dwFileAttributes)
{
	lpFileName = TryRedirectA(lpFileName);
	return SR_Original_SetFileAttributesA(lpFileName, dwFileAttributes);
}

REDIRECT(SetFileAttributesW, BOOL, LPCWSTR lpFileName, DWORD dwFileAttributes)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_SetFileAttributesW(lpFileName, dwFileAttributes);
}

#undef REDIRECT

// +==================================================================+
// |                      End Redirect functions                      |
// +==================================================================+

static SR_Redirection* Redirections = NULL;

// Adds a WinAPI function redirection to the list of redirections.
static void AddRedirection(PVOID* original, PVOID redirected, const wchar_t* name)
{
	SR_Redirection* current = current = calloc(1, sizeof(SR_Redirection));
	current->Next = Redirections;

	current->Original = original;
	current->Redirected = redirected;
	current->Name = name;

	Redirections = current;
}

static void CreateNewPaths()
{
	// ==== Paths relative to the Documents folder ===

	wchar_t* documentsPath;
	SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &documentsPath);

	size_t documentsPathLen = wcslen(documentsPath);

	// Skyrim.ini
	size_t newIniSize = documentsPathLen + wcslen(SR_INI_PATH) + 1;
	NewIniW = calloc(newIniSize, sizeof(wchar_t));
	wcscpy_s(NewIniW, newIniSize, documentsPath);
	wcscat_s(NewIniW, newIniSize, SR_INI_PATH);

	NewIniA = SR_Utf16ToUtf8(NewIniW);

	SR_INFO("Skyrim.ini will be redirected to %ls", NewIniW);

	// SkyrimPrefs.ini
	size_t newPrefsIniSize = documentsPathLen + wcslen(SR_PREFS_INI_PATH) + 1;
	NewPrefsIniW = calloc(newPrefsIniSize, sizeof(wchar_t));
	wcscpy_s(NewPrefsIniW, newPrefsIniSize, documentsPath);
	wcscat_s(NewPrefsIniW, newPrefsIniSize, SR_PREFS_INI_PATH);

	NewPrefsIniA = SR_Utf16ToUtf8(NewPrefsIniW);

	SR_INFO("SkyrimPrefs.ini will be redirected to %ls", NewPrefsIniW);

	CoTaskMemFree(documentsPath);

	// ==== Paths relative to the Local AppData folder ===

	wchar_t* localAppDataPath;
	SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &localAppDataPath);

	size_t localAppDataPathLen = wcslen(localAppDataPath);

	// plugins.txt
	size_t newPluginsSize = localAppDataPathLen + wcslen(SR_PLUGINS_PATH) + 1;
	NewPluginsW = calloc(newPluginsSize, sizeof(wchar_t));
	wcscpy_s(NewPluginsW, newPluginsSize, localAppDataPath);
	wcscat_s(NewPluginsW, newPluginsSize, SR_PLUGINS_PATH);

	NewPluginsA = SR_Utf16ToUtf8(NewPluginsW);

	SR_INFO("plugins.txt will be redirect to %ls", NewPluginsW);

	CoTaskMemFree(localAppDataPath);
}

#define ADD_REDIRECT(name) SR_Original_##name = (name##_t)GetProcAddress(kernel32, #name); AddRedirection(&(PVOID)SR_Original_##name, (PVOID)SR_Redirect_##name, L#name)
#define ADD_REDIRECTAW(name) ADD_REDIRECT(name##A); ADD_REDIRECT(name##W)

static void CreateRedirections()
{
	SR_FreeRedirections();
	CreateNewPaths();

	HMODULE kernel32 = GetModuleHandleW(L"kernel32");

	ADD_REDIRECTAW(CreateFile);
	ADD_REDIRECT(OpenFile);

	ADD_REDIRECTAW(GetPrivateProfileSection);
	ADD_REDIRECTAW(GetPrivateProfileString);
	ADD_REDIRECTAW(GetPrivateProfileInt);
	ADD_REDIRECTAW(GetPrivateProfileStruct);
	ADD_REDIRECTAW(GetPrivateProfileSectionNames);

	ADD_REDIRECTAW(WritePrivateProfileSection);
	ADD_REDIRECTAW(WritePrivateProfileString);
	ADD_REDIRECTAW(WritePrivateProfileStruct);

	ADD_REDIRECTAW(GetFileAttributes);
	ADD_REDIRECTAW(GetFileAttributesEx);
	ADD_REDIRECTAW(SetFileAttributes);

	CloseHandle(kernel32);

}

#undef ADD_REDIRECTAW
#undef ADD_REDIRECT

SR_Redirection* SR_GetRedirections()
{
	if (Redirections == NULL) CreateRedirections();
	return Redirections;
}

static void FreeNewPaths()
{
	free(NewIniA);
	NewIniA = NULL;

	free(NewIniW);
	NewIniW = NULL;

	free(NewPrefsIniA);
	NewPrefsIniA = NULL;

	free(NewPrefsIniW);
	NewPrefsIniW = NULL;

	free(NewPluginsW);
	NewPluginsW = NULL;

	free(NewPluginsA);
	NewPluginsA = NULL;
}

void SR_FreeRedirections()
{
	while (Redirections != NULL)
	{
		SR_Redirection* previous = Redirections;
		Redirections = Redirections->Next;
		free(previous);
	}

	FreeNewPaths();
}