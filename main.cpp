#include <iostream>
#include "Enigma.h"

int main()
{
	/* Initialize Enigma */
	puts("Enigma - by guard3");
	putchar('\n');
	cEnigma::Initialize();
	
	for (;;)
	{
		static char c;
		fputs(">>> ", stdout);
		
		/* Check the first char from stdin for command escape character */
		c = getchar();
		if (c == '\n')
			continue;
		else if (c == '~')
		{
			/* Command mode is triggered */
			char b[3];
			fgets(b, 3, stdin);
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
						puts("Reload tba");
						break;
					case 'N':
					case 'n':
						puts("New tba");
						break;
					case 'H':
					case 'h':
						puts("Help tba");
						break;
					default:
						puts("Invalid command.");
				}
			}
			else
			{
				/*
				 * If more than one chars are input, clear stdin
				 * If compiling with msvc, clear stdin with fflush()
				 * in any other case it's undefined behaviour, so clear with getchar()
				 */
				if (b[1] != 0)
				{
#ifdef _MSC_VER
					fflush(stdin);
#else
					while (getchar() != '\n');
#endif
				}
				puts("Invalid command.");
			}
			putchar('\n');
		}
		else
		{
			/* Cypher the first char we read earlier */
			putchar(cEnigma::Cypher(c));
			
			/* Cypher the rest from stdin */
			while ((c = getchar()) != '\n')
				putchar(cEnigma::Cypher(c));
			
			/* Reset enigma for next input */
			cEnigma::Reset();
			putchar('\n');
			putchar('\n');
		}
		
	}
	return 0;
}
