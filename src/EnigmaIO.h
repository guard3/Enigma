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
#include <unistd.h>
#include <fcntl.h>

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
	/* Size of string buffer type */
	typedef DWORD strsize_t;

	/* Static console variables */
	static cConsole instance;       // Static instance of cConsole; used only to invoke the private constructor
	inline static HANDLE hIn, hOut; // stdin and stdout handles
	inline static WORD   attr;      // Default console text attributes; used for resetting console colors

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
					attr = csbi.wAttributes;
					return;
				}
			}
			CloseHandle(hOut);
		}
		exit(EXIT_FAILURE);
	}

	/* Intermediary functions */
	static DWORD _my_read(void* p, strsize_t n)
	{
		static DWORD result;
		ReadConsoleA(hIn, p, n, &result, NULL);
		return result;
	}
	static void _my_write(const void* p, strsize_t n) { WriteConsoleA(hOut, p, n, NULL, NULL); }
#else
	/* Size of string buffer type */
	typedef size_t strsize_t;

	/* Private constructor to prevent object creation */
	cConsole() {}
	
	/* Intermediary functions */
	static ssize_t _my_read(void* p, strsize_t n) { return read(STDIN_FILENO, p, n); }
	static void _my_write(const void* p, strsize_t n) { write(STDOUT_FILENO, p, n); }
#endif

public:
	/*-----------------------------------------------------*
	 | FUNCTION: cConsole::Read()                          |
	 |     * Read a character or string from stdin.        |
	 |                                                     |
	 | NOTES:                                              |
	 |     * Reading a character doesn't skip any platform |
	 |       dependent characters in the input stream,     |
	 |       like CR in windows.                           |
	 |     * Reading string is guaranteed to skip CR.      |
	 *-----------------------------------------------------*/
	static void Read(char& c) { _my_read(&c, 1); }
	template<strsize_t size>
	static void Read(char(&str)[size])
	{
		auto num = _my_read(str, size - 1);
#ifdef _WIN32
		if (str[num - 1] == '\n')
		{
			str[num - 2] = '\n';
			str[num - 1] = '\0';
			return;
		}
		if (str[num - 1] == '\r')
			Read(str[num - 1]);
#endif
		str[num] = '\0';
	}
	static void Read(char(&str)[2]) { ReadSkipCarriageReturn(str[0]); str[1] = '\0'; }
	static void Read(char(&str)[1]) { str[0] = '\0'; }

	/*-----------------------------------------------------*
	 | FUNCTION: cConsole::ReadSkipCarriageReturn()        |
	 |     * Read a character from stdin, skipping any CR. |
	 *-----------------------------------------------------*/
	static void ReadSkipCarriageReturn(char& c)
	{
		Read(c);
#ifdef _WIN32
		if (c == '\r')
			Read(c);
#endif
	}
	
	/*----------------------------------------------*
	 | FUNCTION: cConsole::Write()                  |
	 |     * Print a character or string to stdout. |
	 *----------------------------------------------*/
	static void Write(const char c) { _my_write(&c, 1); }
	template<strsize_t size>
	static void Write(const char(&str)[size]) { _my_write(str, size - 1); }
	static void Write(const char(&str)[1]) {}

	/*-------------------------------*
	 | FUNCTION: cConsole::NewLine() |
	 |     * Print a LF to stdout.   |
	 *-------------------------------*/
	static void NewLine() { Write('\n'); }

	/*---------------------------------------------*
	 | FUNCTION: cConsole::WriteLine()             |
	 |     * Print a character or string to stdout |
	 |       followed by a LF.                     |
	 *---------------------------------------------*/
	static void WriteLine(const char c) { Write(c); NewLine(); }
	template<strsize_t size>
	static void WriteLine(const char(&str)[size]) { Write(str); NewLine(); }

	/*-----------------------------------*
	 | FUNCTION: cConsole::ChangeColor() |
	 |     * Change console text color.  |
	 *-----------------------------------*/
	template<eConsoleColor color>
	static void SetColor()
	{
#ifdef _WIN32
		if constexpr(color == COLOR_DEFAULT)
			SetConsoleTextAttribute(hOut, attr);
		else
			SetConsoleTextAttribute(hOut, color);
#else
		if constexpr(color == COLOR_DEFAULT)
			Write("\033[0m");
		else
		{
			static char colstr[] = "\033[1;3\0m";
			colstr[5] = color;
			Write(colstr);
		}
#endif
	}
#define SetColor(color) SetColor<color>()

	/*-----------------------------*
	 | FUNCTION: cConsole::Flush() |
	 |     * Clear stdin.          |
	 *-----------------------------*/
	static void Flush()
	{
		char c;
		do { Read(c); } while (c != '\n');
	}

	/* Color variants of functions */
	template<eConsoleColor color>
	static void Write(const char c) { SetColor(color); Write(c); }
	template<eConsoleColor color, strsize_t size>
	static void Write(const char(&str)[size]) { SetColor(color); Write(str); }
	template<eConsoleColor color>
	static void NewLine() { SetColor(color); NewLine(); }
	template<eConsoleColor color>
	static void WriteLine(const char c) { SetColor(color); WriteLine(c); }
	template<eConsoleColor color, strsize_t size>
	static void WriteLine(const char(&str)[size]) { SetColor(color); WriteLine(str); }

#ifdef _WIN32
	/* Reset text color and close console handles */
	~cConsole()
	{
		SetColor(COLOR_DEFAULT);
		CloseHandle(hIn);
		CloseHandle(hOut);
	}
#endif
};

#ifdef _WIN32
inline cConsole cConsole::instance;
#endif

class cF
{
protected:
#ifdef _WIN32
	typedef HANDLE file_t;
	typedef DWORD bufsize_t;
	static inline constexpr file_t F_ERROR = INVALID_HANDLE_VALUE;
	
#else
	typedef int file_t;
	typedef size_t bufsize_t;
	static inline constexpr file_t F_ERROR = -1;
	
	static file_t _my_open_r(const char* filename) { return open (filename, O_RDONLY); }
	static file_t _my_open_w(const char* filename) { return creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); }
	static bool   _my_read  (file_t f,       void* b, bufsize_t s) { return read (f, b, s) == s; }
	static bool   _my_write (file_t f, const void* b, bufsize_t s) { return write(f, b, s) == s; }
	static void   _my_close (file_t f) { close(f); }
#endif
	
	file_t m_fd;
	
	cF() {}
	cF(const cF&) = delete;
	cF(cF&&) = delete;
	cF& operator=(const cF&) = delete;
	cF& operator=(cF&&) = delete;
	
public:
	/* Boolean operators to check if file is successfully opened */
	operator bool()  { return m_fd != F_ERROR; }
	bool operator!() { return m_fd == F_ERROR; }
	
	void Close()
	{
		if (m_fd != F_ERROR)
			_my_close(m_fd);
		m_fd = F_ERROR;
	}
	
	~cF() { Close(); }
};

class cIFile : public cF
{
public:
	cIFile(const char* filename) { m_fd = _my_open_r(filename); }
	bool Read(void* dest, bufsize_t size) { return _my_read(m_fd, dest, size); }
};

class cOFile : public cF
{
public:
	cOFile(const char* filename) { m_fd = _my_open_w(filename); }
	bool Write(const void* src, bufsize_t size) { return _my_write(m_fd, src, size); }
};

#ifdef _WIN32
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
