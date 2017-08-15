#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <dirent.h>
#include <sys/stat.h>  // Unix specific header

void check_input(std::string in_str, std::vector<std::string> &fvec)
{
		std::string match = "";
		size_t pos = in_str.find("::");
		if (pos != std::string::npos)
		{
				match = in_str.substr(pos + 2);
				in_str = in_str.substr(0, pos);
		}
		// Only compile in Unix sys, for Windows, Use GetFileAttributes
		struct stat s;
		if (stat(in_str.c_str(), &s) == 0)
		{
				if (s.st_mode & S_IFDIR)
				{
						// is a directory
						DIR *dir;
						struct dirent *ent;

						if ((dir = opendir(in_str.c_str())) != NULL)
						{
								while ((ent = readdir(dir)) != NULL)
								{
										if (match != "")
												if (std::string(ent->d_name).find(match) == std::string::npos)
														continue;
										fvec.push_back(std::string(in_str) + ent->d_name);
								}
								closedir(dir);
						}
						else
						{
								std::cout << "Unable to read input dir: " << in_str << std::endl;
								exit(0);
						}
				}
				else if (s.st_mode & S_IFREG)
				{
						// is a file
						fvec.push_back(in_str);
				}
				else
				{
						std::cout << "Unknown file or directory: " << in_str << std::endl;
						exit(0);
				}
		}
		else
		{
				std::cout << "Unknown file or directory: " << in_str << std::endl;
				exit(0);
		}

}

void check_output(std::string out, float rate, std::vector<std::string> &output_files)
{
		std::ostringstream os1, os2;
		std::string tmp;
		std::string file1, file2;

		os1 << std::fixed << std::setprecision(2) << rate;
		file1 = out + "_" + os1.str() + ".txt";
		os2 << std::fixed << std::setprecision(2) << (1 - rate);
		file2 = out + "_" + os2.str() + ".txt";
		std::ofstream f1(file1), f2(file2);
		if (!f1.good())
		{
				std::cout << "Unable to open output file: " << file1 << std::endl;
				exit(0);
		}
		if (!f2.good())
		{
				std::cout << "Unable to open output file: " << file2 << std::endl;
				exit(0);
		}

		output_files.push_back(file1);
		output_files.push_back(file2);
}
