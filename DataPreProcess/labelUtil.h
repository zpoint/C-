#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <cstdlib>

unsigned label_total;
auto engine = std::default_random_engine{};

template <typename T>
void random_vector(T &vec, unsigned group_size = 0)
{
		if (group_size == 0)
			std::shuffle(vec.begin(), vec.end(), engine);
		else
		{
				typename T::iterator iter_begin = vec.begin();
				while (iter_begin + group_size <= vec.end())
				{
						std::shuffle(iter_begin, iter_begin + group_size, engine);
						iter_begin += group_size;
				}
				std::shuffle(iter_begin, vec.end(), engine);
		}
}

void SortVecLength(std::vector<std::pair<std::string, unsigned>> &svec)
{
		std::sort(svec.begin(), svec.end(), [ ](const std::pair<std::string, unsigned> &lhs, const std::pair<std::string, unsigned> &rhs)
						{ return lhs.second < rhs.second; }
						);
}

void pr_label(std::map<std::string, std::vector<std::pair<std::string, unsigned>>> &label_map)
{
		unsigned long long total = 0;
		std::cout << "\nTotal data: \n";
		for (const auto &pair : label_map)
		{
				total += pair.second.size();
				std::cout << pair.first << " " << pair.second.size() << "\n";
		}
		std::cout << "total: " << total << "\n" << std::endl;
}

void write_out(std::vector<std::string> &out_files, std::map<std::string, std::vector<std::pair<std::string, unsigned>>> &label_map, float rate, bool random_flag, bool sort_by_length, unsigned group_size)
{
	   pr_label(label_map);	
		std::ofstream f_out_first(out_files[0]), f_out_second(out_files[1]);
		std::vector<std::pair<std::string, unsigned>> vec_first, vec_second;
		for (auto &pair : label_map)
		{
				std::string label = pair.first;
				std::vector<std::pair<std::string, unsigned>> &svec = pair.second;
				std::size_t mid_point = int(svec.size() * rate), index = -1;

				for (const auto &s_pair : svec)
				{
						index += 1;
						index > mid_point ? vec_second.push_back(std::move(s_pair)) : vec_first.push_back(std::move(s_pair));
				}
				label_map[label] = std::vector<std::pair<std::string, unsigned>>(); // clear mapped value, since use std::move already
		}
		
		if (sort_by_length)
		{
				SortVecLength(vec_first);
				SortVecLength(vec_second);
		}

		if (random_flag)
		{
				random_vector(vec_first, group_size);
				random_vector(vec_second, group_size);
		}

		for (const auto & pair : vec_first)
				f_out_first << pair.first << "\n";
		for (const auto & pair : vec_second)
				f_out_second << pair.first << "\n";

		std::cout << "\nWrite to: \n" << out_files[0] << " " << vec_first.size() << "\n" << out_files[1] << " " << vec_second.size() << std::endl; 
}

void labelCount(std::ifstream &fin, std::ofstream &fout, unsigned min, unsigned max)
{
		std::map<std::string, unsigned> label_map;
		std::string label, word, line;
		while (std::getline(fin, line))
		{
				unsigned wordCount = 0;
				std::istringstream lineBuffer(line);
				while (lineBuffer >> word)
				{
						wordCount += 1;
						if (wordCount == 1)
								label = word;
				}
				wordCount -= 1;
				// std::cout << "wordCount: " << wordCount << std::endl;
				if (wordCount < min or wordCount > max)
						continue;
				label_map[label] += 1;
				label_total += 1;
		}

		for (const auto & item : label_map)
				fout << item.first << " " << item.second << "\n";
		fout << "Total " << label_total << std::endl;
}

void labelProcess(std::vector<std::string> &in_files, std::vector<std::string> &out_files, size_t min_count, size_t max_count, float rate, bool random_flag, 
				bool label_flag, bool sort_by_length, unsigned group_size)
{
		std::map<std::string, std::vector<std::pair<std::string, unsigned>>> label_map;
		std::string line, label, word;
		unsigned long long depreacted_less_than_min_count = 0, depreacted_more_than_max_count = 0, depreacted_other = 0;
		for (const std::string &fin_str : in_files)
		{
				std::cout << "Processing data from file: " << fin_str << std::endl;
				std::ifstream fin(fin_str);
				if (label_flag)
				{
						std::size_t pos = fin_str.find_last_of("/\\");
						label = (pos != std::string::npos) ? fin_str.substr(pos + 1) : fin_str;
						pos = label.find_first_of(".");
						label = (pos != std::string::npos) ? label.substr(0, pos) : label;
						if (*(label.cend() - 1) == '2')
								label = std::string(label.begin(), label.end() - 1);
				}
				else
						label = fin_str;

				while (std::getline(fin, line))
				{
						unsigned wordCount = 0;
						std::istringstream lineBuffer(line);
						while (lineBuffer >> word)
								wordCount += 1;
						if (wordCount < min_count)
						{
								depreacted_less_than_min_count += 1;
								continue;
						}
						else if (wordCount > max_count)
						{
								depreacted_more_than_max_count += 1;
								continue;
						}
						else if (line == "None " or line == "None")
						{
								depreacted_other += 1;
								continue;
						}
						
						label_map[label].push_back(std::make_pair(label_flag ? label + " " + line : line, wordCount));
				}
		}

		write_out(out_files, label_map, rate, random_flag, sort_by_length, group_size);
		std::cout << "\nDeprecated total line: " << (depreacted_less_than_min_count + depreacted_more_than_max_count) 
				<< "\nDeprecated more than max line: " << depreacted_more_than_max_count << "\nDeprecated less than min line: " << depreacted_less_than_min_count 
				<< "\nDeprecated other line: " << depreacted_other << std::endl;
}

