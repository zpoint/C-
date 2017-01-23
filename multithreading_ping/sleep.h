#ifdef _WIN32
	#include <windows.h>
	void my_sleep(unsigned int milliseconds)
	{
		Sleep(milliseconds);
	}
#else
	#include <unistd.h>
	void my_sleep(unsigned int milliseconds)
	{
			usleep(milliseconds * 1000); // take microseconds
	}
#endif
