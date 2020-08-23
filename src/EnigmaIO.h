#pragma once
//#undef _WIN32
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
#include <stdlib.h>
#include <iostream>

/* Enum for colours to be used in cConsole methods */
enum eConsoleColor : WORD
{
	COLOR_DEFAULT = 0,
	COLOR_BLUE    = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	COLOR_RED     = FOREGROUND_INTENSITY | FOREGROUND_RED,
	COLOR_GREEN   = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	COLOR_YELLOW  = FOREGROUND_RED       | FOREGROUND_GREEN
};

#else
#include <stdint.h>
#include <stdio.h>

/* Enum for colours to be used in cConsole methods */
enum eConsoleColor : char
{
	COLOR_DEFAULT = 0,
	COLOR_RED     = '1', // "\033[1;31m"
	COLOR_GREEN   = '2', // "\033[1;32m"
	COLOR_YELLOW  = '3', // "\033[1;33m"
	COLOR_BLUE    = '6'  // "\033[1;36m"
};
#endif

class cConsole final
{
private:
#ifdef _WIN32
	/* Static console variables */
	static cConsole instance;        // Static instance of cConsole; used only to invoke the private constructor
	inline static HANDLE hIn, hOut;  // stdin and stdout handles
	inline static WORD   attributes; // Default console text attributes; used for resetting console colors
	inline static DWORD  numRead;    // A DWORD for ReadConsoleA() to store the number of chars read from stdin

	/* Instance constructor; initializes WINAPI console handles and attributes */
	cConsole()
	{
		/* Get std handles and save default console text attributes */
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut && hOut != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (GetConsoleScreenBufferInfo(hOut, &csbi))
			{
				hIn = GetStdHandle(STD_INPUT_HANDLE);
				if (hIn && hIn != INVALID_HANDLE_VALUE)
				{
					attributes = csbi.wAttributes;
					return;
				}
			}
			CloseHandle(hOut);
		}
		exit(EXIT_FAILURE);
	}

#else
	typedef uint32_t DWORD;

	cConsole() {}
#endif

public:


	/* FUNCTION: cConsole::Write()                     *
	 *           Print a character or string to stdout */
#ifdef _WIN32
	static void Write(const char c) { WriteConsoleA(hOut, &c, 1, NULL, NULL); }
	template<DWORD size>
	static void Write(const char(&str)[size]) { WriteConsoleA(hOut, str, size - 1, NULL, NULL); }
#else
	static void Write(const char c) { putchar(c); }
	template<DWORD size>
	static void Write(const char(&str)[size]) { fputs(str, stdout); }
#endif
	template<>
	static void Write(const char(&str)[1]) {}

	/* FUNCTION: cConsole::NewLine()                  *
	 *           Print a new line character to stdout */
	static void NewLine() { Write('\n'); }

	/* FUNCTION: cConsole::WriteLine()                           *
	 *           Print a character or string to stdout as a line */
	static void WriteLine(const char c) { Write(c); NewLine(); }
#ifdef _WIN32
	template<DWORD size>
	static void WriteLine(const char(&str)[size]) { Write(str); NewLine(); }
#else
	template<DWORD size>
	static void WriteLine(const char(&str)[size]) { puts(str); }
	template<>
	static void WriteLine(const char(&str)[1]) { NewLine(); }
#endif

	/* FUNCTION: cConsole::ChangeColor()   *
	 *           Change console text color */
#ifdef _WIN32
	template<eConsoleColor color>
	static void SetColor() { SetConsoleTextAttribute(hOut, color); }
	template<>
	static void SetColor<COLOR_DEFAULT>() { SetConsoleTextAttribute(hOut, attributes); }
#else
	template<eConsoleColor color>
	static void SetColor()
	{
		static char colstr[] = "\033[1;3\0m";
		colstr[5] = color;
		Write(colstr);
	}
	template<>
	static void SetColor<COLOR_DEFAULT>() { Write("\033[0m"); }
#endif
#define SetColor(color) SetColor<color>()

	/* FUNCTION: cConsole::Read()                      *
	 *           Read a character or string from stdin */
	static char Read()
	{
#ifdef _WIN32
		char c;
		ReadConsoleA(hIn, &c, 1, &numRead, NULL);
		return c;
#else
		return getchar();
#endif
	}
	template<DWORD size>
	static void Read(char(&str)[size])
	{
#ifdef _WIN32
		if (!ReadConsoleA(hIn, str, size - 1, &numRead, NULL))
			return;
		if (str[numRead - 1] == '\r')
			str[numRead - 1] = Read();
		str[numRead] = '\0';
#else
		fgets(str, size, stdin);
#endif
	}
	template<>
	static void Read(char(&str)[1]) { str[0] = '\0'; }

	/* FUNCTION: cConsole::ReadSkipCarriageReturn()              *
	 *           Read a character from stdin, skipping any '\r's */
	static char ReadSkipCarriageReturn()
	{
		char c = Read();
#ifdef _WIN32
		if (c == '\r')
			return Read();
#endif
		return c;
	}

	/* FUNCTION: cConsole::Flush() *
	 *           Clear stdin       */
	static void Flush() { while (Read() != '\n'); }

	/* Color variants of functions */
	template<eConsoleColor color>
	static void Write(const char c) { SetColor(color); Write(c); }
	template<eConsoleColor color, DWORD size>
	static void Write(const char(&str)[size]) { SetColor(color); Write(str); }
	template<eConsoleColor color>
	static void NewLine() { SetColor(color); NewLine(); }
	template<eConsoleColor color>
	static void WriteLine(const char c) { SetColor(color); WriteLine(c); }
	template<eConsoleColor color, DWORD size>
	static void WriteLine(const char(&str)[size]) { SetColor(color); WriteLine(str); }

#ifdef _WIN32
	/* Close console handles */
	~cConsole()
	{
		SetColor(COLOR_DEFAULT);
		CloseHandle(hIn);
		CloseHandle(hOut);
	}
};

inline cConsole cConsole::instance;
#else
};
#endif

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

#ifdef NULL
#else
#include <stdint.h>
#include <stdio.h>

//#define COLOR_DEFAULT "\033[0m"
//#define COLOR_RED     "\033[1;31m"
//#define COLOR_GREEN   "\033[1;32m"
//#define COLOR_YELLOW  "\033[1;33m"
//#define COLOR_BLUE    "\033[1;36m"

enum eConsoleColor : char
{
	COLOR_DEFAULT = 0,
	COLOR_RED = '1', // "\033[1;31m"
	COLOR_GREEN = '2', // "\033[1;32m"
	COLOR_YELLOW = '3', // "\033[1;33m"
	COLOR_BLUE = '6'  // "\033[1;36m"
};

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
