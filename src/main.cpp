
#include "PrefixSpan.h"
#include "utils.h"
#include <map>
#include <filesystem>

class Parser{
    std::map<std::string, std::vector<std::string>> parsed_;
    const std::vector<std::string> none_;

    const std::vector<std::string>& getArgGeneric(const std::string& arg, int expectedNumberOfArg) const{
        auto ret = parsed_.find(arg);

        // empty
        if(ret == parsed_.end()){
            if(expectedNumberOfArg != -1){
                throw std::runtime_error("Error: Parser: expected flag: " + arg);
            }
            return none_;
        }

        if(expectedNumberOfArg != -1 && expectedNumberOfArg != static_cast<int>(ret->second.size())){
            std::string tmp;
            for (const auto& it : ret->second){
                tmp += it + "; ";
            }
            throw std::runtime_error("Error: Parser: expected more arguments for flag: " + arg + "; Provided arguments: " + tmp);
        }

        return ret->second;
    }

public:
    void parse(const std::vector<std::string>& line){
        std::string key = "none";
        for(unsigned int i = 0; i < line.size(); ++i){
            if(line[i][0] == '-'){
                key = line[i];
                parsed_[key];
            }
            else {
                parsed_[key].push_back(line[i]);
            }
        }
    }

    void printArgs(std::ostream& stream) const{
        stream << "Parsed arguments\n";
        for(const auto& it : parsed_){
            stream << it.first << " == ";
            for(const auto& it2 : it.second){
                stream << it2 << ";";
            }
            stream << std::endl;
        }
    }

    template<typename TargetClass = std::string>
    std::vector<TargetClass> getArg(const std::string& arg, int expectedNumberOfArg) const{
        std::vector<TargetClass> ret;
        auto generic = getArgGeneric(arg, expectedNumberOfArg);
        for(const auto& it : generic){
            ret.push_back(TargetClass(it));
        }
        return ret;
    }

    bool flagExist(const std::string& arg){
        return parsed_.find(arg) != parsed_.end();
    }
};

template<>
std::vector<int> Parser::getArg<int>(const std::string& arg, int expectedNumberOfArg) const{
    std::vector<int> ret;
    auto generic = getArgGeneric(arg, expectedNumberOfArg);
    for(const auto& it : generic){
        ret.push_back(atoi(it.c_str()));
    }
    return ret;
}

template<>
std::vector<unsigned int> Parser::getArg<unsigned int> (const std::string& arg, int expectedNumberOfArg) const{
    std::vector<unsigned int> ret;
    auto generic = getArgGeneric(arg, expectedNumberOfArg);
    for(const auto& it : generic){
        ret.push_back(atoi(it.c_str()));
    }
    return ret;
}

template<>
std::vector<unsigned long int> Parser::getArg<unsigned long int>(const std::string& arg, int expectedNumberOfArg) const{
        std::vector<unsigned long int> ret;
        auto generic = getArgGeneric(arg, expectedNumberOfArg);
        for(const auto& it : generic){
            ret.push_back(atoi(it.c_str()));
        }
        return ret;
    }

struct ParsedArgs{
    unsigned int repeat = 1;
    unsigned int threads = 1;
    std::string outFolder = "something_went_wrong";
    PrefixSpan::TransactionIndexType minSupport;
    PrefixSpan::IndexType maxPatternSize;
};

void help(){
    std::cout << "No arguments provided." << std::endl;
    std::cout << "-ms <integer [1;], required> == minimum support / how many transactions must be in database with evaluated prefix to be recognized as valid\n";
    std::cout << "-mp <integer [1;], required> == maximum pattern lenght. Longer pattern -> more time required\n";
    std::cout << "-f <file path, required> == input database file path\n";
    std::cout << "-o <folder path, required> == folder where program will create output files. This folder must not before exist.\n";
    std::cout << "-v == flag, verbose -> print all prefixes to standard output\n";
    std::cout << "-pt == flag, print transactions numbers -> print for each prefix transactions numbers where program found this prefix. It require more memory and time to process and print out. \
With '-v' flag it will also print transactions to standard output.\n";
    std::cout << "-r <integer [1;]> == how many times invoke PrefixSpan algorithm. Each invocation have separate in memory database, PrefixSpan structures and output. Default 1.\n";
    std::cout << "-t == flag, use multithreading implementation with mutexes and thread pool. Without this flag other multithreading options would not be used\n";
    std::cout << "-thr <integer [1;]> == number of threads to use" << std::endl;
}

bool setupDir(const std::string& folder){
    if(std::filesystem::exists(folder)){
        std::cout << "Folder '" << folder << "' already exist" << std::endl;
        return true;
    }

    if(! std::filesystem::create_directories(folder)){
        return true;
    }

    return false;
}

bool checkParams(ParsedArgs& args, const Parser& parser){
    args.minSupport = parser.getArg<PrefixSpan::TransactionIndexType>("-ms", 1)[0];
    args.maxPatternSize = parser.getArg<PrefixSpan::TransactionIndexType>("-mp", 1)[0];

    auto repeat = parser.getArg<int>("-r", -1);
    if(!repeat.empty()){
        if(repeat.back() <= 0){
            std::cout << "Repeat less than one.\n";
            args.repeat = 0;
            return true;
        }
        args.repeat = repeat[0];
    }

    auto threadsVec = parser.getArg<int>("-thr",-1);
    if(!threadsVec.empty()){
        if(threadsVec.back() <= 0){
            std::cout << "Less than one thread selected.\n";
            args.threads = 0;
            return true;
        }
        args.threads = threadsVec[0];
    }

    args.outFolder = parser.getArg("-o", 1)[0];
    if(setupDir(args.outFolder)){
        std::cout << "Folder creation failed.\n";
        return true;
    }

    return false;
}

int main(int argc, char** argv){
    Parser parser;
    ParsedArgs args;
    PrefixSpan::PrefixSpan::Flags flags;

    int num_threads = std::thread::hardware_concurrency();
    std::cout << "Avaliable threads: " << num_threads << std::endl;

    if(argc == 1){
        help();
        return 0;
    }
    std::string s;
    std::vector<std::string> vecargs;
    for(int i = 1; i < argc; ++i){
        vecargs.push_back(argv[i]);
    }

    parser.parse(vecargs);
    std::string inputFile = parser.getArg("-f", 1)[0];

    flags.useThreads = parser.flagExist("-t");
    flags.verbose = parser.flagExist("-v");
    flags.printTransNumb = parser.flagExist("-pt");

    parser.printArgs(std::cout);

    if(checkParams(args, parser)){
        std::cout << "Program fails" << std::endl;
        return 1;
    }

    SystemStats& stats = SystemStats::getInstance();

    std::fstream outSetup(args.outFolder + "/other.txt", std::ofstream::out | std::ios_base::trunc);
    parser.printArgs(outSetup);

    for(size_t i = 0; i < args.repeat; ++i){
        std::fstream outPattern(args.outFolder + "/" + std::to_string(i) + "_pattern.txt", std::ofstream::out | std::ios_base::trunc);

        std::unique_ptr<PrefixSpan::PrefixSpan> algorithm;
        if(flags.useThreads){
            std::cout << "Using multithreading, loop " << i << std::endl;
            algorithm = std::make_unique<PrefixSpan::PrefixSpan>(args.minSupport, args.maxPatternSize, outPattern, PrefixSpan::MIN_MEMORY_USAGE + 1, args.threads);
        }
        else{
            std::cout << "Single thread, loop " << i << std::endl;
            algorithm = std::make_unique<PrefixSpan::PrefixSpan>(args.minSupport, args.maxPatternSize, outPattern);
        } 
        PrefixSpan::Data data;

        stats.memoryUsage_.snapshot();
        stats.timeIntervals_.snapshot("Start PrefixSpan");
        data.load(inputFile);
        algorithm->prefixProject(data, flags);
        stats.timeIntervals_.snapshot("End PrefixSpan");

        auto reportMem = stats.memoryUsage_.getReport();
        auto reportTime = stats.timeIntervals_.getReport();

        std::fstream fileMemory(args.outFolder + "/" + std::to_string(i) + "_memory.csv", std::ofstream::out | std::ios_base::trunc);
        std::fstream fileTime(args.outFolder + "/" + std::to_string(i) + "_time.csv", std::ofstream::out | std::ios_base::trunc);

        //no need for mutex. All other threads already ended
        fileMemory << reportMem.str();
        fileTime << reportTime.str();
        std::flush(std::cout);
        stats.clear();
    }
    

    std::cout << "Program ends successfully" << std::endl;
    return 0;
}