#include "Config.h"
#include "StringUtils.h"
#include "Logging.h"
#include <stdlib.h>
#include <ShlObj.h>
#include <Windows.h>

// The base directory, relative to the current module path, where all logs and configs from this plugin are stored
#define SR_BASE_DIR L"Data\\SKSE\\Plugins"

// The file path, relative to SR_BASE_DIR, where the configuration is stored
#define SR_CONFIG_FILE L"\\SkyrimRedirector.ini"

// The default file path, relative to SR_BASE_DIR, where the log file is stored
#define SR_DEFAULT_LOG_FILE L"\\SkyrimRedirector.log"

// The default log level, in text format
#ifdef DEBUG
#define SR_DEFAULT_LOG_LEVEL L"TRACE"
#else
#define SR_DEFAULT_LOG_LEVEL L"INFO"
#endif

// The default file path to where Skyrim.ini will be redirected, relative to the Documents folder.
#define SR_DEFAULT_REDIRECTION_INI L"\\My Games\\Enderal\\Enderal.ini"

// The default file path to where SkyrimPrefs.ini will be redirected, relative to the Documents folder.
#define SR_DEFAULT_REDIRECTION_PREFS_INI L"\\My Games\\Enderal\\EnderalPrefs.ini"

// The default file path to where plugins.txt will be redirected, relative to the Local AppData folder.
#define SR_DEFAULT_REDIRECTION_PLUGINS L"\\Enderal\\plugins.txt"


// Reads a string stored in a .ini fully
// If the specified key doesn't exist, this returns null.
// Otherwise, the returned string is allocated dynamically and must be freed.
static wchar_t* SR_ReadIniString(const wchar_t* section, const wchar_t* key, const wchar_t* file)
{
	// Exponentially increase the buffer size until it fits the full module file name
	size_t resultSize = 16;
	wchar_t* result = NULL;
	DWORD actualLen;
	do
	{
		resultSize *= 2;
		result = realloc(result, resultSize * sizeof(wchar_t));
		actualLen = GetPrivateProfileStringW(section, key, NULL, result, resultSize, file);

	} while (actualLen >= resultSize - 1);

	if (actualLen == 0) // Key doesn't exist, defaults to empty string
	{
		free(result);
		return NULL;
	}

	// Trim buffer to fit string exactly
	result = realloc(result, (actualLen + 1) * sizeof(wchar_t));

	return result;
}


// Gets the full module file path of the currently running executable.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetModuleFilePath()
{
	// Exponentially increase the buffer size until it fits the full module file name
	size_t moduleFilePathSize = 16;
	wchar_t* moduleFilePath = NULL;
	do
	{
		moduleFilePathSize *= 2;
		moduleFilePath = realloc(moduleFilePath, moduleFilePathSize * sizeof(wchar_t));
		GetModuleFileNameW(NULL, moduleFilePath, moduleFilePathSize);

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	// Trim buffer to fit string exactly
	size_t actualSize = wcslen(moduleFilePath) + 1;
	moduleFilePath = realloc(moduleFilePath, actualSize * sizeof(wchar_t));

	return moduleFilePath;
}

// Gets the base directory where the config and default log file will be stored.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetBaseDir()
{
	wchar_t* moduleFilePath = SR_GetModuleFilePath();

	// Start Dir is the folder of the module file -- the module file path until its last path separator
	ptrdiff_t startDirLen = wcsrchr(moduleFilePath, L'\\') - &moduleFilePath[0];

	// Base Dir = Start Dir \ Relative Base Dir '\0'
	rsize_t baseDirSize = startDirLen + 1 + wcslen(SR_BASE_DIR) + 1;
	wchar_t* baseDir = calloc(baseDirSize, sizeof(wchar_t));

	wcsncpy_s(baseDir, baseDirSize, moduleFilePath, startDirLen);
	free(moduleFilePath);

	wcscat_s(baseDir, baseDirSize, L"\\");
	wcscat_s(baseDir, baseDirSize, SR_BASE_DIR);

	return baseDir;
}

// Gets the file path of the configuration file.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetConfigFile()
{
	wchar_t* baseDir = SR_GetBaseDir();
	wchar_t* result = SR_Concat(2, baseDir, SR_CONFIG_FILE);
	free(baseDir);

	return result;
}

// Gets the default file path of the log file.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetDefaultLogFile()
{
	wchar_t* baseDir = SR_GetBaseDir();
	wchar_t* result = SR_Concat(2, baseDir, SR_DEFAULT_LOG_FILE);
	free(baseDir);

	return result;
}

// Gets a known folder for the current user.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetKnownFolder(const KNOWNFOLDERID* const rfid)
{
	wchar_t* documentsPath;
	SHGetKnownFolderPath(rfid, 0, NULL, &documentsPath);

	// Copy to a standard C-duplicated string to allow the caller to free this string with `free`
	// instead of CoTaskMemFree
	wchar_t* result = _wcsdup(documentsPath);
	CoTaskMemFree(documentsPath);

	return result;
}

// Gets the default file path of the redirected .ini file.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetDefaultRedirectionIni()
{
	wchar_t* documentsPath = SR_GetKnownFolder(&FOLDERID_Documents);

	wchar_t* result = SR_Concat(2, documentsPath, SR_DEFAULT_REDIRECTION_INI);
	free(documentsPath);

	return result;
}

// Gets the default file path of the redirected prefs .ini file.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetDefaultRedirectionPrefsIni()
{
	wchar_t* documentsPath = SR_GetKnownFolder(&FOLDERID_Documents);

	wchar_t* result = SR_Concat(2, documentsPath, SR_DEFAULT_REDIRECTION_PREFS_INI);
	free(documentsPath);

	return result;
}

// Gets the default file path of the redirected plugins file.
// The returned string is allocated dynamically and must be freed.
static wchar_t* SR_GetDefaultRedirectionPlugins()
{
	wchar_t* documentsPath = SR_GetKnownFolder(&FOLDERID_LocalAppData);

	wchar_t* result = SR_Concat(2, documentsPath, SR_DEFAULT_REDIRECTION_PLUGINS);
	free(documentsPath);

	return result;
}

static uint8_t SR_LogStringToLogLevel(const wchar_t* level)
{
	if (SR_AreCaseInsensitiveEqualW(level, L"TRACE")) return SR_LOG_LEVEL_TRACE;
	if (SR_AreCaseInsensitiveEqualW(level, L"DEBUG")) return SR_LOG_LEVEL_DEBUG;
	if (SR_AreCaseInsensitiveEqualW(level, L"INFO")) return SR_LOG_LEVEL_INFO;
	if (SR_AreCaseInsensitiveEqualW(level, L"WARN")) return SR_LOG_LEVEL_WARN;
	if (SR_AreCaseInsensitiveEqualW(level, L"ERROR")) return SR_LOG_LEVEL_ERROR;
	return SR_LOG_LEVEL_OFF;
}

static SR_UserConfig* UserConfig = NULL;

/*

This macro is to be used as follows:

  READOR("Section", "Key", GetDefaultValue());
  someVar = read;

It does the following:
	
  1. Call SR_ReadIniString with the variable `configFile` as the config file parameter
	 and assign its return value to the variable `read`

  2. If `read` is null, call the specified default value generator and assign it to read.

  3. Write the value of `read` to `configFile`, at the specified section and key.

*/
#define READOR(section, key, default) \
	read = SR_ReadIniString(L##section, L##key, configFile); \
	if (read == NULL) \
		read = default; \
	WritePrivateProfileStringW(L##section, L##key, read, configFile)

static void SR_LoadConfig()
{
	SR_FreeUserConfig();

	UserConfig = calloc(1, sizeof(SR_UserConfig));

	wchar_t* configFile = SR_GetConfigFile();
	wchar_t* read;

	READOR("Logging", "File", SR_GetDefaultLogFile());
	UserConfig->Logging.File = read;

	READOR("Logging", "Level", _wcsdup(SR_DEFAULT_LOG_LEVEL));
	UserConfig->Logging.Level = SR_LogStringToLogLevel(read);
	free(read);
	
	READOR("Logging", "Append", _wcsdup(L"TRUE"));
	UserConfig->Logging.Append = SR_AreCaseInsensitiveEqualW(read, L"TRUE");
	free(read);

	READOR("Redirection", "Ini", SR_GetDefaultRedirectionIni());
	UserConfig->Redirection.Ini = read;

	READOR("Redirection", "PrefsIni", SR_GetDefaultRedirectionPrefsIni());
	UserConfig->Redirection.PrefsIni = read;

	READOR("Redirection", "Plugins", SR_GetDefaultRedirectionPlugins());
	UserConfig->Redirection.Plugins = read;
	
	free(configFile);
}

#undef READOR

const SR_UserConfig* SR_GetUserConfig()
{
	if (UserConfig == NULL) SR_LoadConfig();
	return UserConfig;
}

void SR_FreeUserConfig()
{
	if (UserConfig == NULL) return;

	free(UserConfig->Logging.File);

	free(UserConfig->Redirection.Ini);
	free(UserConfig->Redirection.PrefsIni);
	free(UserConfig->Redirection.Plugins);

	free(UserConfig);
	UserConfig = NULL;
}
