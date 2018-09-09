#include "Redirections.h"
#include "Logging.h"
#include "StringUtils.h"
#include <ShlObj.h>
#include <stdbool.h>

// +==================================================================+
// |                         Redirect support                         |
// +==================================================================+

#define NEW_INIS_COMMON_SUFFIX L"\\My Games\\Enderal\\"
#define NEW_INI_SUFFIX NEW_INIS_COMMON_SUFFIX L"Enderal.ini"
#define NEW_PREFS_INI_SUFFIX NEW_INIS_COMMON_SUFFIX L"EnderalPrefs.ini"

static wchar_t* NewIniW = NULL;
static wchar_t* NewPrefsIniW = NULL;

static char* NewIniA = NULL;
static char* NewPrefsIniA = NULL;

static const wchar_t* TryRedirectW(const wchar_t* input)
{
	size_t inputSize = wcslen(input) + 1;
	wchar_t* inputUpper = calloc(inputSize, sizeof(wchar_t));
	wcscpy_s(inputUpper, inputSize, input);
	_wcsupr_s(inputUpper, inputSize);

	const wchar_t* result = input;
	if (wcsstr(inputUpper, L"SKYRIM.INI")) result = NewIniW;
	else if (wcsstr(inputUpper, L"SKYRIMPREFS.INI")) result = NewPrefsIniW;

	free(inputUpper);

	return result;
}

static const char* TryRedirectA(const char* input)
{
	size_t inputSize = strlen(input) + 1;
	char* inputUpper = calloc(inputSize, sizeof(char));
	strcpy_s(inputUpper, inputSize, input);
	_strupr_s(inputUpper, inputSize);

	const char* result = input;
	if (strstr(inputUpper, "SKYRIM.INI")) result = NewIniA;
	else if (strstr(inputUpper, "SKYRIMPREFS.INI")) result = NewPrefsIniA;

	free(inputUpper);

	return result;
}

// +==================================================================+
// |                        Redirect functions                        |
// +==================================================================+

#define REDIRECT(name, ret, ...) typedef ret(WINAPI *##name##_t)(__VA_ARGS__); \
	static name##_t SR_Original_##name; \
	ret WINAPI SR_##name(__VA_ARGS__)

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

REDIRECT(CreateFile2, HANDLE, LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_CreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
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

REDIRECT(GetFileAttributesExW, BOOL,LPCWSTR lpFileName,GET_FILEEX_INFO_LEVELS fInfoLevelId,LPVOID lpFileInformation)
{
	lpFileName = TryRedirectW(lpFileName);
	return SR_Original_GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
}

#undef REDIRECT

// +==================================================================+
// |                      End Redirect functions                      |
// +==================================================================+

static SR_Redirection* Redirections = NULL;

static void AddRedirection(PVOID* original, PVOID redirected, const wchar_t* name)
{
	SR_Redirection* current = current = calloc(1, sizeof(SR_Redirection));
	current->Next = Redirections;

	current->Original = original;
	current->Redirected = redirected;
	current->Name = name;

	Redirections = current;
}

#define ADD_REDIRECT(name) SR_Original_##name = (name##_t)GetProcAddress(kernel32, #name); AddRedirection(&(PVOID)SR_Original_##name, (PVOID)SR_##name, L#name)
#define ADD_REDIRECTAW(name) ADD_REDIRECT(name##A); ADD_REDIRECT(name##W)

static void CreateNewInis()
{
	wchar_t* documentsPath;
	SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &documentsPath);

	size_t documentsPathLen = wcslen(documentsPath);

	size_t newIniSize = documentsPathLen + wcslen(NEW_INI_SUFFIX) + 1;
	NewIniW = calloc(newIniSize, sizeof(wchar_t));
	wcscpy_s(NewIniW, newIniSize, documentsPath);
	wcscat_s(NewIniW, newIniSize, NEW_INI_SUFFIX);

	NewIniA = SR_Utf16ToUtf8(NewIniW);

	SR_INFO("Skyrim.ini will be redirected to %ls", NewIniW);

	size_t newPrefsIniSize = documentsPathLen + wcslen(NEW_PREFS_INI_SUFFIX) + 1;
	NewPrefsIniW = calloc(newPrefsIniSize, sizeof(wchar_t));
	wcscpy_s(NewPrefsIniW, newPrefsIniSize, documentsPath);
	wcscat_s(NewPrefsIniW, newPrefsIniSize, NEW_PREFS_INI_SUFFIX);

	NewPrefsIniA = SR_Utf16ToUtf8(NewPrefsIniW);

	SR_INFO("SkyrimPrefs.ini will be redirected to %ls", NewPrefsIniW);

	CoTaskMemFree(documentsPath);
}

static void CreateRedirections()
{
	SR_FreeRedirections();
	CreateNewInis();

	HMODULE kernel32 = GetModuleHandleW(L"kernel32");

	ADD_REDIRECTAW(CreateFile);
	ADD_REDIRECT(CreateFile2);
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

	CloseHandle(kernel32);

}

#undef ADD_REDIRECTAW
#undef ADD_REDIRECT

SR_Redirection* SR_GetRedirections()
{
	if (Redirections == NULL) CreateRedirections();
	return Redirections;
}

static void FreeNewInis()
{
	free(NewIniA);
	NewIniA = NULL;
	free(NewIniW);
	NewIniW = NULL;

	free(NewPrefsIniA);
	NewPrefsIniA = NULL;
	free(NewPrefsIniW);
	NewPrefsIniW = NULL;
}

void SR_FreeRedirections()
{
	while (Redirections != NULL)
	{
		SR_Redirection* previous = Redirections;
		Redirections = Redirections->Next;
		free(previous);
	}

	FreeNewInis();	
}