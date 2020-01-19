#pragma once
#include <shtypes.h>

// Gets a known folder for the current user.
// The returned string is allocated dynamically and must be freed.
wchar_t* SR_GetKnownFolder(const KNOWNFOLDERID* const rfid);

// Canonicizes a wide path, transforming it into an absolute path with no '.' or '..' nodes and in all uppercase
// The returned string is allocated dynamically and must be freed.
wchar_t* SR_CanonicizePathW(const wchar_t* path);

// Canonicizes a narrow path, transforming it into an absolute path with no '.' or '..' nodes and in all uppercase
// The returned string is allocated dynamically and must be freed.
char* SR_CanonicizePathA(const char* path);
