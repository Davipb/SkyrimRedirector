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

// Gets the current UserConfig
SR_UserConfig* SR_GetUserConfig();

// Frees all resources allocated to the user config
// Any pointers returned by SR_GetUserConfig() must be considered invalid 
// after this function is called.
void SR_FreeUserConfig();
