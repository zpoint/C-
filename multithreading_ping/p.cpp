#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <stdio.h>  /* popen */
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "thread.h"
#define MAXLINE 1024
#define MAX_LINE 5  // ping up to 5 lines to generate result 


bool pre_check_sys()
{
		std::cout << "Checking if processor is available..." << std::endl;
		if (system(NULL)) 
		{
				std::cout << "Ok" << std::endl;
				return true;
		}
		else 
		{
				std::cout << "cannot run system command " << std::endl;
				return false;
		}
}

std::string ping(const std::string ip_addr)
{
		char line[MAXLINE];
		std::string last_line;
		FILE *fpin;
		std::vector<std::string> vec_lines;
		std::string command = "ping -c " + std::to_string(MAX_LINE) + " " + ip_addr + " 2>&1";  // static cast
		std::cout << "Executing command " << command << "\r\n";
		if ((fpin = popen(command.c_str(), "r")) == NULL)
		{
				std::cerr << "Popen error, command: " << command << std::endl;
				exit (EXIT_FAILURE);
		}
		while (fgets(line, MAXLINE, fpin) != NULL)
				last_line = line;
		if (pclose(fpin) == -1)
				std::cerr << "pclose error for command: " << command << std::endl;
		return last_line;
}

double get_avg(const std::string line)
{
		// std::cout << line << std::endl;
		if (line.find("min/avg/max/mdev") == std::string::npos) // fail to find correct ping result
				return -1;
		// find '='
		std::size_t pos = line.find_first_of("=");
		std::string sub_line = line.substr(pos + 1);
		// find max
		pos = sub_line.find_first_of("/");
		sub_line = sub_line.substr(pos + 1);
		// find avg
		sub_line = sub_line.substr(0, sub_line.find_first_of("/"));
		return static_cast<double>(atof(sub_line.c_str()));
}



int main(int argc, char **argv)
{
		/*
		std::string ip_addr;
		std::string line;
		if (!pre_check_sys())
				exit (EXIT_FAILURE);
		std::cin >> ip_addr;
		line = ping(ip_addr);
		std::cout << get_avg(line) << std::endl;
		*/
		if (argc != 2)
		{
				std::cerr << "Usage: program_name filename" << std::endl;
				exit(EXIT_FAILURE);
		}

		std::ifstream fin(argv[1]);
		if (!fin.good())
		{
				std::cerr << "Unable to open the file: " << argv[1] << std::endl;
				exit(EXIT_FAILURE);
		}

		if (!pre_check_sys())
		{
				std::cerr << "Can't use sys command" << std::endl;
				exit(EXIT_FAILURE);
		}

		std::vector<process_struct> results = main_func(fin);
		for (const auto &item : results)
				std::cout << item.ip_addr << ": avg is " << item.avg_val << std::endl;

		return 0;
}

