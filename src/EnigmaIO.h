#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HANDLE file;

class cConsole
{
private:
	static HANDLE hOut, hIn;
	static WORD defaultTextAttributes;
	static cConsole obj;
	static DWORD temp;

public:
	~cConsole();

	static bool Initialize();

	static inline void Flush() { while (ReadChar() != '\n'); }

	static inline void WriteChar(const char c) { WriteConsoleA(hOut, &c, 1, NULL, NULL); }
	template<DWORD size>
	static inline void Write(const char(&str)[size]) { WriteConsoleA(hOut, str, size - 1, NULL, NULL); }
	template<DWORD size>
	static inline void WriteLine(const char(&str)[size]) { Write(str); WriteChar('\n'); }

	static inline char ReadChar()
	{
		char c;
		ReadConsoleA(hIn, &c, 1, &temp, NULL);
		return c;
	}
	template<DWORD size>
	static inline void Read(char(&str)[size]) { ReadConsoleA(hIn, str, size, &temp, NULL); }
};
#else
#include <stdio.h>
typedef FILE* file;
#endif



class cFile
{
private:
public:

};