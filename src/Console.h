#pragma once
#ifndef ENIGMA_CONSOLE_H
#define ENIGMA_CONSOLE_H
#ifdef _WIN32
#include "Win32.h"
#else
#endif
#include <cstdint>

typedef uint32_t tUnicodeChar;

class cConsole final {
private:
#ifdef _WIN32
	/* Default console text attributes; used for resetting console colors */
	static inline WORD ms_attr;
	/* Functions for writing to utf16 console or utf8 files */
	static inline BOOL(__stdcall* ms_write)(HANDLE, LPCVOID, DWORD, LPDWORD, LPVOID);
	static inline void(*ms_put)(tUnicodeChar);
	static inline tUnicodeChar(*ms_get)();
	/* Static instance to invoke private constructor */
	cConsole();
	static cConsole ms_instance;
#else
	typedef char* string_t;
	typedef const char* cstring_t;

	static inline int ms_stdin_fd = STDIN_FILENO;
	static inline int ms_stdout_fd = STDOUT_FILENO;

	cConsole() = default;
#endif


public:
	~cConsole();
};

#endif //ENIGMA_CONSOLE_H