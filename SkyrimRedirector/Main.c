#include "SR_Base.h"
#include "PluginAPI.h"
#include "Logging.h"
#include "Redirector.h"
#include "Config.h"
#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>

__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version =
{
	PLUGIN_INFO_VERSION,

	1,
	"Skyrim Redirector",

	"Davipb and contributors",
	"https://github.com/Davipb/SkyrimRedirector",

	// setting ourselves as "not using structures" hopefully exempts us from any version checking
	VERSIONINDEPENDENTEX_NO_STRUCT_USE,
	VERSIONINDEPENDENT_SIGNATURES,
	{ 0 },
	0,
};

// API for compatibility with older versions of SKSE64
__declspec(dllexport) bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
{
	SR_DEBUG("SKSE query received");

	info->infoVersion = PLUGIN_INFO_VERSION;
	info->name = "Skyrim Redirector";
	info->version = 1;

	return !skse->isEditor;
}

__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
{
	(void)skse;

	SR_StartLogging();
	SR_DEBUG("SKSE load request received");

	SR_ValidateUserConfig();
	return SR_AttachRedirector();
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	(void)hinst;
	(void)reserved;

	if (dwReason == DLL_PROCESS_DETACH)
	{
		bool result = SR_DetachRedirector();
		SR_StopLogging();
		SR_FreeUserConfig();

		return result;
	}

	return TRUE;
}
