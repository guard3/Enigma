#pragma once
#ifdef _WIN32
/* Disable unnecessary Windows definitions */
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
	COLOR_BLUE    = FOREGROUND_BLUE      | FOREGROUND_GREEN,
	COLOR_RED     = FOREGROUND_INTENSITY | FOREGROUND_RED,
	COLOR_GREEN   = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	COLOR_YELLOW  = FOREGROUND_RED       | FOREGROUND_GREEN
};

#else
#include <unistd.h>
#include <fcntl.h>
#endif
#include <utility>

/* Enum for colours to be used in cConsole methods */
enum eConsoleColor {
	COLOR_DEFAULT,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE
};

class cConsole final {
private:
	/* cConsole instance */
	static cConsole ms_instance;
	/* Array of functions that change console color */
	static void(*ms_color_funcs[5])();
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
	/* Private constructor to prevent cConsole object creation */
	cConsole() = default;
	
	/* Intermediary functions */
	static ssize_t read(char* p, size_t n) { return ::read(STDIN_FILENO, p, n); }
	static void write(const char* p, size_t n) { ::write(STDOUT_FILENO, p, n); }
#endif

public:
	/*----------------------------------------------*
	 | FUNCTION: cConsole::Read()                   |
	 |     - Read a character or string from stdin. |
	 |     - Ignore any CR characters at the end.   |
	 *----------------------------------------------*/
	static void Read(char& c) {
#ifdef _WIN32
		do { read(&c, 1); } while (c == '\r');
#else
		read(&c, 1);
#endif
	}
	template<size_t size>
	static void Read(char(&str)[size]) {
		auto num = read(str, size - 1);
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
	static void Read(char(&str)[1]) { str[0] = '\0'; }
	
	/*----------------------------------------------*
	 | FUNCTION: cConsole::Write()                  |
	 |     - Print a character or string to stdout. |
	 |     - Change console color.                  |
	 *----------------------------------------------*/
	static void Write(char c) { write(&c, 1); }
	template<size_t size>
	static void Write(const char(&str)[size]) { if constexpr(size > 1) write(str, size - 1); }
	static void Write(eConsoleColor color) { ms_color_funcs[color](); }
	template<typename Arg, typename... Args>
	static void Write(Arg&& arg, Args&&... args) {
		Write(std::forward<Arg>(arg));
		Write(std::forward<Args>(args)...);
	}

	/*-----------------------------*
	 | FUNCTION: cConsole::Flush() |
	 |     * Clear stdin.          |
	 *-----------------------------*/
	static void Flush()
	{
#if 1
		// MAYBE BETTER??
		char temp[32];
		size_t size;
		do { size = read(temp, 32); } while (size == 32);
#else
		char c;
		do { read(&c, 1); } while (c != '\n');
#endif
	}

	~cConsole() { Write(COLOR_DEFAULT); }
};

class cFile
{
#ifdef _WIN32
private:
	/* CreateFileA wrapper */
	static HANDLE _my_simple_open(const char* filename, DWORD dwDesiredAccess, DWORD dwCreationDisposition)
	{
		return CreateFileA(
			filename,
			dwDesiredAccess,
			NULL,
			NULL,
			dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
		);
	}

protected:
	/* Platform dependent typedefs */
	typedef HANDLE file_t;
	typedef DWORD bufsize_t;
	static inline constexpr file_t F_ERROR = INVALID_HANDLE_VALUE;

	/* Intermediary functions */
	static file_t _my_open_r(const char* filename) { return _my_simple_open(filename, GENERIC_READ,  OPEN_EXISTING); }
	static file_t _my_open_w(const char* filename) { return _my_simple_open(filename, GENERIC_WRITE, CREATE_ALWAYS); }
	static bool _my_read(file_t f, void* b, bufsize_t s)
	{
		DWORD bytesRead;
		if (!ReadFile(f, b, s, &bytesRead, NULL))
			return false;
		return bytesRead == s;
	}
	static bool _my_write(file_t f, const void* b, bufsize_t s)
	{
		DWORD bytesWritten;
		if (!WriteFile(f, b, s, &bytesWritten, NULL))
			return false;
		return bytesWritten == s;
	}
	static void _my_close(file_t f) { CloseHandle(f); }
	static void _my_delete(const char* filename) { DeleteFileA(filename); }
#else
protected:
	/* Platform dependent typedefs */
	typedef int file_t;
	typedef size_t bufsize_t;
	static inline constexpr file_t F_ERROR = -1;
	
	/* Intermediary functions */
	static file_t _my_open_r(const char* filename) { return open (filename, O_RDONLY); }
	static file_t _my_open_w(const char* filename) { return creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); }
	static bool   _my_read  (file_t f,       void* b, bufsize_t s) { return read (f, b, s) == s; }
	static bool   _my_write (file_t f, const void* b, bufsize_t s) { return write(f, b, s) == s; }
	static void   _my_close (file_t f) { close(f); }
	static void   _my_delete(const char* filename) { unlink(filename); }
#endif
	
	/* The file object's file descriptor or handle */
	file_t m_fd;
	
	/* Private constructors to disable cFile object creation */
	cFile() { m_fd = F_ERROR; }
	cFile(const cFile&) = delete;
	cFile(cFile&&)      = delete;
	cFile& operator=(const cFile&) = delete;
	cFile& operator=(cFile&&)      = delete;
	
public:
	~cFile() { if (m_fd != F_ERROR) _my_close(m_fd); }

	/* Boolean operators to check if file is successfully opened */
	operator bool()  { return m_fd != F_ERROR; }
	bool operator!() { return m_fd == F_ERROR; }

	static void Delete(const char* filename) { _my_delete(filename); }
};

class cIFile : public cFile
{
public:
	cIFile(const char* filename) { m_fd = _my_open_r(filename); }
	bool Read(void* dest, bufsize_t size) { return _my_read(m_fd, dest, size); }
};

class cOFile : public cFile
{
public:
	cOFile(const char* filename) { m_fd = _my_open_w(filename); }
	bool Write(const void* src, bufsize_t size) { return _my_write(m_fd, src, size); }
};