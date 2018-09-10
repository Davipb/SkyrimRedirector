# Contributing to Skyrim Redirector

## Key Points
If you read nothing else, just read this:
* This is a C project, do not use C++ features.
* Every non-static symbol should begin with `SR_` (**S**kyrim **R**edirector) to avoid conflicts
* Skyrim is a Windows-only game, and so is this plugin. If the Windows C Runtime has a function you need, use it, don't reinvent the wheel.
* Always write ANSI (`A`) and Wide (`W`) versions of your hooks, don't convert from one to the other. 
Remember, the hooks will run at *every call to the Windows API*, they should be as free from overhead as possible.
* Function pointers and string manipulation, the two major points of this project, can be very tricky and non-intuitive. Be sure to double-check your code for any missing `free`s or incorrect pointer levels (Passing a `PVOID` to a function that expects a `PVOID*` by mistake is particularly common).


## String Manipulation
* Strings should be `wchar_t` UTF16 whenever possible.
* Mind the `const`. In special, strings received by WinAPI hooks should *never* **ever** be altered in-place: Copy it to a local buffer before changing anything!
* Avoid allocations for intermediate processes. If possible, calculate the final size of the result ahead of time, allocate a buffer of that size, and do your operations directly there.

## Windows API
* Avoid fixed-size buffers. If the function tells you how many bytes you'll need, such as `WideCharToMultiByte`, use it to allocate a buffer of the right size. If not, then start small, check if the data was truncated, and increase until you can fit all the data.
* Always call the correct char size function directly (e.g. `CreateFileW` or `CreateFileA`), do not use the macros (e.g. `CreateFile`).
