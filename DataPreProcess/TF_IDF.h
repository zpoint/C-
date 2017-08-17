#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "vectorUtil.h"

#define UNK "UNK_I_AM_A_WORD_THAT_CAN_NEVER_HAVE"

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

void save_vocab(const std::string &output_file, map_t &freq_map, map_t &empty_vocab_map, const long total_line)
{
		std::size_t index = 0;
		std::ofstream os(output_file);
		os << total_line << " " << freq_map.size() << "\n";
		for (const auto & item : freq_map)
		{
				os << item.first << " " << item.second << "\n";
				empty_vocab_map[item.first] = index;
				index += 1;
		}
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

void scan_tf_idf(map_t &freq_map, std::string &line, const double &total_count, std::ofstream &os, map_t &vocab_map)
{
		double tf_idf;
		double line_word_count = 0; // for double division
		map_t::mapped_type total_word_count = 0;
		map_t::iterator iter;
		map_t line_freq_map;
		std::string word, word_out_to_os;
		std::istringstream istring(line), istring_bk(line);

		os << std::fixed << std::setprecision(16);
		while (istring >> word)
		{
				line_freq_map[word] += 1;
				line_word_count += 1;
		}
		while (istring_bk >> word)
		{
				if ((iter = freq_map.find(word)) != freq_map.end())
				{
						total_word_count = (*iter).second + 1;
						word_out_to_os = word;
				}
				else
				{
						total_word_count = 1; // no zero division error
						word_out_to_os = UNK; // new word first meet
				}

				tf_idf = (line_freq_map[word] / line_word_count) * std::log10(total_count / total_word_count);
				os << vocab_map[word_out_to_os] << ":" << tf_idf << " ";
		}
		os << std::endl;
}

void tf_idf_from_vector(std::vector<std::vector<std::pair<std::string, unsigned>>> &vecs, std::vector<std::string> &files_out, const std::string vocab_out,
				std::vector<std::vector<std::pair<std::string, unsigned>>> &vecs_test, std::vector<std::string> &test_files_out)
{
		map_t freq_map, vocab_map;
		freq_map[UNK] = 0;
		double total_count = 0;
		std::size_t index = 0;
		std::cout << "generating frequency map" << std::endl;
		for (const auto &vec : vecs)
		{
				// fill freq_map
				for (const auto &line_pair : vec)
						scan_line(line_pair.first, freq_map);
				total_count += vec.size();
		}
		std::cout << "frequency map generated, total: " << int(total_count) << " line, saving to " << vocab_out << std::endl;
		save_vocab(vocab_out, freq_map, vocab_map, total_count);
		std::cout << "saved, generating tf_idf file" << std::endl;
		for (auto &vec : vecs)
		{
				// compute tf_idf and save to output file
				std::ofstream os(files_out[index]);
				os << total_count << " " << vocab_map.size() << std::endl;
				for (auto &line_pair : vec)
						scan_tf_idf(freq_map, line_pair.first, total_count, os, vocab_map);
				std::cout << "saved to " << files_out[index] << std::endl; 
				index += 1;
		}
		// avoid copy vector again
		index = 0;
		for (auto &vec : vecs_test)
		{
				std::ofstream os(files_out[index]);
				os << total_count << " " << vocab_map.size() << std::endl;
				for (auto &line_pair : vec)
						scan_tf_idf(freq_map, line_pair.first, total_count, os, vocab_map);
				std::cout << "saved to " << test_files_out[index] << std::endl;
		}
}

void tf_idf_from_files(const std::vector<std::string> &files_in, const std::vector<std::string> &files_out, const std::string vocab_out, 
				const std::vector<std::string> &test_files_in, const::std::vector<std::string> &test_files_out, bool skip_first=false)
{
		long line_num = 0;
		std::size_t index = 0;
		map_t freq_map, vocab_map;
		freq_map[UNK] = 0;
		std::vector<std::string> final_files_in = files_in, final_files_out = files_out;

		std::cout << "generating frequency map" << std::endl;
		for (const auto & filename : files_in)
				line_num += scan_file_to_freq_map(filename, freq_map, skip_first);
		std::cout << "frequency map generated, total: " << line_num << " line, saving to " << vocab_out << std::endl;
		save_vocab(vocab_out, freq_map, vocab_map, line_num);
		std::cout << "saved, generating tf_idf file" << std::endl;

		final_files_in += test_files_in;
		final_files_out += test_files_out;

		for (const auto & filename : final_files_in)
		{
				std::ofstream os(final_files_out[index]);
				std::ifstream in(filename);
				std::string line;
				os << line_num << " " << vocab_map.size() << std::endl;
				while (std::getline(in, line))
				{
						line = skip_first ? skip_first_word(line) : line;
						scan_tf_idf(freq_map, line, line_num, os, vocab_map);
				}
				std::cout << "saved to " << final_files_out[index] << std::endl;
				index += 1;
		}
}

