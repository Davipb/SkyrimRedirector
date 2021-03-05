#include "SR_Base.h"
#include "Logging.h"
#include "Config.h"
#include "StringUtils.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

#define SR_LOG_HEADER_SIZE 32

static HANDLE LogFile = INVALID_HANDLE_VALUE;

static wchar_t* VFormat(const wchar_t* format, va_list args)
{
	int needed = _vscwprintf(format, args);

	size_t bufferLen = (size_t)needed + 1;
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

	const SR_UserConfig* config = SR_GetUserConfig();
	if (config->Logging.Level < SR_LOG_LEVEL_OFF)
	{
		DWORD creationDisposition = CREATE_ALWAYS;
		if (config->Logging.Append)
			creationDisposition = OPEN_ALWAYS;

		LogFile = CreateFileW(
			config->Logging.File,
			FILE_APPEND_DATA,
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			creationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}

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

void SR_Log(uint8_t level, const wchar_t* message, ...)
{
	if (level < SR_GetUserConfig()->Logging.Level || LogFile == INVALID_HANDLE_VALUE) return;

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

	DWORD bytesWritten;
	WriteFile(LogFile, result, (DWORD)strlen(result), &bytesWritten, NULL);
	free(result);
}
