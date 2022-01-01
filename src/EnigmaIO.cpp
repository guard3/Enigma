#include "EnigmaIO.h"

cConsole cConsole::ms_instance;

void(*cConsole::ms_color_funcs[5])() {
#ifdef _WIN32
#else
	[]() { Write("\033[0m");    },
	[]() { Write("\033[1;31m"); },
	[]() { Write("\033[1;32m"); },
	[]() { Write("\033[1;33m"); },
	[]() { Write("\033[1;36m"); }
#endif
};