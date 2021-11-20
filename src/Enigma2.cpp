#include "Enigma2.h"
#include <cstdlib>
#include <cstring>
#ifndef _WIN32
#include <fcntl.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
#include <mach-o/dyld.h>
#endif
#include <iostream>

/* Defining an enigma instance to trigger the destructor at exit */
cEnigma2 cEnigma2::ms_instance;

static uint8_t *g_buffer = nullptr, *g_plugboard = nullptr, *g_rotor1_in = nullptr, *g_rotor2_in = nullptr, *g_reflector = nullptr, *g_rotor2_out = nullptr, *g_rotor1_out = nullptr;

#ifdef _WIN32
static LPWSTR g_settings_path = nullptr;
#else
static char* g_settings_path = nullptr;
#endif

cEnigma2::~cEnigma2() {
	free(g_buffer);
	free(g_settings_path);
#ifdef _WIN32
	...
#else
	if (ms_stdout_fd != STDOUT_FILENO)
		close(ms_stdout_fd);
	if (ms_stdin_fd  != STDIN_FILENO)
		close(ms_stdin_fd);
#endif
}

int cEnigma2::Initialize() {
	/* Get a suitable path for the settings file */
#ifdef _WIN32
	...
#else
#if defined(__APPLE__) && defined(__MACH__)
	/* Prepare the initial buffer */
	uint32_t size = 247;
	char *path = (char *) malloc(256);
	if (!path)
		return -1;
	/* Retrieve a path to this executable */
	char *p;
	if (_NSGetExecutablePath(path, &size) < 0) {
		p = (char *) realloc(path, size + 9);
		if (!p) {
			free(path);
			return -1;
		}
		path = p;
		_NSGetExecutablePath(path, &size);
	}
	/* Replace executable name with settings file name */
	p = strrchr(path, '/');
	strcpy(p ? p + 1 : path, "enigma.set");
	/* Resolve any potential symbolic links and canonicalize path */
	p = realpath(path, nullptr);
	free(path);
	if (!p)
		return -1;
	/* Save path to global variable */
	g_settings_path = p;
#else
	/* TODO: readlink on /proc/self/exe */
	char* p = (char*)malloc(16);
	if (!p)
		return -1;
	strcpy(p, "/etc/enigma.set");
	/* Save path to global variable */
	g_settings_path = p;
#endif

	int fd = open(p, O_RDONLY);
	if (fd == -1) {
		/* If settings file doesn't exist, create it */
		std::cout << "SETTINGS FILE DOESN'T EXIST" << std::endl;
	}
	else {
		std::cout << "SETTINGS FILE EXIST" << std::endl;
		close(fd);
	}


#endif
	return 0;
}

int cEnigma2::Initialize(const char *input_file) {
#ifdef _WIN32
#else
	int fd = open(input_file, O_RDONLY);
	if (fd == -1)
		return 1;
	ms_stdin_fd = fd;
#endif
	return Initialize();
}

int cEnigma2::Initialize(const char *input_file, const char *output_file) {
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