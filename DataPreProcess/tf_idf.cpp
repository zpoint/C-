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
						<< "-in <file>\n"
						<< "\t\tRead data from pattern <file>, Can be specific file path or pattern, Pattern example: if <file> is '/root/data/news/::.txt', search all file contains .txt in /root/data/news/\n"
						<< "\t -skip_first <bool>\n"
						<< "\t\t Whether skip first word for each line, default 1\n"
						<< "\t -inN <file>\n"
						<< "\t\tcan be -in2, in4 or ... -in10, you can supply another 10 input file or input file directory\n"
						<< "\nExamples:\n"
						<< "./tf_idf -in '/root/data/news2/news_mixed_0.8.txt' -in2 '/root/data/news2/news_mixed_0.2.txt' \n"
						<< "Will read news_mixed_0.8.txt and news_mixed_0.2.txt and compute tf_idf and store to /root/data/news2/news_mixed_0.8_tf_idf.txt and "
						<< "/root/data/news_mixed_0.2_tf_idf.txt\n"
						<< std::endl;
				return 0;
		}

		int i;
		bool skip_first = false;
		std::vector<std::string> input_files, output_files;

		if ((i = ArgPos("-in", argc, argv)) > 0) check_input(argv[i + 1], input_files);
		for (int index = 2; index <= 10; ++index)
		{
				static std::ostringstream ostream;
				ostream.flush();
				ostream.clear();
				ostream << "-in" << index;
				if ((i = ArgPos(ostream.str().c_str(), argc, argv, false)) > 0)
				{
						check_input(argv[i + 1], input_files);
						std::cout << "Scanning input file from: " << argv[i + 1] << std::endl;
				}
				else
						break;
		}
		if ((i = ArgPos("-skip_first", argc, argv, false)) > 0) skip_first = bool(atoi(argv[i + 1]));
		check_output_from_input_files(input_files, output_files, "tf_idf");
		tf_idf_from_files(input_files, output_files, skip_first);
}
