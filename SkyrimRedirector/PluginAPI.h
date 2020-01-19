#pragma once
#include <stdint.h>

/*
*  +=================================================================================================+
*  | This file contains a slimmed-down version of the PluginAPI.h found in SKSE build version 1.7.3  |
*  | The file has been altered slightly to work better in a pure C environment.                      |
*  +=================================================================================================+
*/

typedef struct
{
	uint32_t skseVersion;
	uint32_t runtimeVersion;
	uint32_t editorVersion;
	uint32_t isEditor;

	// We won't use these
	void* QueryInterface;
	void* GetPluginHandle;
	void* GetReleaseIndex;

} SKSEInterface;


// The version of the PluginInfo struct when the program was compiled
#define PLUGIN_INFO_VERSION 1

typedef struct
{
	uint32_t infoVersion;
	const char* name;
	uint32_t version;

} PluginInfo;
