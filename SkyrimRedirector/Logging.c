#include "Logging.h"
#include "Config.h"
#include "StringUtils.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

#define SR_LOG_LEVEL SR_LOG_LEVEL_TRACE

#define SR_LOG_FILE_NAME L"SkyrimRedirector.log"
#define SR_LOG_HEADER_SIZE 32

static HANDLE LogFile = INVALID_HANDLE_VALUE;

static wchar_t* VFormat(const wchar_t* format, va_list args)
{
	int needed = _vscwprintf(format, args);

	size_t bufferLen = needed + 1;
	wchar_t* buffer = calloc(bufferLen, sizeof(wchar_t));
	vswprintf_s(buffer, bufferLen, format, args);

	return buffer;
}

static wchar_t* Format(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	wchar_t* result = VFormat(format, args);
	va_end(args);

	return result;
}

void SR_StartLogging()
{
	if (LogFile != INVALID_HANDLE_VALUE)
	{
		SR_WARN("Tried to start logging twice");
		return;
	}

	// Exponentially increase the buffer size until it fits the full module file name
	size_t moduleFilePathLen = 16;
	wchar_t* moduleFilePath = NULL;
	do
	{
		moduleFilePathLen *= 2;
		moduleFilePath = realloc(moduleFilePath, moduleFilePathLen * sizeof(wchar_t));
		GetModuleFileNameW(NULL, moduleFilePath, moduleFilePathLen);

	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	// Start Dir is the folder of the module file -- the module file path until its last path separator
	ptrdiff_t startDirLen = wcsrchr(moduleFilePath, L'\\') - &moduleFilePath[0];

	// Log File = Start Dir \ Base Dir \ File Name '\0'
	rsize_t logFilePathLen = startDirLen + 1 + wcslen(SR_BASE_DIR) + 1 + wcslen(SR_LOG_FILE_NAME) + 1;
	wchar_t* logFilePath = calloc(logFilePathLen, sizeof(wchar_t));

	wcsncat_s(logFilePath, logFilePathLen, moduleFilePath, startDirLen);
	free(moduleFilePath);

	wcscat_s(logFilePath, logFilePathLen, L"\\");
	wcscat_s(logFilePath, logFilePathLen, SR_BASE_DIR);
	wcscat_s(logFilePath, logFilePathLen, L"\\");
	wcscat_s(logFilePath, logFilePathLen, SR_LOG_FILE_NAME);

	LogFile = CreateFileW(
		logFilePath,
		GENERIC_WRITE,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	free(logFilePath);

	SR_INFO("Skyrim Redirector by Davipb started. github.com/Davipb/SkyrimRedirector");
}

void SR_StopLogging()
{
	if (LogFile == INVALID_HANDLE_VALUE) return;

	SR_INFO("Skyrim Redirector stopped.");
	CloseHandle(LogFile);
	LogFile = INVALID_HANDLE_VALUE;
}

static const wchar_t* NameOf(int level)
{
	switch (level)
	{
	case SR_LOG_LEVEL_TRACE: return L"TRACE";
	case SR_LOG_LEVEL_DEBUG: return L"DEBUG";
	case SR_LOG_LEVEL_INFO: return L"INFO";
	case SR_LOG_LEVEL_WARN: return L"WARN";
	case SR_LOG_LEVEL_ERROR: return L"ERROR";
	default: return L"????";
	}
}



static bool LogLock = false;

void SR_Log(int level, const wchar_t* message, ...)
{
	if (level < SR_LOG_LEVEL || LogFile == INVALID_HANDLE_VALUE) return;
	if (LogLock) return;
	LogLock = true;


	SYSTEMTIME time;
	GetLocalTime(&time);

	// yyyy-MM-dd HH:mm:ss.SSS [LEVEL] 
	wchar_t* buffer = Format(L"%04u-%02u-%02u %02u:%02u:%02u.%03u [%-5s] ", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, NameOf(level));

	va_list args;
	va_start(args, message);
	wchar_t* fmtMessage = VFormat(message, args);
	va_end(args);

	// Date + Message + "\r\n" + '\0'
	size_t totalLen = wcslen(buffer) + wcslen(fmtMessage) + 3;
	buffer = realloc(buffer, totalLen * sizeof(wchar_t));
	wcscat_s(buffer, totalLen, fmtMessage);
	free(fmtMessage);
	wcscat_s(buffer, totalLen, L"\r\n");

	char* result = SR_Utf16ToUtf8(buffer);
	free(buffer);

	WriteFile(LogFile, result, strlen(result), NULL, NULL);
	free(result);

	LogLock = false;
}

#undef SR_LOG_LEVEL
#undef SR_LOG_FILE_NAME