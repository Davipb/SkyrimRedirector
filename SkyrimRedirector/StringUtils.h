#pragma once
#include <wchar.h>
#include <stdbool.h>

// Gets a locale that can be used to perform invariant string operations.
_locale_t SR_GetInvariantLocale();

// Transforms a UTF16 string to a UTF8 string.
// The new string is allocated dynamically must be freed
char* SR_Utf16ToUtf8(const wchar_t* utf16);

// Transforms a UTF16 string to a Windows Codepage string.
// The new string is allocated dynamically must be freed
char* SR_Utf16ToCodepage(const wchar_t* utf16);

// Gets the file name from a wide file path.
// The returned string points to the same buffer as `path`, and doesn't need to be freed.
const wchar_t* SR_GetFileNameW(const wchar_t* path);

// Gets the file name from a narrow file path.
// The returned string points to the same buffer as `path`, and doesn't need to be freed.
const char* SR_GetFileNameA(const char* path);

// Transforms a wide string to all-upercase, using an invariant locale.
// The returned string is allocated dynamically and must be freed
const wchar_t* SR_ToUpperW(const wchar_t* input);

// Transforms a narrow string to all-upercase, using an invariant locale.
// The returned string is allocated dynamically and must be freed
const char* SR_ToUpperA(const char* input);

// Checks if two wide strings are equal, ignoring case and using an invariant locale.
bool SR_AreCaseInsensitiveEqualW(const wchar_t* first, const wchar_t* second);

// Checks if two narrow strings are equal, ignoring case and using an invariant locale.
bool SR_AreCaseInsensitiveEqualA(const char* first, const char* second);

// Concatenates multiple strings. All arguments must be of type const wchar_t*.
// The returned string is allocated dynamically and must be freed.
wchar_t* SR_Concat(size_t count, ...);

// Checks if a wide string ends with another wide string.
// full: The full string
// component: The string that should be at the end of the full string
bool SR_EndsWithW(const wchar_t* full, const wchar_t* component);

// Checks if a narrow string ends with another narrow string.
// full: The full string
// component: The string that should be at the end of the full string
bool SR_EndsWithA(const char* full, const char* component);
