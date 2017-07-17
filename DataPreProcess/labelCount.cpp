/*
 * Usage: ./a.out <FILE_INPUT> <FILE_OUTPUT> <MIN_SEQUENCE> <MAX_SEQUENCE>
   
   >> g++ -std=c++11 labelCount.cpp
   >> ./a.out eval_news.txt label.txt 1 1000 
   >> cat label.txt
   
    体育 45001
	健康 21236
	其他 130908
	军事 11347
	娱乐 92319
	房 6968
	汽车 45723
	社会 10867
	科技 135127
	美食 22224
	财经 126166
	Total 647886
	
	>> 
 * 
 */
#include "ArgUtil.h" 
#include "labelUtil.h"

int main(int argc, char **argv)
{
		if (argc != 5)
		{
				std::cout << "Invalid argument, Usage <FILE_INPUT> <FILE_OUTPUT> <MIN_SEQUENCE> <MAX_SEQUENCE>"
						"\nExample: ./a.out labels_in.txt labels_out.txt 1 100" << std::endl;
				return 0;
		}
		
		std::ifstream f_in(argv[1]);
		if (!f_in.good())
		{
				std::cout << "Unable to open file for reading: " << argv[1] << std::endl;
				return 0;
		}

		std::ofstream f_out(argv[2]);
		if (!f_out.good())
		{
				std::cout << "Unable to open file for writing: " << argv[2] << std::endl;
				return 0;
		}

		unsigned min = std::atoi(argv[3]);
		unsigned max = std::atoi(argv[4]);

		labelCount(f_in, f_out, min, max);

		return 0;
}
