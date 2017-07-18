#include "ArgUtil.h"
#include "labelUtil.h"
#include <vector>
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

int main(int argc, char **argv) 
{
  if (argc == 1) {
	std::cout << "LabelProcess v1.1\n\n" << "Options:\n" << "Parameters for processing: <bool> means you can only supply 0 or 1\n"
    << "\t-in <file>\n"
    << "\t\tRead data from pattern <file>, Can be specific file path or pattern, Pattern example: if <file> is '/root/date/news/::.txt', search all file contains .txt in /root/data/news/\n"
	<< "\t-out <file>\n"
	<< "\t\tUse <file> to save the result, check the -rate parameter description\n"
	<< "\t-min-count <int>\n"
	<< "\t\tThis will discard words that appear less than <int> times; default is 1\n"
	<< "\t-max-count <int>\n"
	<< "\t\tThis will discard words that appear more than <int> times; If the value set to 0, means don't discard. default is 0\n"
	<< "\t-rate <float>\n"
	<< "\t\tThis will seperate your file into two part, if rate == 0.8, The 80% of data will be stored to <OUT_FILE>_0.8.txt, 20% of data will be stored to <OUT_FILE>_0.2.txt\n"
	<< "\t-label <bool>\n"
	<< "\t\twhether add label(each file name) in front of each output line, 1 true, 0 false, default 1\n"
	<< "\t -word-sort <bool>\n"
	<< "\t\twhether sort by word count for each line, 1 true, 0 false, default 1\n"
	<< "\t-group-size <int>\n"
	<< "\t\tif -group-size set to 500, means 0-500 lines are group1, 500-1000 lines are group2 ..., If -group-size set to 0, means all lines are in a single group\n"
	<< "\t-ingroup-sort <bool>\n"
	<< "\t\twhether random sort lines among a single group, default 0, if -word-sort flag set, sort by word count will be performed first\n"
	<< "\t-outgroup-sort <bool>\n"
	<< "\t\twhether random sort groups among all groups in a single file, default 0, if -word-sort flag set, sort by word count first, if -ingroup_sort set, ingroup_sort will be performed before -outgroup-sort\n"
	<< "\nExamples:\n"
	<< "./LabelProcess -in '/root/data/news2/news_mixed.txt' -out '/root/data/news2/news_mixed' -min-count 1 -max-count 2000 --rate 0.8\n"
	<< "Will read news_mixed.txt line by line, discard line with words less than 1 or more than 2000, save other line two two part, "
	   "news_mixed_0.8.txt and news_mixed_0.2.txt\n" << std::endl;
    return 0;
  }

  int i;
  unsigned min_count = 1, max_count = 0, group_size = 0;
  float rate = 0;
  bool random_flag = true, label_flag = true, sort_by_length = true, ingroup_sort = false, outgroup_sort = false;
  std::vector<std::string> input_files, output_files;
  if ((i = ArgPos("-rate", argc, argv)) > 0) rate = atof(argv[i + 1]);
  if ((i = ArgPos("-in", argc, argv)) > 0) check_input(argv[i + 1], input_files);
  if ((i = ArgPos("-out", argc, argv)) > 0) check_output(argv[i + 1], rate, output_files);
  if ((i = ArgPos("-min-count", argc, argv, false)) > 0) min_count = atoi(argv[i + 1]);
  if ((i = ArgPos("-max-count", argc, argv, false)) > 0) max_count = atoi(argv[i + 1]);
  if ((i = ArgPos("-random", argc, argv, false)) > 0) random_flag = bool(atoi(argv[i + 1]));
  if ((i = ArgPos("-label", argc, argv, false)) > 0) label_flag = bool(atoi(argv[i + 1]));
  if ((i = ArgPos("-word-sort", argc, argv, false)) > 0) sort_by_length = bool(atoi(argv[i + 1]));
  if ((i = ArgPos("-group-size", argc, argv, false)) > 0) group_size = unsigned(atoi(argv[i + 1]));
  if ((i = ArgPos("-ingroup-sort", argc, argv, false)) > 0) ingroup_sort = bool(atoi(argv[i + 1]));
  if ((i = ArgPos("-outgroup-sort", argc, argv, false)) > 0) outgroup_sort = bool(atoi(argv[i + 1]));
  labelProcess(input_files, output_files, min_count, max_count, rate, label_flag, sort_by_length, group_size, ingroup_sort, outgroup_sort);
  return 0;
}
