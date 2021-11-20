#pragma once
#ifndef ENIGMA_ENIGMA2_H
#define ENIGMA_ENIGMA2_H
#include <cstdint>
#include <unistd.h>

typedef int32_t tUnicodeChar;

class cEnigma2 final {
private:
#ifdef _WIN32
	...
#else
	static inline int ms_stdin_fd  = STDIN_FILENO;
	static inline int ms_stdout_fd = STDOUT_FILENO;
#endif

	cEnigma2() = default;
	static cEnigma2 ms_instance;

public:
	~cEnigma2();

	static int Initialize();
	static int Initialize(const char* input_file);
	static int Initialize(const char* input_file, const char* output_file);

};

#endif //ENIGMA_ENIGMA2_H
