#pragma once
#include <Windows.h>

typedef struct SR_Redirection
{
	const wchar_t* Name;
	PVOID* Original;
	PVOID Redirected;

	struct SR_Redirection* Next;

} SR_Redirection;

SR_Redirection* SR_GetRedirections();
void SR_FreeRedirections();
