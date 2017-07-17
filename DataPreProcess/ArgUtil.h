#include <string>
#include <iostream>
#include <cstdlib>

int ArgPos(std::string str, int argc, char **argv, bool exitIfMissing=true)
{
		for (int i = 0; i < argc; ++i)
		{
				if (str != std::string(argv[i]))
				{
						if (i == argc - 1 and exitIfMissing)
						{
								std::cout << "Argument missing for " << str << std::endl;
								exit(0);
						}
				}
				else
						return i;
		}
		return -1;
}
