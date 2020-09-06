#include "Enigma.h"
#include "EnigmaIO.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

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

void cEnigma::Backup()
{
	word (&src)[NUM_COPY]  = *reinterpret_cast<word(*)[NUM_COPY]>(settings[1]);
	word (&dest)[NUM_COPY] = *reinterpret_cast<word(*)[NUM_COPY]>(settings[6]);
	for (int i = 0; i < NUM_COPY; ++i)
		dest[i] = src[i];
}

bool cEnigma::CreateSettings()
{
	int i, j;
	
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
	cOFile f("enigma.set");
	if (!f)
	{
		cConsole::WriteLine<COLOR_RED>("Could not initialize Enigma.");
		cConsole::WriteLine("Unable to create settings file.");
		return false;
	}
	if (!f.Write(settings, 6 * 94))
	{
		cConsole::WriteLine<COLOR_RED>("Could not initialize Enigma.");
		cConsole::WriteLine("Creating settings file was interrupted.");
		
		/* Delete faulty file */
		cFile::Delete("enigma.set");
		return false;
	}
	Backup();
	
	/* Print confirmation message */
	cConsole::Write<COLOR_GREEN>("Settings file ");
	cConsole::WriteLine("created.");
	
	return true;
}

void cEnigma::ReloadSettings()
{
	/* Open settings file */
	cIFile f("enigma.set");
	if (!f)
	{
		cConsole::WriteLine<COLOR_RED>("Could not reload settings file.");
		cConsole::Write("Settings file ");
		cConsole::WriteLine("is missing.");
		return;
	}
	
	/* Load contents */
	if (!f.Read(settings, 6 * 94))
	{
		cConsole::WriteLine<COLOR_RED>("Could not reload settings file.");
		cConsole::WriteLine("Invalid size of settings file.");
		return;
	}
	Backup();
	
	/* Print confirmation message */
	cConsole::Write<COLOR_GREEN>("Settings file ");
	cConsole::WriteLine("reloaded.");
}

bool cEnigma::Initialize()
{
	/* Seed random engine */
	srand(static_cast<unsigned int>(time(nullptr)));
	
	/* Try to open settings file */
	cIFile f("enigma.set");
	if (!f)
	{
		/* Settings file can't be opened, so we create a new one */
		return CreateSettings();
	}
	
	/* Settings file is open, so we simply load it */
	if (!f.Read(settings, 6 * 94))
	{
		cConsole::WriteLine<COLOR_RED>("Could not initialize Enigma.");
		cConsole::WriteLine("Invalid size of settings file.");
		return false;
	}
	Backup();
	
	/* Print confirmation message */
	cConsole::Write<COLOR_GREEN>("Settings file ");
	cConsole::WriteLine("loaded.");
	return true;
}

void cEnigma::Reset()
{
	rot = 0;
	word (&src)[NUM_COPY]  = *reinterpret_cast<word(*)[NUM_COPY]>(settings[6]);
	word (&dest)[NUM_COPY] = *reinterpret_cast<word(*)[NUM_COPY]>(settings[1]);
	for (int i = 0; i < NUM_COPY; ++i)
		dest[i] = src[i];
}

void cEnigma::PrintHelp()
{
	cConsole::NewLine();
	cConsole::WriteLine<COLOR_BLUE>("Commands:");
	cConsole::Write<COLOR_YELLOW>("  ~h ~H");
	cConsole::WriteLine<COLOR_DEFAULT>("    Show help menu");
	cConsole::Write<COLOR_YELLOW>("  ~r ~R");
	cConsole::WriteLine<COLOR_DEFAULT>("    Reload settings file");
	cConsole::Write<COLOR_YELLOW>("  ~n ~N");
	cConsole::WriteLine<COLOR_DEFAULT>("    Create a new settings file");
	cConsole::Write<COLOR_YELLOW>("  ~q ~Q");
	cConsole::WriteLine<COLOR_DEFAULT>("    Quit");
}
