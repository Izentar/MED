
#include "PrefixSpan.h"
#include "utils.h"


int main(int argc, char** argv){
    std::vector<std::string> files;
    PrefixSpan::TransactionIndexType minSupport = PrefixSpan::TransactionIndexMax;
    PrefixSpan::IndexType maxPatternSize = PrefixSpan::IndexTypeMax;
    std::vector<std::string> outputFile;

    if(argc == 1){
        return 0;
    }
    std::string s;

    for(int i = 1; i < argc; ++i){
        std::string arg(argv[i]);
        if(arg.compare("-f")){
            arg.clear();
            ++i;
            if(i < argc){
                arg.assign(argv[i]);
            }
            while(i < argc && arg.compare(0, 1, "-")){
                files.push_back(argv[i]);
            }
        }
        if(arg.compare("-ms")){
            ++i;
            if(i < argc){
                minSupport = atoi(argv[i]);
            } 
        }
        if(arg.compare("-mp")){
            ++i;
            if(i < argc){
                maxPatternSize = atoi(argv[i]);
            } 
        }
        if(arg.compare("-o")){
            ++i;
            if(i < argc){
                outputFile.push_back(std::string(argv[i]));
            } 
        }
    }

    if(outputFile.empty()){
        outputFile.resize(files.size(), std::string("output.txt"));
    }

    if(outputFile.size() != files.size() && outputFile.size() != 1){
        std::cout << "Different number of input and output files. Note - you can also provide only one output file." << std::endl;
        return 0;
    }

    SystemStats stats = SystemStats::getInstance();
    int num_threads = std::thread::hardware_concurrency() / 2;

    for(size_t i = 0; i < files.size(); ++i){
        std::fstream out(outputFile[i]);
        PrefixSpan::PrefixSpan algorithm(minSupport, maxPatternSize, out);
        PrefixSpan::Data data;
        data.load(files[i]);
        stats.timeIntervals_.snapshot("Start PrefixSpan");
        algorithm.prefixProject(data);
        stats.timeIntervals_.snapshot("End PrefixSpan");
    }


    return 0;
}