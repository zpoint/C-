#include <iostream>
#include <vector>

#include "fileUtil.h"
#include "ArgUtil.h"
#include "TF_IDF.h"

int main(int argc, char **argv)
{
		if (argc == 1)
		{
				std::cout << "tf_idf v1.0\n\n"
						<< "\t-in <file>\n"
						<< "\t\tRead data from pattern <file>, Can be specific file path or pattern, Pattern example: if <file> is '/root/data/news/::.txt', search all file contains .txt in /root/data/news/\n"
						<< "\t -skip_first <bool>\n"
						<< "\t\t Whether skip first word for each line, default 1\n"
						<< "\t -inN <file>\n"
						<< "\t\tcan be -in2, in4 or ... -in10, you can supply another 10 input file or input file directory\n"
						<< "\t-out <file>\n"
						<< "\t\tDirectory to save output, if not provide, save each result to each input file's directory\n"
						<< "\nExamples:\n"
						<< "./tf_idf -in '/root/data/news2/news_mixed_0.8.txt' -in2 '/root/data/news2/news_mixed_0.2.txt' -out ./ \n"
						<< "Will read news_mixed_0.8.txt and news_mixed_0.2.txt and compute tf_idf and store to ./news_mixed_0.8_tf_idf.txt and "
						<< "./news_mixed_0.2_tf_idf.txt and ./vocab.txt\n"
						<< "Notice, number in vocab.txt is how many line contains the specific word\n"
						<< std::endl;
				return 0;
		}

		int i;
		std::size_t pos;
		bool skip_first = false;
		std::string output_dir = "", vocab_out;
		std::vector<std::string> input_files, output_files;

		if ((i = ArgPos("-in", argc, argv)) > 0) check_input(argv[i + 1], input_files);

		for (int index = 2; index <= 10; ++index)
		{
				// in2 to in10
				static std::ostringstream ostream;
				ostream.flush();
				ostream.clear();
				ostream << "-in" << index;
				if ((i = ArgPos(ostream.str().c_str(), argc, argv, false)) > 0)
						check_input(argv[i + 1], input_files);
				else
						break;
		}

		if ((i = ArgPos("-skip_first", argc, argv, false)) > 0) skip_first = bool(atoi(argv[i + 1]));
		if ((i = ArgPos("-out", argc, argv, false)) > 0) output_dir = argv[i + 1];
		
		check_output_from_input_files(input_files, output_files, "tf_idf");
		
		if (output_dir != "")
		{
				// remove directory if user provide -out
				for (auto &filename : output_files)
				{
						if ((pos = filename.find_last_of("/")) != std::string::npos)
								filename = filename.substr(pos + 1);
				}
				// add user provided directory to each filename
				output_files = check_dir(output_dir, output_files);
				// vocab out directory
				if (output_dir[output_dir.size() - 1] != '/')
						vocab_out = output_dir + "/vocab.txt";
				else
						vocab_out = output_dir + "vocab.txt";
		}
		else
		{
				// vocab_out
				if ((pos = input_files[0].find_last_of("/")) != std::string::npos)
						vocab_out = input_files[0].substr(0, pos) + "/vocab.txt";
				else
						vocab_out = "vocab.txt";
		}

		good_file(vocab_out); // check whether can open vocab out to write

		std::cout << "Added input files\n";
		for (const auto & filename : input_files)
				std::cout << "\t" << filename << "\n";

		std::cout << "\nProcessing " << std::endl;
		tf_idf_from_files(input_files, output_files, vocab_out, skip_first);
		std::cout << "Done" << std::endl;
}
