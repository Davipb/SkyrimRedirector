#pragma once

#ifdef SR_SPECIAL_EDITION
	#ifdef SR_GOG
		#define SR_FOLDER_SUFFIX_A " SPECIAL EDITION GOG"
		#define SR_FOLDER_SUFFIX_W L" SPECIAL EDITION GOG"

		#define SR_PLATFORM_IDENTIFIER_A "Special Edition GOG"
		#define SR_PLATFORM_IDENTIFIER_W L"Special Edition GOG"
	#else
		#define SR_FOLDER_SUFFIX_A " SPECIAL EDITION"
		#define SR_FOLDER_SUFFIX_W L" SPECIAL EDITION"

		#define SR_PLATFORM_IDENTIFIER_A "Special Edition Steam"
		#define SR_PLATFORM_IDENTIFIER_W L"Special Edition Steam"
	#endif
#else
	#ifdef SR_GOG
		#error "Legendary Edition isn't supported by GOG'"
	#else
		#define SR_FOLDER_SUFFIX_A ""
		#define SR_FOLDER_SUFFIX_W L""

		#define SR_PLATFORM_IDENTIFIER_A "Legendary Edition Steam"
		#define SR_PLATFORM_IDENTIFIER_W L"Legendary Edition Steam"
	#endif
#endif
