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
#include <chrono>

unsigned label_total;
auto engine = std::default_random_engine{};

template <typename T>
void randomIngroupVector(T &vec, unsigned group_size)
{
		if (group_size == 0)
			return;
		else if (group_size > 1)
		{
				typename T::iterator iter_begin = vec.begin();
				while (iter_begin + group_size <= vec.end())
				{
						std::shuffle(iter_begin, iter_begin + group_size, engine);
						iter_begin += group_size;
				}
				std::shuffle(iter_begin, vec.end(), engine);
		}
		else
				std::cerr << "Warning: randomIngroupVector group_size less equal than 1 bu not equal to 0, group_size: " << group_size << std::endl;
}

template <typename T>
void randomOutgropVector(T &vec, unsigned group_size)
{
		if (group_size == 0)
				return;
		else if (group_size > 1)
		{
				T vec_temp(vec.size());
				int index = 0;
				unsigned iner_counter = 0;
				std::vector<unsigned> index_vec(vec.size() / group_size + 1);
				typename T::iterator iter_begin = vec.begin(), iter_end = vec.end(), tmp_iter_begin, tmp_iter_end;
				for (auto &val : index_vec)
				{
						val = index;
						index += group_size;
				}
				std::shuffle(index_vec.begin(), index_vec.end(), engine);
				for (const auto &index_val : index_vec)
				{
						tmp_iter_begin = iter_begin + index_val;
						tmp_iter_end = tmp_iter_begin + group_size;
						if (tmp_iter_end > iter_end)
								tmp_iter_end = iter_end;

						while (tmp_iter_begin < tmp_iter_end)
						{
								vec_temp[iner_counter] = std::move(*tmp_iter_begin);
								iner_counter += 1;
								tmp_iter_begin += 1;
						}
				}
				vec = std::move(vec_temp);
		}
		else
				std::cerr << "Warning: randomOutgropVector group_size less equal than 1 but not equal to 0, group_size: " << group_size << std::endl;
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

void write_out(std::vector<std::string> &out_files, std::map<std::string, std::vector<std::pair<std::string, unsigned>>> &label_map, float rate, 
				bool sort_by_length, unsigned group_size, bool ingroup_sort, bool outgroup_sort)
{
		engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
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
		// std::cout << "sort_by_length: " << sort_by_length << " ingroup_sort: " << ingroup_sort << " outgroup_sort: " << outgroup_sort << std::endl;		
		if (sort_by_length)
		{
				SortVecLength(vec_first);
				SortVecLength(vec_second);
		}

		if (ingroup_sort)
		{
				randomIngroupVector(vec_first, group_size);
				randomIngroupVector(vec_second, group_size);
		}

		if (outgroup_sort)
		{
				randomOutgropVector(vec_first, group_size);
				randomOutgropVector(vec_second, group_size);
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

void labelProcess(std::vector<std::string> &in_files, std::vector<std::string> &out_files, size_t min_count, size_t max_count, float rate, 
				bool label_flag, bool sort_by_length, unsigned group_size, bool ingroup_sort, bool outgroup_sort)
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

		write_out(out_files, label_map, rate, sort_by_length, group_size, ingroup_sort, outgroup_sort);
		std::cout << "\nDeprecated total line: " << (depreacted_less_than_min_count + depreacted_more_than_max_count) 
				<< "\nDeprecated more than max line: " << depreacted_more_than_max_count << "\nDeprecated less than min line: " << depreacted_less_than_min_count 
				<< "\nDeprecated other line: " << depreacted_other << std::endl;
}

