#include "Enigma.h"
#include "EnigmaIO.h"
#include <iostream>

int main()
{
	/* Using a lambda because (on windows) it gets messy changing all those colors */
	auto ShowHelpHint = []() {
		cConsole::Write<COLOR_DEFAULT>("Type ");
		cConsole::Write<COLOR_YELLOW>("~h ");
		cConsole::Write<COLOR_DEFAULT>("or ");
		cConsole::Write<COLOR_YELLOW>("~H ");
		cConsole::WriteLine<COLOR_DEFAULT>("to show help menu.");
	};
	
	/* Initialize Enigma */
	cConsole::WriteLine<COLOR_BLUE>("Enigma - by guard3");
	ShowHelpHint();
	cConsole::NewLine();
	if (!cEnigma::Initialize())
		return 1;
	
	for (;;)
	{
		static char c;
		cConsole::Write<COLOR_GREEN>(">>> ");
		cConsole::SetColor(COLOR_DEFAULT);
		
		/* Check the first char from stdin for command escape character */
		cConsole::ReadSkipCarriageReturn(c);
		if (c == '\n')
			continue;
		else if (c == '~')
		{
			/* Command mode is triggered */
			static char b[3];
			cConsole::Read(b);
			if (b[1] == '\n')
			{
				/* If only one char was read */
				switch (b[0])
				{
					case 'Q':
					case 'q':
						return 0;
					case 'R':
					case 'r':
						cEnigma::ReloadSettings();
						break;
					case 'N':
					case 'n':
						if (!cEnigma::CreateSettings())
							return 2;
						break;
					case 'H':
					case 'h':
						cEnigma::PrintHelp();
						break;
					default:
						cConsole::WriteLine<COLOR_RED>("Invalid command.");
						ShowHelpHint();
				}
			}
			else
			{
				/* If more than one chars are input, clear stdin */
				if (b[0] != '\n')
					cConsole::Flush();
				cConsole::WriteLine<COLOR_RED>("Invalid command.");
				ShowHelpHint();
			}
			cConsole::NewLine();
		}
		else
		{
			cConsole::SetColor(COLOR_BLUE);

			/* Cypher */
			do
			{
				cConsole::Write(cEnigma::Cypher(c));
				cConsole::Read(c);
			} while (c != '\n');
			
			/* Reset enigma for next input */
			cEnigma::Reset();

			cConsole::NewLine();
			cConsole::NewLine();
		}
	}
	return 0;
}