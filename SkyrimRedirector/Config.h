#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	struct
	{
		wchar_t* File;
		uint8_t Level;
		bool Append;

	} Logging;

	struct
	{
		wchar_t* Ini;
		wchar_t* PrefsIni;
		wchar_t* Plugins;

	} Redirection;

} SR_UserConfig;

const SR_UserConfig* SR_GetUserConfig();
void SR_FreeUserConfig();


