# Skyrim Redirector
This is a SKSE (*not SKSE64!*) plugin that redirects every file I/O in `Skyrim.ini`, `SkyrimPrefs.ini`, and `plugins.txt` to a new path.
It does so  by hooking into the Windows API calls made by the game and changing their target file path if necessary.

This plugin was made for the [Enderal total conversion of Skyrim](http://sureai.net/games/enderal/) and was inspired by [Mod Organizer's 'Hook DLL'](https://github.com/ModOrganizer2/modorganizer-hookdll), which does the same and more as this plugin, but is tied to the Mod Organizer program, and so wasn't suited for including in Enderal. 

This project is under the MIT License, as is its one dependency, [Microsoft's Detours library](https://github.com/Microsoft/Detours).
