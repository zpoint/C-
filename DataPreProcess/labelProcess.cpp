#include <vector>
#include <iomanip>
#include <sstream>

#include "ArgUtil.h"
#include "labelUtil.h"
#include "fileUtil.h"

int main(int argc, char **argv) 
{
  if (argc == 1) {
	std::cout << "LabelProcess v1.2\n\n" << "Options:\n" << "Parameters for processing: <bool> means you can only supply 0 or 1\n"
    << "\t-in <file>\n"
    << "\t\tRead data from pattern <file>, Can be specific file path or pattern, Pattern example: if <file> is '/root/data/news/::.txt', search all file contains .txt in /root/data/news/\n"
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
	<< "\t\twhether sort by word count(sort by length) for each line, 1 true, 0 false, default 1\n"
	<< "\t-group-size <int>\n"
	<< "\t\tif -group-size set to 500, means 0-500 lines are group1, 500-1000 lines are group2 ..., If -group-size set to 0, means all lines are in a single group\n"
	<< "\t-ingroup-sort <bool>\n"
	<< "\t\twhether random sort lines among a single group, default 0, if -word-sort flag set, sort by word count will be performed first, default 0\n"
	<< "\t-outgroup-sort <bool>\n"
	<< "\t\twhether random sort groups among all groups in a single file, default 0, if -word-sort flag set, sort by word count first, if -ingroup_sort set, ingroup_sort will be performed before -outgroup-sort, default 0\n"
	<< "\t-in2 <file>\n"
	<< "\t\t If provided, search all files in -in2\n"
	<< "\t-inN <file>\n"
	<< "\t\tcan be -in3, -in4 or ... -in10, same effort as -in2\n"
	<< "\t -tf_idf <bool>\n"
	<< "\t\twhether compute tf_idf, and store to -out, default 0"
	<< "\nExamples:\n"
	<< "./LabelProcess -in '/root/data/news2/news_mixed.txt' -out '/root/data/news2/news_mixed' -min-count 1 -max-count 2000 --rate 0.8\n"
	<< "Will read news_mixed.txt line by line, discard line with words less than 1 or more than 2000, save other line two two part, "
	   "news_mixed_0.8.txt and news_mixed_0.2.txt\n" << std::endl;
    return 0;
  }

  int i;
  std::size_t pos;
  unsigned min_count = 1, max_count = 0, group_size = 0;
  float rate = 0;
  bool random_flag = true, label_flag = true, sort_by_length = true, ingroup_sort = false, outgroup_sort = false, tf_idf=false;
  std::vector<std::string> input_files, output_files;
  std::string vocab_out;

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
  if ((i = ArgPos("-tf_idf", argc, argv, false)) > 0) tf_idf = bool(atoi(argv[i + 1]));
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
  // vocab_out
  if ((pos = output_files[0].find_last_of("/")) != std::string::npos)
		  vocab_out = output_files[0].substr(pos + 1) + "/vocab.txt";
  else
		  vocab_out = "vocab.txt";

  std::cout << "Processing " << std::endl;
  labelProcess(input_files, output_files, min_count, max_count, rate, label_flag, sort_by_length, group_size, ingroup_sort, outgroup_sort, tf_idf, vocab_out);
  return 0;
}
