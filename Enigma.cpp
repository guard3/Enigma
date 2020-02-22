#include "Enigma.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#if defined(__APPLE__) || defined(_WIN64)
#define NUM_COPY 47
typedef int64_t word;
#else
#define NUM_COPY 94
typedef int32_t word;
#endif

char cEnigma::settings[10][94];
/* * * * * * * * * * * * * * * * * * * * *
 * [ ][ ]...[ ][ ] settings[0] plugboard *
 *  |  |     |  |                        *
 * [ ][ ]...[ ][ ] settings[1] rotor1a   *
 * [ ][ ]...[ ][ ] settings[2] rotor1b   *
 *  |  |     |  |                        *
 * [ ][ ]...[ ][ ] settings[3] rotor2a   *
 * [ ][ ]...[ ][ ] settings[4] rotor2b   *
 *  |  |     |  |                        *
 * [ ][ ]...[ ][ ] settings[5] reflector *
 *                                       *
 * [             ]                       *
 * [             ]             backup    *
 * [             ]                       *
 * [             ]                       *
 * * * * * * * * * * * * * * * * * * * * */
char (&cEnigma::plugboard)[94] = *reinterpret_cast<char(*)[94]>(settings[0]);
char (&cEnigma::rotor1a)[94]   = *reinterpret_cast<char(*)[94]>(settings[1]);
char (&cEnigma::rotor1b)[94]   = *reinterpret_cast<char(*)[94]>(settings[2]);
char (&cEnigma::rotor2a)[94]   = *reinterpret_cast<char(*)[94]>(settings[3]);
char (&cEnigma::rotor2b)[94]   = *reinterpret_cast<char(*)[94]>(settings[4]);
char (&cEnigma::reflector)[94] = *reinterpret_cast<char(*)[94]>(settings[5]);

int cEnigma::rot = 0;

bool cEnigma::Initialize()
{
	/* Try to open settings file */
	FILE* f = fopen("enigma.set", "rb");
	if (f)
	{
		/* Settings file is open, so we simply load it */
		auto itemsRead = fread(settings, 94, 6, f);
		fclose(f);
		if (itemsRead != 6)
		{
			puts("Could not initialize Enigma.");
			puts("Invalid size of settings file.");
			return false;
		}
		
		/* Print confirmation message */
		fputs("Settings file ", stdout);
		puts("loaded.");
	}
	else
	{
		/* Settings file is not open, so we create it */
		int i, j;
		srand(static_cast<unsigned int>(time(nullptr)));
		
		/* Useful lambdas */
		auto Shuffle = [&i, &j](char* table) {
			for (i = 93; i > 0; --i)
			{
				static char c;
				j = rand() % (i + 1);
				c = table[j];
				table[j] = table[i];
				table[i] = c;
			}
		};
		auto MakeRotorXb = [&i](char* table) {
			char (&t)[94] = *reinterpret_cast<char(*)[94]>(table - 94);
			for (i = 0; i < 94; ++i)
				table[t[i]] = i;
		};
		
		/* Fill settings[1] with 0...93 */
		for (i = 0; i < 94; ++i)
			settings[1][i] = i;
		
		/* Shuffle settings[1] and copy to settings[3] */
		for (i = 93; i > 0; --i)
		{
			j = rand() % (i + 1);
			settings[3][i] = settings[1][j];
			settings[1][j] = settings[1][i];
			settings[1][i] = settings[3][i];
		}
		settings[3][0] = settings[1][0];
		
		/* Create match pair for the reflector */
		char (&t)[47][2] = *reinterpret_cast<char(*)[47][2]>(settings[1]);
		for (i = 0; i < 47; ++i)
		{
			reflector[t[i][0]] = t[i][1];
			reflector[t[i][1]] = t[i][0];
		}
		
		/* Reshuffle settings[1] and create pair match for plugboard */
		Shuffle(settings[1]);
		j = rand() % 47;
		for (i = 0; i < j; ++i)
		{
			plugboard[t[i][0]] = t[i][1];
			plugboard[t[i][1]] = t[i][0];
		}
		for (i = 2 * i; i < 94; ++i)
			plugboard[settings[1][i]] = settings[1][i];
		
		/* Prepare rotors */
		Shuffle(rotor1a);
		Shuffle(rotor2a);
		MakeRotorXb(rotor1b);
		MakeRotorXb(rotor2b);
		
		/* Write to file */
		f = fopen("enigma.set", "wb");
		if (!f)
		{
			puts("Could not initialize Enigma.");
			puts("Unable to create settings file.");
			return false;
		}
		auto itemsWritten = fwrite(settings, 94, 6, f);
		fclose(f);
		if (itemsWritten != 6)
		{
			puts("Could not initialize Enigma.");
			puts("Creating settings file was interrupted.");
			
			/* Delete faulty file */
			remove("enigma.set");
			return false;
		}
		
		/* Print confirmation message */;
		fputs("Settings file ", stdout);
		puts("created.");
	}
	
	/* Back up rotors for resetting later */
	word
		*src  = reinterpret_cast<word*>(settings[1]),
		*dest = reinterpret_cast<word*>(settings[6]);
	for (int i = 0; i < NUM_COPY; ++i)
		*dest++ = *src++;
	
	return true;
}

void cEnigma::Reset()
{
	/* Restore rotors from back up */
	word
		*src  = reinterpret_cast<word*>(settings[6]),
		*dest = reinterpret_cast<word*>(settings[1]);
	for (int i = 0; i < NUM_COPY; ++i)
		*dest++ = *src++;
	rot = 0;
}
