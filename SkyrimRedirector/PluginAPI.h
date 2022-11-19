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
	void* GetPluginInfo;

} SKSEInterface;


// The version of the PluginInfo struct when the program was compiled
#define PLUGIN_INFO_VERSION 1

// **********************************
// flags for versionIndependence

// set this if you are using a post-AE version of the Address Library
#define VERSIONINDEPENDENT_ADDRESSLIBRARY_POSTAE 1

// set this if you exclusively use signature matching to find your addresses and have NO HARDCODED ADDRESSES
#define VERSIONINDEPENDENT_SIGNATURES 2

// set this if you are using 1.6.629+ compatible structure layout (likely provided by CommonLib/SKSE)
// this also marks you as incompatible with pre-1.6.629. see kVersionIndependentEx_NoStructUse if you have a
// plugin that only does code patches and works across many versions
#define VERSIONINDEPENDENT_STRUCTS_POST629 4

// **********************************
// flags for versionIndependenceEx

// set this if your plugin either doesn't use any game structures or has put in extraordinary effort
// to work with pre and post 1.6.629 structure layout
#define VERSIONINDEPENDENTEX_NO_STRUCT_USE 1


typedef struct
{
	uint32_t dataVersion;			// set to kVersion

	uint32_t pluginVersion;			// version number of your plugin
	char	name[256];				// null-terminated ASCII plugin name

	char	author[256];			// null-terminated ASCII plugin author name (can be empty)
	char	supportEmail[252];		// null-terminated ASCII support email address (can be empty)
									// this is not for showing to users, it's in case I need to contact you about
									// a compatibility problem with your plugin

	// version compatibility
	uint32_t versionIndependenceEx;	// set to one of the kVersionIndependentEx_ enums or zero
	uint32_t versionIndependence;	// set to one of the kVersionIndependent_ enums or zero
	uint32_t compatibleVersions[16];	// zero-terminated list of RUNTIME_VERSION_ defines your plugin is compatible with

	uint32_t seVersionRequired;		// minimum version of the script extender required, compared against PACKED_SKSE_VERSION
									// you probably should just set this to 0 unless you know what you are doing
} SKSEPluginVersionData;

typedef struct
{
	uint32_t infoVersion;
	const char* name;
	uint32_t version;

} PluginInfo;
