#pragma once

#ifdef _WIN32
/* Disable Windows unnecessary definitions */
#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS     // - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // - VK_*
#define NOWINMESSAGES     // - WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // - SM_*
#define NOMENUS           // - MF_*
#define NOICONS           // - IDI_*
#define NOKEYSTATES       // - MK_*
#define NOSYSCOMMANDS     // - SC_*
#define NORASTEROPS       // - Binary and Tertiary raster ops
#define NOSHOWWINDOW      // - SW_*
#define OEMRESOURCE       // - OEM Resource values
#define NOATOM            // - Atom Manager routines
#define NOCLIPBOARD       // - Clipboard routines
#define NOCOLOR           // - Screen colors
#define NOCTLMGR          // - Control and Dialog routines
#define NODRAWTEXT        // - DrawText() and DT_*
#define NOGDI             // - All GDI defines and routines
#define NOKERNEL          // - All KERNEL defines and routines
#define NOUSER            // - All USER defines and routines
#define NONLS             // - All NLS defines and routines
#define NOMB              // - MB_* and MessageBox()
#define NOMEMMGR          // - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // - typedef METAFILEPICT
#define NOMINMAX          // - Macros min(a,b) and max(a,b)
#define NOMSG             // - typedef MSG and associated routines
#define NOOPENFILE        // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // - SB_* and scrolling routines
#define NOSERVICE         // - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // - Sound driver routines
#define NOTEXTMETRIC      // - typedef TEXTMETRIC and associated routines
#define NOWH              // - SetWindowsHook and WH_*
#define NOWINOFFSETS      // - GWL_*, GCL_*, associated routines
#define NOCOMM            // - COMM driver routines
#define NOKANJI           // - Kanji support stuff.
#define NOHELP            // - Help engine interface.
#define NOPROFILER        // - Profiler interface.
#define NODEFERWINDOWPOS  // - DeferWindowPos routines
#define NOMCX             // - Modem Configuration Extensions

#include <Windows.h>

#define COLOR_RED    (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLOR_GREEN  (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define COLOR_BLUE   (FOREGROUND_BLUE | FOREGROUND_INTENSITY)

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
	static inline void ChangeColor(WORD color) { SetConsoleTextAttribute(hOut, color); }
	static inline void ResetColor() { SetConsoleTextAttribute(hOut, defaultTextAttributes); }
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
