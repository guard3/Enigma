#include "Enigma.h"
#include "EnigmaIO.h"

int main()
{
#ifdef _WIN32
	/* Initialize console */
	if (!cConsole::Initialize())
		return -1;
#endif
	
	/* Using a lambda because (on windows) it gets messy changing all those colors */
	auto ShowHelpHint = []() {
#ifdef _WIN32
		cConsole::Write("Type ");
		cConsole::ChangeColor(COLOR_YELLOW);
		cConsole::WriteChar('~');
		cConsole::WriteChar('h');
		cConsole::ResetColor();
		cConsole::Write(" or ");
		cConsole::ChangeColor(COLOR_YELLOW);
		cConsole::WriteChar('~');
		cConsole::WriteChar('H');
		cConsole::ResetColor();
		cConsole::WriteLine(" to show help menu.");
#else
		cConsole::WriteLine("Type " COLOR_YELLOW "~h" COLOR_DEFAULT " or " COLOR_YELLOW "~H" COLOR_DEFAULT " to show help menu.");
#endif
	};
	
	/* Initialize Enigma */
#ifdef _WIN32
	cConsole::ChangeColor(COLOR_BLUE);
	cConsole::WriteLine("Enigma - by guard3");
	cConsole::ResetColor();
#else
	cConsole::WriteLine(COLOR_BLUE "Enigma - by guard3" COLOR_DEFAULT);
#endif
	ShowHelpHint();
	cConsole::WriteChar('\n');
	if (!cEnigma::Initialize())
		return 1;
	
	for (;;)
	{
		static char c;
#ifdef _WIN32
		cConsole::ChangeColor(COLOR_GREEN);
		cConsole::Write(">>> ");
		cConsole::ResetColor();
#else
		cConsole::Write(COLOR_GREEN ">>> " COLOR_DEFAULT);
#endif
		
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
#ifdef _WIN32
						cConsole::ChangeColor(COLOR_RED);
						cConsole::WriteLine("Invalid command.");
						cConsole::ResetColor();
#else
						cConsole::WriteLine(COLOR_RED "Invalid command." COLOR_DEFAULT);
#endif
						ShowHelpHint();
				}
			}
			else
			{
				/* If more than one chars are input, clear stdin */
#if 0
#ifdef _WIN32
				if (b[1] != '\n')
#else
				if (b[1] != 0)
#endif
					cConsole::Flush();

				cConsole::WriteLine(COLOR_RED "Invalid command." COLOR_DEFAULT);
#endif
#ifdef _WIN32
				if (b[1] != '\n')
					cConsole::Flush();
				cConsole::ChangeColor(COLOR_RED);
				cConsole::WriteLine("Invalid command.");
				cConsole::ResetColor();
#else
				if (b[1] != 0)
					cConsole::Flush();
				cConsole::WriteLine(COLOR_RED "Invalid command." COLOR_DEFAULT);
#endif
				ShowHelpHint();
			}
			cConsole::WriteChar('\n');
		}
		else
		{
#ifdef _WIN32
			cConsole::ChangeColor(COLOR_BLUE);
#else
			cConsole::Write(COLOR_BLUE);
#endif
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
