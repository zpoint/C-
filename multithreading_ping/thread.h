#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

std::string ping(std::string);
double get_avg(std::string);

struct process_struct 
{
		std::string ip_addr;
		double avg_val;
		process_struct() = default;
		process_struct(std::string str, double dval = 0.0): ip_addr(str), avg_val(dval) {}
};

void *get_avg_for_ip_addr(void *ip_val_obj)
{
		process_struct *ip_val_ptr = static_cast<process_struct *>(ip_val_obj);
		std::string line = ping(ip_val_ptr->ip_addr);
		ip_val_ptr->avg_val = get_avg(line);
		pthread_exit(ip_val_obj);
}

#ifdef _WIN32
	#include <Windows.h>
	

#else
	#include <pthread.h>
	std::vector<process_struct> main_func(std::ifstream &fin)
	{
			std::vector<pthread_t> vec_threads;
			std::vector<process_struct> vec_process_struct;

			std::string ip_addr;
			int iret;
			while (fin >> ip_addr)
			{
					process_struct new_val(ip_addr);
					pthread_t thread;  // pthread_t ===> unsigned type, copy assign to push back

					vec_threads.push_back(thread);
					vec_process_struct.push_back(new_val);
			}
			for (std::vector<std::string>::size_type index = 0; index < vec_process_struct.size(); ++index)
			{
					iret = pthread_create(&vec_threads[index], NULL, get_avg_for_ip_addr, (void *)&vec_process_struct[index]);
					if (iret)
					{
							std::cerr << "pthread_create() for ip_addr: " << vec_process_struct[index].ip_addr << " return " << iret << std::endl;
							exit(EXIT_FAILURE);
					}
			}
			// wait for all thread
			// each thread fill it's process_struct
			for (const auto &thread : vec_threads)
					pthread_join(thread, NULL);
			return vec_process_struct;
	}

#endif
