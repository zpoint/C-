### Cross platform Multithreading Ping

#### Compile

        g++ -std=c++11 -pthread p.cpp -o mping

or Visual Studio

### Usage

		mping ip_address.txt

or double click **mping**

#### Results

		Checking if processor is available...
		Ok
		Executing, please wait...

        Success ip address:
        ip address: www.qq.com     , average time: 3.26
        ip address: www.baidu.com  , average time: 5.75
        ip address: www.hao123.com , average time: 48.02

        Failure ip address:
        ip address: 123.456.789

        Press Enter key to continue...
