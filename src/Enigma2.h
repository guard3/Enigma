#pragma once
#ifndef ENIGMA_ENIGMA2_H
#define ENIGMA_ENIGMA2_H

#ifdef _WIN32
#include "Win32.h"
#else
#include <unistd.h>
#endif

#include <cstdint>

typedef int32_t tUnicodeChar;

class cEnigma2 final {
private:

	static cEnigma2 ms_instance;

public:
	~cEnigma2();
	
	static int Initialize(const char* settings_b64);

};

#endif //ENIGMA_ENIGMA2_H
