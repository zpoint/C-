#include <iostream>
#include <string>
#include <vector>

int main()
{
		int i = 0;
		std::vector<std::string> ivec;
		while (i < 10)
		{
				std::string st = "12";
			ivec.push_back(st);
			++i;
		}
		for (const auto i : ivec)
				std::cout << i << std::endl;
		return 0;
}
