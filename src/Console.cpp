#include "Console.h"
#include <iostream>

#ifdef _WIN32
cConsole cConsole::ms_instance;

/* The standard utf16 console handles */
static HANDLE hIn, hOut;

/* Handles to utf8 files, in case they're specified */
static HANDLE hInFile = INVALID_HANDLE_VALUE, hOutFile = INVALID_HANDLE_VALUE;

static void put_console(tUnicodeChar c) {
	std::cout << "put_console" << std::endl;
}

static void put_file(tUnicodeChar c) {
	std::cout << "put_file" << std::endl;
}

static tUnicodeChar get_console() {
	std::cout << "get_console" << std::endl;
	return 0;
}

static tUnicodeChar get_file() {
	std::cout << "get_file" << std::endl;
	return 0;
}

cConsole::cConsole() {
	/* Initialize functions */
	ms_write = WriteConsoleA;
	ms_put   = put_console;
	ms_get   = get_console;
	/* Initialize console handles */
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn && hIn != INVALID_HANDLE_VALUE) {
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut && hOut != INVALID_HANDLE_VALUE) {
			/* Save default console attributes */
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
				ms_attr = csbi.wAttributes;
				return;
			}
		}
	}
	exit(EXIT_FAILURE);
}

cConsole::~cConsole() {
	if (hInFile != INVALID_HANDLE_VALUE)
		CloseHandle(hInFile);
	if (hOutFile != INVALID_HANDLE_VALUE)
		CloseHandle(hOutFile);
}
#else
cConsole::~cConsole() {
	if (ms_stdin_fd != STDIN_FILENO)
		close(ms_stdin_fd);
	if (ms_stdout_fd != STDOUT_FILENO)
		close(ms_stdout_fd);
}
#endif

#if 0
int cEnigma2::Initialize(const char* input_file) {
#ifdef _WIN32
#else
	int fd = open(input_file, O_RDONLY);
	if (fd == -1)
		return 1;
	ms_stdin_fd = fd;
#endif
	return Initialize();
}

int cEnigma2::Initialize(const char* input_file, const char* output_file) {
#ifdef _WIN32
#else
	int fd = open(output_file, O_WRONLY | O_TRUNC);
	if (fd == -1) {
		fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd == -1)
			return 2;
	}
	ms_stdout_fd = fd;
#endif
	return Initialize(input_file);
}
#endif