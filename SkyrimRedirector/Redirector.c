#include "SR_Base.h"
#include "Redirector.h"
#include "Redirections.h"
#include "Logging.h"

#include <stdbool.h>
#include <Windows.h>
#include "..\Detours\detours.h"


static bool Attached = false;

bool SR_AttachRedirector()
{
	if (Attached)
	{
		SR_WARN("Tried to attach redirections, but we are already attached. Ignoring.");
		return true;
	}
	
	Attached = true;

	SR_DEBUG("Attaching all redirections");
	
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	SR_Redirection* current = SR_GetRedirections();
	while (current != NULL)
	{
		DetourAttach(current->Original, current->Redirected);
		SR_TRACE("Attached %ls", current->Name);
		current = current->Next;
	}
	
	if (DetourTransactionCommit() != NO_ERROR) 
	{
		SR_TRACE("Unable to commit transaction");
		SR_ERROR("Unable to attach redirections, plugin failed to load");
		return false;
	}
	
	SR_TRACE("Transaction commited");
	SR_INFO("Redirections attached successfully, plugin loaded");

	return true;
}

bool SR_DetachRedirector()
{
	if (!Attached)
	{
		SR_WARN("Tried to detach redirections, but we are already detached. Ignoring.");
		return true;
	}

	Attached = false;

	SR_TRACE("Detaching all redirections");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	SR_Redirection* current = SR_GetRedirections();
	while (current != NULL)
	{
		DetourDetach(current->Original, current->Redirected);
		SR_TRACE("Detached %ls", current->Name);
		current = current->Next;
	}

	SR_FreeRedirections();

	if (DetourTransactionCommit() != NO_ERROR)
	{
		SR_ERROR("Unable to detach redirections, plugin failed to unload");
		return false;
	}

	SR_INFO("Redirections detached successfully, plugin unloaded");
	return true;	
}