#include "Enigma.h"
#include "EnigmaIO.h"

int main()
{
#ifdef _WIN32
	/* Initialize console */
	if (!cConsole::Initialize())
		return -1;
#endif
	
	/* Initialize Enigma */
	cConsole::WriteLine("Enigma - by guard3");
	cConsole::WriteLine("Type '~h' or '~H' to show help menu.");
	cConsole::WriteChar('\n');
	if (!cEnigma::Initialize())
		return 1;
	
	for (;;)
	{
		static char c;
		cConsole::Write(">>> ");
		
		/* Check the first char from stdin for command escape character */
		c = cConsole::ReadChar();
		if (c == '\n')
			continue;
		else if (c == '~')
		{
			/* Command mode is triggered */
			char b[3];
			cConsole::Read(b);
#ifdef _WIN32
			if (b[1] == '\r')
#else
			if (b[1] == '\n')
#endif
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
						cConsole::WriteLine("Invalid commannd.");
						cConsole::WriteLine("Type '~h' or '~H' to show help menu.");
				}
			}
			else
			{
				/* If more than one chars are input, clear stdin */
#ifdef _WIN32
				if (b[1] != '\n')
#else
				if (b[1] != 0)
#endif
					cConsole::Flush();

				cConsole::WriteLine("Invalid command.");
				cConsole::WriteLine("Type '~h' or '~H' to show help menu.");
			}
			cConsole::WriteChar('\n');
		}
		else
		{
			/* Cypher the first char we read earlier */
			cConsole::WriteChar(cEnigma::Cypher(c));
			
			/* Cypher the rest from stdin */
			while ((c = cConsole::ReadChar()) != '\n')
				cConsole::WriteChar(cEnigma::Cypher(c));
			
			/* Reset enigma for next input */
			cEnigma::Reset();
			cConsole::WriteChar('\n');
			cConsole::WriteChar('\n');
		}
	}
	return 0;
}
