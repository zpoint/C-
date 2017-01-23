#include <string>
#include <vector>
#include <fstream>
std::string ping(std::string);
double get_avg(std::string);

double get_avg_for_ip_addr(void *ip_addr)
{
		std::string line = ping(static_cast<char *>(ip_addr));
		return get_avg(line);
}

#ifdef _WIN32
	#include <Windows.h>
	

#else
	#include <pthread.h>
	double main_func(std::ifstream fin)
	{
			std::vector<pthread_t> vec_threads;
			std::vector<std::string> vec_str;
			std::string ip_addr;
			int iret;
			int index = 0;
			while (fin >> ip_addr)
			{
					vec_str.push_back(ip_addr);
					pthread_t thread;
					vec_threads.push_back(thread);
					iret = pthread_create(&thread, NULL, get_avg_for_ip_addr, (void *)&ip_addr.c_str())
			}
	}

		
