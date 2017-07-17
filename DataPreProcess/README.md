### DataPreProcess for LSTM (article classfication)

#### Compile

        g++ -std=c++11 labelProcess.cpp -o labelProcess
        g++ -std=c++11 labelCount.cpp -o labelCount

#### labelCount Usage

    ./labelCount

    Invalid argument, Usage <FILE_INPUT> <FILE_OUTPUT> <MIN_SEQUENCE> <MAX_SEQUENCE>
	Example: ./a.out labels_in.txt labels_out.txt 1 100

#### LabelProcess Usage

    LabelProcess v1.0

    Options:
    Parameters for processing:
        -in <file>
            Read data from pattern <file>, Can be specific file path or pattern, Pattern example: if <file> is '/root/date/news/::.txt', search all file contains .txt in /root/data/news/
        -out <file>
            Use <file> to save the result
        -min-count <int>
            This will discard words that appear less than <int> times; default is 1
        -max-count <int>
            This will discard words that appear more than <int> times; If the value set to 0, means don't discard. default is 0
        -rate <float>
            This will seperate your file into two part, if rate == 0.8, The 80% of data will be stored to <FILE>_0.8.txt, 20% of data will be stored to <FILE>_0.2.txt
        -random <int>
            random output, 1 true, 0 false, default 1
        -label <int>
            whether add label for each output line, 1 true, 0 false, default 1
         -sort <int>
            whether sort by word count for each line, 1 true, 0 false, default 1
        -group-size <int>
            if group-size set to 500, means random sort 0-500 lines, 500-1000 lines... until EOF, First sort by length, then sort by random with group size, set to 0 means regard all the file as a group. Default 0

    Examples:
    ./LabelProcess -in '/root/data/news2/news_mixed.txt' -out '/root/data/news2/news_mixed' -min-count 1 -max-count 2000 --rate 0.8
    Will read news_mixed.txt line by line, discard line with words less than 1 or more than 2000, save other line two two part, news_mixed_0.8.txt and news_mixed_0.2.txt


#### Results for LabelProcess

    Processing data from file: /home/zpoint/Desktop/news/娱乐2.txt
    Processing data from file: /home/zpoint/Desktop/news/其他2.txt
    Processing data from file: /home/zpoint/Desktop/news/汽车2.txt
    Processing data from file: /home/zpoint/Desktop/news/美食2.txt
    Processing data from file: /home/zpoint/Desktop/news/房2.txt
    Processing data from file: /home/zpoint/Desktop/news/健康2.txt
    Processing data from file: /home/zpoint/Desktop/news/科技2.txt
    Processing data from file: /home/zpoint/Desktop/news/财经2.txt
    Processing data from file: /home/zpoint/Desktop/news/军事2.txt
    Processing data from file: /home/zpoint/Desktop/news/社会2.txt
    Processing data from file: /home/zpoint/Desktop/news/体育2.txt

    Total data:
    体育 496
    健康 482
    其他 447
    军事 455
    娱乐 444
    房 477
    汽车 487
    社会 484
    科技 471
    美食 442
    财经 459
    total: 5144


    Write to: 
    /home/zpoint/Desktop/news/news_mixed_0.80.txt 4121
    /home/zpoint/Desktop/news/news_mixed_0.20.txt 1023

    Deprecated total line: 287
    Deprecated more than max line: 287
    Deprecated less than min line: 0
    Deprecated other line: 32

