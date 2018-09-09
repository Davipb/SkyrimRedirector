#include "PluginAPI.h"
#include "Logging.h"
#include "Redirector.h"
#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>

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

	SR_DEBUG("SKSE load request received");
	return true;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	(void)hinst;
	(void)reserved;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		SR_StartLogging();
		return SR_AttachRedirector();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		bool result = SR_DetachRedirector();
		SR_StopLogging();

		return result;
	}

	return TRUE;
}