#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "ArgUtil.h"
#include "fileUtil.h"

#define UNK "<UNK>"

typedef std::map<std::string, std::vector<float>> f_map_t;

const long long max_w = 50;

class Word2Vec
{
		public:
			Word2Vec(const std::string &);
			std::vector<float> get_vector(const std::string &);
			void get_line_average(const std::string &, std::vector<double> &);
		private:
			FILE *fp;
			long long words = 0, size = 0;
			f_map_t f_map;
};


Word2Vec::Word2Vec(const std::string &path)
{
		int index;
		float f_val;
		char c_word[max_w];
		std::string word;
		fp = fopen(path.c_str(), "rb");
		if (fp == nullptr)
				throw std::runtime_error("Unable to open vector file: " + path);
		fscanf(fp, "%lld", &words);
		fscanf(fp, "%lld", &size);

		std::cout << "Reading from vector, words: " << words << " size: " << size << std::endl;
		for (int i = 0; i < words; ++i)
		{
				std::vector<float> float_vec;
				index = 0;
				while (true)
				{
						// scan word
						c_word[index] = fgetc(fp);
						if (feof(fp) or c_word[index] == ' ') break;
						if (index < max_w and c_word[index] != '\n') index++;
				}
				c_word[index] = 0;
				word = c_word;
				for (int i = 0; i < size; ++i)
				{
						fread(&f_val, sizeof(float), 1, fp);
						float_vec.push_back(f_val);
				}
				f_map[word] = std::move(float_vec);
		}
}

std::vector<float> Word2Vec::get_vector(const std::string &word)
{
		f_map_t::iterator iter;
		if ((iter = f_map.find(word)) != f_map.end())
				return f_map[word];
		return f_map[UNK];
}

void Word2Vec::get_line_average(const std::string &line, std::vector<double> &ret_vec)
{
		std::istringstream is(line);
		std::string word;
		std::vector<f_map_t::iterator> vec_iter;
		std::size_t vec_size;
		f_map_t::iterator iter;

		while (is >> word)
		{
				if ((iter = f_map.find(word)) == f_map.end())
						iter = f_map.find(UNK);
				vec_iter.push_back(iter);
		}
		
		vec_size = vec_iter.size();
		if (vec_size)
		{
				for (long long index = 0; index < size; ++index)
				{
						double dval = 0;
						for (const auto &iter : vec_iter)
								dval += (*iter).second[index] / vec_size;
						ret_vec.push_back(dval);
				}
		}
}

void process_average(const std::string &vector_path, const std::string &input_file, const std::string output_filename, bool skip_first)
{
		std::size_t count = 0, pos;
		Word2Vec word2vec(vector_path);
		std::ifstream is(input_file);
		std::ofstream os(output_filename);
		std::string line, cate;
		std::vector<double> d_vec;
		os << std::fixed << std::setprecision(16);
		
		std::cout << "Processing" << std::endl;
		while (std::getline(is, line))
		{
				if (skip_first)
				{
						pos = line.find_first_of(" ");
						if (pos != std::string::npos)
						{
								cate = line.substr(0, pos);
								line = line.substr(pos + 1);
								os << cate << " ";
						}
				}
				d_vec.clear();
				word2vec.get_line_average(line, d_vec);
				for (const double &d_val : d_vec)
						os << d_val << " ";
				os << "\n";
				count += 1;
				if (count % 1000 == 0)
						std::cout << "Perform " << count << " line " << std::endl;
		}
}

int main(int argc, char **argv)
{
		if (argc == 1)
		{
				std::cout << "average vec v1.0\n\n"
						<< "Required:\n"
						<< "\t-vector_path <file>\n"
						<< "\t\tvector path to open\n"
						<< "\t-in <file>\n"
						<< "\t\tInput file\n"
						<< "\t-out <file>\n"
						<< "\t\t output directory, if not provide, save each result to each input file's directory\n"
						<< "\t-skip_first <bool>\n"
						<< "\t\t Whether skip first word for each line, default 0\n"
						<< "\n\nExample:\n"
						<< "\t./average_vec -vector_path '/home/zpoint/Documents/news' -in '/home/zpoint/Documents/news/news_mixed_0.80.txt' -out './'\n"
						<< std::endl;
		}
		int i;
		std::size_t pos;
		std::string vector_path, input_file, output_filename, prev_dir, output_dir = "";
		std::vector<std::string> output_files;
		bool skip_first = false;
		if ((i = ArgPos("-vector_path", argc, argv)) > 0) { vector_path = argv[i + 1]; good_file_in(vector_path); }
		if ((i = ArgPos("-in", argc, argv)) > 0) { input_file = argv[i + 1]; good_file_in(input_file); }
		if ((i = ArgPos("-out", argc, argv, false)) > 0) { output_dir = argv[i + 1]; }
		if ((i = ArgPos("-skip_first", argc, argv, false)) > 0) {skip_first = bool(atoi(argv[i + 1])); }
		std::cout << "input_file " << input_file << std::endl;
		prev_dir = (pos = input_file.find_last_of("/")) == std::string::npos ? (input_file + "/") : input_file.substr(0, pos + 1);
		output_filename = (pos = input_file.find_last_of("/")) == std::string::npos ? input_file : input_file.substr(pos + 1);
		output_filename = (pos = output_filename.find_last_of(".")) == std::string::npos ? (output_filename + "_avg") : (output_filename.substr(0, pos) + "_avg" + output_filename.substr(pos));
		if (output_dir != "")
				output_dir = output_dir + (output_dir[output_dir.size() - 1] == '/' ? "" : "/") + output_filename;
		else
				output_dir = prev_dir + output_filename;

		good_file(output_dir);
		std::cout << "vector path: " << vector_path 
				<< "\nRead from : " << input_file 
				<< "\nOutput to: " << output_dir 
				<< "\nskip_first: " << skip_first << std::endl;
		process_average(vector_path, input_file, output_dir, skip_first);	
		return 0;
}
