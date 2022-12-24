#include <Windows.h>
#include <ShlObj.h>
#include <stdio.h>
#include <stdbool.h>
#include "..\SkyrimRedirector\PluginAPI.h"
#include "..\SkyrimRedirector\PlatformDefinitions.h"

#define NUMBER_OF_TESTS 9

typedef bool(*SKSEPlugin_Load_t)(const SKSEInterface*);

#define FOREGROUND_GRAY FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_WHITE FOREGROUND_GRAY | FOREGROUND_INTENSITY

#define FOREGROUND_BRIGHT_RED FOREGROUND_RED | FOREGROUND_INTENSITY
#define FOREGROUND_BRIGHT_GREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define FOREGROUND_BRIGHT_BLUE FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define FOREGROUND_NORMAL FOREGROUND_GRAY

#define TRY(action) if(!action) return false

#define SKYRIM_INI &FOLDERID_Documents, L"\\My Games\\Skyrim" SR_FOLDER_SUFFIX_W L"\\Skyrim.ini"
#define SKYRIM_PREFS_INI &FOLDERID_Documents, L"\\My Games\\Skyrim" SR_FOLDER_SUFFIX_W L"\\SkyrimPrefs.ini"
#define SKYRIM_PLUGINS &FOLDERID_LocalAppData, L"\\Skyrim" SR_FOLDER_SUFFIX_W L"\\plugins.txt"

#define TRY_READ_INI(shouldRedirect) TRY(TryRead(SKYRIM_INI, &OriginalInfo.Ini, shouldRedirect))
#define TRY_READ_PREFS_INI(shouldRedirect) TRY(TryRead(SKYRIM_PREFS_INI, &OriginalInfo.PrefsIni, shouldRedirect))
#define TRY_READ_PLUGINS(shouldRedirect) TRY(TryRead(SKYRIM_PLUGINS, &OriginalInfo.Plugins, shouldRedirect))

#define TRY_READ_ALL(shouldRedirect) \
	TRY_READ_INI(shouldRedirect);\
	wprintf_s(L"\n");\
	TRY_READ_PREFS_INI(shouldRedirect);\
	wprintf_s(L"\n");\
	TRY_READ_PLUGINS(shouldRedirect);\
	wprintf_s(L"\n")

#define RETURN_ERROR(message, ...) \
{\
	wchar_t* error = FormatLastError();\
	SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_RED);\
	wprintf_s(L##message, __VA_ARGS__);\
	wprintf_s(L"\nError: %ls\n", error);\
	LocalFree(error);\
	SetConsoleTextAttribute(StdOut, FOREGROUND_NORMAL);\
	return false;\
}

#define TEST_HEADER(message, shouldRedirect) \
	SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_BLUE);\
	wprintf_s(L"\n" message L": Files should ");\
	if (!shouldRedirect)\
		wprintf_s(L"not ");\
	wprintf_s(L"redirect\n");\
	SetConsoleTextAttribute(StdOut, FOREGROUND_NORMAL);


#define PERFORM_TEST(header, shouldRedirect) \
	TEST_HEADER(header, shouldRedirect);\
	TRY_READ_ALL(shouldRedirect)

HANDLE StdOut;
int TestsPassed = 0;

struct
{
	BY_HANDLE_FILE_INFORMATION Ini;
	BY_HANDLE_FILE_INFORMATION PrefsIni;
	BY_HANDLE_FILE_INFORMATION Plugins;
} OriginalInfo;

bool FileInformationEquals(BY_HANDLE_FILE_INFORMATION* a, BY_HANDLE_FILE_INFORMATION* b)
{
	return
		a->dwVolumeSerialNumber == b->dwVolumeSerialNumber &&
		a->nFileIndexLow == b->nFileIndexLow &&
		a->nFileIndexHigh == b->nFileIndexHigh;
}

wchar_t* FormatLastError()
{
	DWORD error = GetLastError();
	if (error == NO_ERROR) return NULL;

	wchar_t* message;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		0,
		(LPWSTR)&message,
		0,
		NULL
	);

	return message;
}

bool GetKnownPath(const KNOWNFOLDERID* const refid, const wchar_t* const suffix, wchar_t** result)
{
	wchar_t* baseFolder;
	if (SHGetKnownFolderPath(refid, 0, NULL, &baseFolder) != NO_ERROR)
		RETURN_ERROR("Unable to get full path for %ls", suffix);

	size_t filePathSize = wcslen(baseFolder) + wcslen(suffix) + 1;
	wchar_t* filePath = calloc(filePathSize, sizeof(wchar_t));

	wcscpy_s(filePath, filePathSize, baseFolder);
	CoTaskMemFree(baseFolder);

	wcscat_s(filePath, filePathSize, suffix);
	*result = filePath;

	return true;
}

bool OpenExisting(const wchar_t* const file, HANDLE* result)
{
	HANDLE handle = CreateFileW(
		file,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE) RETURN_ERROR("Unable to open %ls", file);

	*result = handle;
	return true;
}

bool TryRead(const KNOWNFOLDERID* const refid, const wchar_t* const suffix, BY_HANDLE_FILE_INFORMATION* original, bool shouldRedirect)
{
	wchar_t* filePath;
	TRY(GetKnownPath(refid, suffix, &filePath));

	wprintf_s(L"    Trying to read: %ls\n", filePath);

	HANDLE handle;
	TRY(OpenExisting(filePath, &handle));

	wchar_t actualPath[2048];
	if (GetFinalPathNameByHandleW(handle, &actualPath[0], 2047, 0) == 0)
		RETURN_ERROR("Unable to get path name of opened file");

	wprintf_s(L"    I actually read: %ls\n", actualPath);

	BY_HANDLE_FILE_INFORMATION info;
	if (!GetFileInformationByHandle(handle, &info)) RETURN_ERROR("Unable to get file information for %ls", actualPath);

	bool redirected = !FileInformationEquals(original, &info);

	if (redirected)
		wprintf_s(L"    File has been redirected\n");
	else
		wprintf_s(L"    File was not redirected\n");

	if (redirected != shouldRedirect)
	{
		SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_RED);
		wprintf_s(L"    X Failed\n");
	}
	else
	{
		SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_GREEN);
		wprintf_s(L"    Y Passed\n");
		TestsPassed++;
	}

	SetConsoleTextAttribute(StdOut, FOREGROUND_NORMAL);

	CloseHandle(handle);

	return true;
}

bool MoveRedirector()
{
	DWORD dllAttributes = GetFileAttributesW(L"SkyrimRedirector.dll");
	if (dllAttributes == INVALID_FILE_ATTRIBUTES)
		return true;

	CreateDirectoryW(L"Data", NULL);
	CreateDirectoryW(L"Data\\SKSE", NULL);
	CreateDirectoryW(L"Data\\SKSE\\Plugins", NULL);
	DeleteFileW(L"Data\\SKSE\\Plugins\\SkyrimRedirector.dll");

	if (CopyFileW(L"SkyrimRedirector.dll", L"Data\\SKSE\\Plugins\\SkyrimRedirector.dll", FALSE))
		return true;

	RETURN_ERROR("Unable to copy the redirector to its proper folder");
}

bool ExecuteTests()
{
	TRY(MoveRedirector());

	HMODULE redirector = LoadLibraryW(L"Data\\SKSE\\Plugins\\SkyrimRedirector");
	if (redirector == NULL) RETURN_ERROR("Unable find the redirector. Is it in Data\\SKSE\\Plugins?");

	SKSEPlugin_Load_t load = (SKSEPlugin_Load_t)GetProcAddress(redirector, "SKSEPlugin_Load");
	if (load == NULL) RETURN_ERROR("Unable to find SKSEPlugin_Load in the redirector");

	PERFORM_TEST(L"Redirector has been attached but not loaded yet", false);

	if (!load(NULL)) RETURN_ERROR("The redirector failed to load");

	PERFORM_TEST(L"Redirector has been loaded", true);

	if (!FreeLibrary(redirector)) RETURN_ERROR("The redirector failed to unload");

	PERFORM_TEST(L"Redirector has been detached", false);

	if (TestsPassed == NUMBER_OF_TESTS)
		SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_GREEN);
	else
		SetConsoleTextAttribute(StdOut, FOREGROUND_BRIGHT_RED);

	wprintf_s(L"\n[ %d / %d ] Tests passed\n", TestsPassed, NUMBER_OF_TESTS);

	if (TestsPassed == NUMBER_OF_TESTS)
		wprintf_s(L"Redirector is working properly\n");
	else
		wprintf_s(L"Redirector is not working properly\n");

	SetConsoleTextAttribute(StdOut, FOREGROUND_NORMAL);
	return true;
}

bool LoadInfo(const KNOWNFOLDERID* const refid, const wchar_t* const suffix, BY_HANDLE_FILE_INFORMATION* output)
{
	wchar_t* path;
	TRY(GetKnownPath(refid, suffix, &path));

	HANDLE handle;
	TRY(OpenExisting(path, &handle));

	if (!GetFileInformationByHandle(handle, output)) RETURN_ERROR("Unable to load file info for %ls", path);

	free(path);
	return true;
}

bool LoadOriginals()
{
	TRY(LoadInfo(SKYRIM_INI, &OriginalInfo.Ini));
	TRY(LoadInfo(SKYRIM_PREFS_INI, &OriginalInfo.PrefsIni));
	TRY(LoadInfo(SKYRIM_PLUGINS, &OriginalInfo.Plugins));

	return true;
}

bool Execute()
{
	StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (StdOut == INVALID_HANDLE_VALUE)
		RETURN_ERROR("Unable to load console handle");

	TRY(LoadOriginals());
	TRY(ExecuteTests());
	return true;
}

void main()
{
	Execute();
	wprintf_s(L"\n\n");
	system("pause");
}
