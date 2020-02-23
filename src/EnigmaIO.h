#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

typedef struct
{
private:
	friend class cFile;
	HANDLE handle;

public:
	inline operator bool() { return handle != INVALID_HANDLE_VALUE; }
	inline bool operator!() { return handle == INVALID_HANDLE_VALUE; }
} file;

class cFile
{
private:
	typedef DWORD mode;

public:
	static constexpr mode OpenRead  = GENERIC_READ;
	static constexpr mode OpenWrite = GENERIC_WRITE;

	static inline file Open(const char* filename, mode openmode)
	{
		file result;
		result.handle = CreateFileA(
			filename,
			openmode,
			NULL,
			NULL,
			openmode == OpenRead ? OPEN_EXISTING : CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
		);
		return result;
	}
	static inline void Close(file f) { CloseHandle(f.handle); }
	static inline DWORD Read(file f, void* ptr, DWORD size)
	{
		DWORD bytesRead;
#pragma warning(suppress: 6031)
		ReadFile(f.handle, ptr, size, &bytesRead, NULL);
		return bytesRead;
	}
	static inline DWORD Write(file f, void* ptr, DWORD size)
	{
		DWORD bytesWritten;
		WriteFile(f.handle, ptr, size, &bytesWritten, NULL);
		return bytesWritten;
	}
	static inline void Delete(const char* filename) { DeleteFileA(filename); }
};

#else
#include <stdio.h>
typedef FILE* file;
#endif