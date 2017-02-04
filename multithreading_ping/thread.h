#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h> /* system, NULL, EXIT_FAILURE */
#include <stdio.h>  /* popen */
std::string ping(std::string);
double get_avg(std::string);

void pause()
{
	int buffer;
	printf("\r\nPress Enter key to continue...");
	std::cin >> buffer;
}

struct process_struct
{
	std::string ip_addr;
	double avg_val;
	process_struct() = default;
	process_struct(std::string str, double dval = 0.0) : ip_addr(str), avg_val(dval) {}
};


#ifdef _WIN32
#include <Windows.h>
#define POPEN _popen
#define PCLOSE _pclose
DWORD WINAPI get_avg_for_ip_addr(LPVOID lpParameter)
{
	process_struct *ip_val_ptr = static_cast<process_struct *>(lpParameter);
	std::string line = ping(ip_val_ptr->ip_addr);
	ip_val_ptr->avg_val = get_avg(line);
	return 0;
}
std::vector<process_struct> main_func(std::ifstream &fin)
{
	std::vector<HANDLE> vec_threads;
	std::vector<process_struct> vec_process_struct;
	std::string ip_addr;
	while (fin >> ip_addr)
	{
		process_struct new_val(ip_addr);
		HANDLE thread;
		vec_process_struct.push_back(ip_addr);
		vec_threads.push_back(thread);
	}
	for (std::vector<std::string>::size_type index = 0; index < vec_process_struct.size(); ++index)
	{
		vec_threads[index] = CreateThread(NULL, 0, get_avg_for_ip_addr, &vec_process_struct[index], 0, 0);
	}
	for (const auto &thread : vec_threads)
	{
		WaitForSingleObject(thread, INFINITE);
	}
	return vec_process_struct;
}




#else
#include <pthread.h>
#define POPEN popen
#define PCLOSE pclose
void *get_avg_for_ip_addr(void *ip_val_obj)
{
	process_struct *ip_val_ptr = static_cast<process_struct *>(ip_val_obj);
	std::string line = ping(ip_val_ptr->ip_addr);
	ip_val_ptr->avg_val = get_avg(line);
	pthread_exit(ip_val_obj);
}
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
			pause();
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
