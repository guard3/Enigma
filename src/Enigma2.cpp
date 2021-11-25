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
#include <random>

static uint8_t *g_plugboard = nullptr, *g_rotor1_in = nullptr, *g_rotor2_in = nullptr, *g_reflector = nullptr, *g_rotor2_out = nullptr, *g_rotor1_out = nullptr, *g_backup;

/* The path to settings file */
#ifdef _WIN32
static LPWSTR g_settings_path = nullptr;
#else
static char  *g_settings_path = nullptr;
#endif

/* Defining an enigma instance to trigger the destructor at exit */
cEnigma2 cEnigma2::ms_instance;

cEnigma2::~cEnigma2() {
	free(g_plugboard);
	free(g_settings_path);
}

static bool generate_settings() {
	uint8_t i, j, n, a, b;
	/* Initialize the random engine */
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist;
#define RANDRANGE(min, max) (dist(rng, std::uniform_int_distribution<int>::param_type((min), (max))))
	/* A lambda used for individual shuffling steps */
	auto shuffle_step = [&i, &j, &rng, &dist](uint8_t* buf, uint8_t min, uint8_t max) -> uint8_t {
		uint8_t result;
		j = RANDRANGE(min, max);
		result = buf[j];
		buf[j] = buf[i];
		buf[i] = result;
		return result;
	};
	/* Choose how many plugboard pairs to make */
	n = RANDRANGE(0, 82) * 2;
	/* Temporarily use backup buffer and fill it with 0...163 */
	for (i = 0; i < 164; ++i)
		g_backup[i] = i;
	/* Shuffle n elements and form plugboard combos, also copy to rotor1 */
	for (i = 0; i < n;) {
		a = shuffle_step(g_backup, i, 163);
		g_rotor1_in[i++] = a;
		b = shuffle_step(g_backup, i, 163);
		g_rotor1_in[i++] = b;
		g_plugboard[a] = b;
		g_plugboard[b] = a;
	}
	for (i = n; i < 164; ++i) {
		a = g_backup[i];
		g_plugboard[a] = a;
		g_rotor1_in[i] = a;
	}
	/* Shuffle rotor1, copy to a temporary buffer and make reflector pairs */
	uint8_t* temp = g_backup + 164;
	for (i = 163; i > 1;) {
		a = shuffle_step(g_rotor1_in, 0, i);
		temp[i--] = a;
		b = shuffle_step(g_rotor1_in, 0, i);
		temp[i--] = b;
		g_reflector[a] = b;
		g_reflector[b] = a;
	}
	a = shuffle_step(g_rotor1_in, 0, 1);
	temp[1] = a;
	b = g_rotor1_in[0];
	temp[0] = b;
	g_reflector[a] = b;
	g_reflector[b] = a;
	/* Shuffle rotor1 and copy to rotor2 */
	for (i = 163; i > 0; --i)
		g_rotor2_in[i] = shuffle_step(g_rotor1_in, 0, i);
	g_rotor2_in[0] = g_rotor1_in[0];
	/* Shuffle rotor1 yet again */
	for (i = 163; i > 0; --i)
		shuffle_step(g_rotor1_in, 0, i);
	/* Set each rotor's backtracking settings */
	for (i = 0; i < 164; ++i) {
		g_rotor1_out[g_rotor1_in[i]] = i;
		g_rotor2_out[g_rotor2_in[i]] = i;
	}

	return 1;
}

int cEnigma2::Initialize(const char* settings_b64) {
#ifdef _WIN32
	/* Get a suitable path for the settings file */
	if (LPWSTR lpPath = (LPWSTR)malloc(16 * sizeof(WCHAR))) {
		for (DWORD dwSize = 7, dwError;;) {
			/* Retrieve the fully qualified path to this executable */
			if (0 == (dwSize = GetModuleFileNameW(NULL, lpPath, dwSize)))
				goto LABEL_ERROR;
			/* Check if path was fully copied into the buffer */
			dwError = GetLastError();
			if (dwError != ERROR_INSUFFICIENT_BUFFER) {
				/* Replace the executable name with settings file name */
				if (LPWSTR lpFilePart = wcsrchr(lpPath, L'\\')) {
					wcscpy(lpFilePart + 1, L"enigma.set");
					/* Save path to global variable */
					g_settings_path = lpPath;
					goto LABEL_OK;
				}
				break;
			}
			/* If buffer isn't large enough, allocate more memory */
			void* temp = realloc(lpPath, ((size_t)(dwSize <<= 1) + 9) * sizeof(WCHAR));
			if (!temp)
				goto LABEL_ERROR;
			lpPath = (LPWSTR)temp;
		}
	LABEL_ERROR:
		free(lpPath);
	}
	return -1;

LABEL_OK:
	
#elif defined(__APPLE__) && defined(__MACH__)
	/* Prepare the initial buffer */
	uint32_t size = 247;
	char* path = (char*)malloc(256);
	if (!path)
		return -1;
	/* Retrieve a path to this executable */
	char* p;
	if (_NSGetExecutablePath(path, &size) < 0) {
		p = (char*)realloc(path, size + 9);
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

	/* Allocate memory for settings */
	void* p = malloc(164 * 8); // 164 characters over 6 rows + 2 backups
	if (!p)
		return -1;
	g_plugboard  = (uint8_t*)p;
	g_rotor1_in  = 164 + g_plugboard;
	g_rotor2_in  = 164 + g_rotor1_in;
	g_reflector  = 164 + g_rotor2_in;
	g_rotor2_out = 164 + g_reflector;
	g_rotor1_out = 164 + g_rotor2_out;
	g_backup     = 164 + g_rotor1_out;

	if (settings_b64) {
		// TODO: Load settings
	}
	else {
		// TODO: Try to load settings
		generate_settings();
	}

	return 0;
}
