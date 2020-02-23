#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class cConsole
{
private:
	static HANDLE   hOut, hIn;
	static WORD     defaultTextAttributes;
	static cConsole obj;
	static DWORD    temp;

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
	inline operator bool()  { return handle != INVALID_HANDLE_VALUE; }
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
#include <stdint.h>
#include <stdio.h>

#define COLOR_DEFAULT "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;36m"

class cConsole
{
public:
	static inline void Flush() { while (getchar() != '\n'); }
	static inline void WriteChar(const char c) { putchar(c); }
	static inline void Write(const char* str) { fputs(str, stdout); }
	static inline void WriteLine(const char* str) { puts(str); }
	static inline char ReadChar() { return getchar(); }
	template<int size>
	static inline void Read(char(&str)[size]) { fgets(str, size, stdin); }
};

typedef struct
{
private:
	FILE* file;
	friend class cFile;
	
public:
	inline operator bool()  { return file; }
	inline bool operator!() { return !file; }
} file;

class cFile
{
private:
	typedef int_fast32_t mode;
	
public:
	static constexpr mode OpenRead  = 0x6272; /* "rb" */
	static constexpr mode OpenWrite = 0x6277; /* "wb" */
	
	static inline file Open(const char* filename, mode openmode)
	{
		file result;
		result.file = fopen(filename, reinterpret_cast<const char*>(&openmode));
		return result;
	}
	static inline void Close(file f) { fclose(f.file); }
	static inline size_t Read(file f, void* ptr, size_t size) { return fread(ptr, 1, size, f.file); }
	static inline size_t Write(file f, void* ptr, size_t size) { return fwrite(ptr, 1, size, f.file); }
	static inline void Delete(const char* filename) { remove(filename); }
};
#endif
