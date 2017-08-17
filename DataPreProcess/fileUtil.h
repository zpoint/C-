#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>

#include <dirent.h>
#include <sys/stat.h>  // Unix specific header

bool good_file(std::string &filename, bool exit_if_fail=true)
{
		std::ofstream os(filename);
		if (!os.good())
		{
				std::cout << "Unable to open file: " << filename << std::endl;
				if (exit_if_fail) exit(0);
				return false;
		}
		return true;
}

std::vector<std::string> check_dir(std::string &dir, const std::vector<std::string> &out_files, bool exit_if_fail=true)
{
		struct stat s;
		std::size_t pos;
		std::vector<std::string> return_files;
		
		if (stat(dir.c_str(), &s) == 0)
		{
				// is a directory
				if (dir[dir.size() - 1] != '/')
						dir += "/";
				for (std::string filename : out_files)
				{
						if ((pos = filename.find_last_of("/")) != std::string::npos)
								filename = filename.substr(pos + 1);
						filename = dir + filename;
						
						if (stat(filename.c_str(), &s) == 0)
						{
								std::ofstream os(filename);
								if (os.good())
										return_files.push_back(filename);
								else
								{
										std::cout << "Unable to open output file: " << filename << std::endl;
										if (exit_if_fail) exit(0);
								}

						}
						else
						{
								std::cout << "Unknown sub file path: " << filename << std::endl;
								if (exit_if_fail) exit(0);
						}
				}

		}
		else
		{
				std::cout << "Unknown output directory: " << dir << std::endl;
				if (exit_if_fail) exit(0);
		}
		return return_files;
}

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
										fvec.push_back(in_str + ent->d_name);
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

bool check_output_file_state(const std::string &filename, bool exit_if_fail=true)
{
		std::ofstream os(filename);
		if (!os.good())
		{
				std::cout << "Unable to open output file: " << filename << std::endl;
				if (exit_if_fail)
						exit(0);
				return false;
		}
		return true;
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
		for (const auto & filename : std::vector<std::string>{file1, file2})
		{
				if (check_output_file_state(filename))
						output_files.push_back(filename);
		}
}

void check_output_from_input_files(const std::vector<std::string> &input_files, std::vector<std::string> &output_files, const std::string &concate_str)
{
		std::string out_filename;
		for (const std::string &in_filename : input_files)
		{
				std::size_t pos = in_filename.find_last_of(".");
				if (pos != std::string::npos)
						out_filename = in_filename.substr(0, pos) + "_" + concate_str + in_filename.substr(pos);
				else
						out_filename = in_filename + "_" + concate_str;
				if (check_output_file_state(out_filename))
						output_files.push_back(out_filename);
		}
}
