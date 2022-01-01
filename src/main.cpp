#include "Enigma.h"
#include "EnigmaIO.h"

int main()
{
	/* Using a lambda for convenience */
	auto ShowHelpHint = []() {
		cConsole::Write(COLOR_DEFAULT, "Type ", COLOR_YELLOW, "~h ", COLOR_DEFAULT, "or ", COLOR_YELLOW, "~H ", COLOR_DEFAULT, "to show help menu.\n");
	};
	
	/* Initialize Enigma */
	cConsole::Write(COLOR_BLUE, "Enigma - by guard3\n");
	ShowHelpHint();
	cConsole::Write('\n');
	if (!cEnigma::Initialize())
		return 1;
	
	for (char c;;) {
		cConsole::Write(COLOR_GREEN, ">>> ", COLOR_DEFAULT);
		
		/* Check the first char from stdin for command escape character */
		cConsole::Read(c);
		if (c == '\n')
			continue;
		else if (c == '~')
		{
			/* Command mode is triggered */
			char b[3];
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
						cConsole::Write(COLOR_RED, "Invalid command.\n");
						ShowHelpHint();
				}
			}
			else
			{
				/* If more than one chars are input, clear stdin */
				if (b[0] != '\n')
					cConsole::Flush();
				cConsole::Write(COLOR_RED, "Invalid command.\n");
				ShowHelpHint();
			}
			cConsole::Write('\n');
		}
		else
		{
			cConsole::Write(COLOR_BLUE);

			/* Cypher */
			do
			{
				cConsole::Write(cEnigma::Cypher(c));
				cConsole::Read(c);
			} while (c != '\n');
			
			/* Reset enigma for next input */
			cEnigma::Reset();

			cConsole::Write("\n\n");
		}
	}
	return 0;
}
