#include <algorithm>
#include "thread.h"
#define MAXLINE 1024
#define MAX_LINE 5  // ping up to 5 lines to generate result 
#ifdef _WIN32
#define PING_COMMAND std::string( "ping")

double get_avg(const std::string line)
{
	// std::cout << line << std::endl;
	std::size_t pos = line.find("Average ="), pos_end;
	if (pos == std::string::npos) // fail in findling average
		return -1;

	std::string sub_line = line.substr(pos + 1);
	// find =
	pos = sub_line.find_first_of("=") + 2;
	pos_end = sub_line.find_first_of("m");
	sub_line = sub_line.substr(pos, pos_end - pos);
	return static_cast<double>(atof(sub_line.c_str()));
}

# else
#define PING_COMMAND std::string( "ping -c ") + std::to_string(MAX_LINE)
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
#endif


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
	std::string command = PING_COMMAND + " " + ip_addr + " 2>&1";  // static cast
	// std::cout << "Executing command " << command << "\r\n";
	if ((fpin = POPEN(command.c_str(), "r")) == NULL)
	{
		std::cerr << "Popen error, command: " << command << std::endl;
		exit(EXIT_FAILURE);
	}
	while (fgets(line, MAXLINE, fpin) != NULL)
		last_line = line;
	if (PCLOSE(fpin) == -1)
		std::cerr << "pclose error for command: " << command << std::endl;
	return last_line;
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Usage: program_name filename" << std::endl;
		pause();
		exit(EXIT_FAILURE);
	}

	std::string filename;
	if (argc == 1)
	{
			std::cout << "Please enter a file name which stores ip address line by line: " << std::endl;
			std::cin >> filename;
	}
	else
			filename = argv[1];

	std::ifstream fin(filename);
	if (!fin.good())
	{
		std::cerr << "Unable to open the file: " << filename << std::endl;
		pause();
		exit(EXIT_FAILURE);
	}

	if (!pre_check_sys())
	{
		std::cerr << "Can't use sys command" << std::endl;
		pause();
		exit(EXIT_FAILURE);
	}
	std::cout << "Executing, please wait..." << std::endl;
	std::vector<process_struct> results = main_func(fin);
	if (!results.size())
	{
	NO_VALID:
		std::cout << "No valid ip address!!!" << std::endl;
		pause();
		exit(EXIT_FAILURE);
	}
	std::sort(results.begin(), results.end(),
		[](const process_struct &a, const process_struct &b) { return a.avg_val < b.avg_val; });
	auto good_item = std::find_if_not(results.begin(), results.end(), [](const process_struct &a) { return a.avg_val <= 0; });
	if (good_item == results.end())
		goto NO_VALID;
	std::cout << "\r\nSuccess ip address:" << std::endl;
	std::for_each(good_item, results.end(),
		[](const process_struct &a) { printf("ip address: %-15s, average time: %-6.2f\r\n", a.ip_addr.c_str(), a.avg_val); });
	if (good_item != results.begin())
	{
		std::cout << "\r\nFailure ip address: " << std::endl;
		std::for_each(results.begin(), good_item,
			[](const process_struct &a) { std::cout << "ip address: " << a.ip_addr << std::endl;});
	}
	pause();
	return 0;
}

