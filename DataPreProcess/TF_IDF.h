#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <cmath>

typedef std::map<std::string, unsigned long> map_t;
/*
 * freq_map is : word ===> how many line contain this word
 * freq_map is the idf map
 */

inline std::string skip_first_word(std::string &line)
{
		std::size_t pos = line.find_first_of(" ");
		return (pos != std::string::npos) ? line.substr(pos + 1) : line;
}

void scan_line(const std::string &line, map_t &freq_map)
{
		std::string word;
		std::set<std::string> str_set;
		std::istringstream istring(line);
		while (istring >> word)
		{
				if (str_set.find(word) == str_set.end())
				{
						freq_map[word] += 1;
						str_set.insert(word);
				}
		}
}

long scan_file_to_freq_map(const std::string &file_name, map_t &freq_map, bool skip_first=false)
{
		/*
		 * return line number of the file
		 */
		std::ifstream fin(file_name);
		std::string line;
		std::string word;
		long line_num = 0;
		while (std::getline(fin, line))
		{
				if (skip_first) { line = skip_first_word(line); }
				scan_line(line, freq_map);
				line_num += 1;
		}
		return line_num;
}

void scan_tf_idf(map_t &freq_map, std::string &line, const double &total_count, std::ofstream &os)
{
		double tf_idf;
		map_t line_freq_map;
		std::string word;
		std::istringstream istring(line), istring_bk(line);

		while (istring >> word)
				line_freq_map[word] += 1;
		while (istring_bk >> word)
		{
				tf_idf = line_freq_map[word] * std::log10(total_count / freq_map[word]);
				os << tf_idf << " ";
		}
		os << std::endl;
}

void tf_idf_from_vector(std::vector<std::vector<std::pair<std::string, unsigned>>> &vecs, std::vector<std::string> &files_out)
{
		map_t freq_map;
		double total_count = 0;
		for (const auto &vec : vecs)
		{
				// fill freq_map
				for (const auto &line_pair : vec)
						scan_line(line_pair.first, freq_map);
				total_count += vec.size();
		}

		std::size_t index = 0;
		for (auto &vec : vecs)
		{
				// compute tf_idf and save to output file
				std::ofstream os(files_out[index]);
				for (auto &line_pair : vec)
						scan_tf_idf(freq_map, line_pair.first, total_count, os);
				index += 1;
		}
}

void tf_idf_from_files(const std::vector<std::string> &files_in, const std::vector<std::string> &files_out, bool skip_first=false)
{
		long line_num = 0;
		map_t freq_map;

		for (const auto & filename : files_in)
				line_num += scan_file_to_freq_map(filename, freq_map, skip_first);
		std::size_t index = 0;
		for (const auto & filename : files_in)
		{
				std::ofstream os(files_out[index]);
				std::ifstream in(filename);
				std::string line;

				while (std::getline(in, line))
				{
						line = skip_first ? skip_first_word(line) : line;
						scan_tf_idf(freq_map, line, line_num, os);
				}
				index += 1;
		}
}
