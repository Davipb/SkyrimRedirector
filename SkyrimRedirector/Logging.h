#pragma once
#include <wchar.h>
#include <stdint.h>

void SR_StartLogging();
void SR_StopLogging();
void SR_Log(uint8_t level, const wchar_t* message, ...);

enum
{
	SR_LOG_LEVEL_TRACE = 0,
	SR_LOG_LEVEL_DEBUG,
	SR_LOG_LEVEL_INFO,
	SR_LOG_LEVEL_WARN,
	SR_LOG_LEVEL_ERROR,
	SR_LOG_LEVEL_OFF
};

#define SR_TRACE(message, ...) SR_Log(SR_LOG_LEVEL_TRACE, L##message, __VA_ARGS__)
#define SR_DEBUG(message, ...) SR_Log(SR_LOG_LEVEL_DEBUG, L##message, __VA_ARGS__)
#define SR_INFO(message, ...) SR_Log(SR_LOG_LEVEL_INFO, L##message, __VA_ARGS__)
#define SR_WARN(message, ...) SR_Log(SR_LOG_LEVEL_WARN, L##message, __VA_ARGS__)
#define SR_ERROR(message, ...) SR_Log(SR_LOG_LEVEL_ERROR, L##message, __VA_ARGS__)
