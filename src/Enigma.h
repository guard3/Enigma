#pragma once

class cEnigma
{
private:
	static char settings[10][94];
	
	static char (&plugboard)[94];
	static char (&rotor1a)[94];
	static char (&rotor1b)[94];
	static char (&rotor2a)[94];
	static char (&rotor2b)[94];
	static char (&reflector)[94];
	
	static int  rot;
	
	static void Backup();
	
public:
	static bool CreateSettings();
	static void ReloadSettings();
	static bool Initialize();
	static void Reset();
	static void PrintHelp();
	static inline char Cypher(const char c)
	{
		if (c < ' ' || c > '}')
			return c;
		
		char result = plugboard[rotor1b[rotor2b[reflector[rotor2a[rotor1a[plugboard[c - ' ']]]]]]] + ' ';
		
		auto Rotate = [](char* r) {
			char c = r[93];
			for (int i = 93; i > 0; --i)
				r[i] = (r[i - 1] + 1) % 94;
			r[0] = (c + 1) % 94;
		};
		
		Rotate(rotor1a);
		Rotate(rotor1b);
		if (++rot == 94)
		{
			rot = 0;
			Rotate(rotor2a);
			Rotate(rotor2b);
		}
		
		return result;
	}
};
