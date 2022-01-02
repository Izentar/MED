
#include "PrefixSpan.h"
#include "utils.h"
#include <map>

class Parser{
    std::map<std::string, std::vector<std::string>> parsed_;
    const std::vector<std::string> none_;

    const std::vector<std::string>& getArgGeneric(const std::string& arg, int expectedNumberOfArg) const{
        auto ret = parsed_.find(arg);
        if(ret == parsed_.end()){
            return none_;
        }
        if(expectedNumberOfArg != -1 && expectedNumberOfArg != static_cast<int>(ret->second.size())){
            return none_;
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

    void printArgs() const{
        for(const auto& it : parsed_){
            std::cout << it.first << " -- ";
            for(const auto& it2 : it.second){
                std::cout << it2 << " ;";
            }
            std::cout << std::endl;
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


int main(int argc, char** argv){
    Parser parser;
    PrefixSpan::PrefixSpan::Flags flags;

    if(argc == 1){
        std::cout << "No arguments" << std::endl;
        return 0;
    }
    std::string s;
    std::vector<std::string> vecargs;
    for(int i = 1; i < argc; ++i){
        vecargs.push_back(argv[i]);
    }

    parser.parse(vecargs);
    PrefixSpan::TransactionIndexType minSupport = parser.getArg<PrefixSpan::TransactionIndexType>("-ms", 1)[0];
    PrefixSpan::IndexType maxPatternSize = parser.getArg<PrefixSpan::TransactionIndexType>("-mp", 1)[0];
    std::vector<std::string> files = parser.getArg("-f", -1);
    std::vector<std::string> outputFile = parser.getArg("-o", -1);
    flags.useThreads = parser.flagExist("-t");
    flags.verbose = parser.flagExist("-v");
    flags.printTransNumb = parser.flagExist("-p");
    auto threadsVec = parser.getArg<unsigned int>("-thr",-1);
    int threads = 0;
    if(! threadsVec.empty()){
        threads = threadsVec[0];
    }

    parser.printArgs();

    if(outputFile.size() != 1){
        int index = 0;
        for(unsigned int i = outputFile.size(); i < files.size(); ++i){
            outputFile.push_back("output_" + std::to_string(index) + ".txt");
            ++index;
        }
    }

    SystemStats& stats = SystemStats::getInstance();
    int num_threads = std::thread::hardware_concurrency();
    std::cout << "Avaliable threads; " << num_threads << std::endl;

    std::fstream out;
    if(outputFile.size() == 1){
        out.open(outputFile[0], std::ofstream::out | std::ios_base::trunc);
        out.close();
    }
    for(size_t i = 0; i < files.size(); ++i){
        if(outputFile.size() == 1){
            out.open(outputFile[0], std::ios_base::in | std::ios_base::app);
        }
        else{
            out.open(outputFile[i], std::ios_base::in | std::ios_base::trunc);
        }

        std::unique_ptr<PrefixSpan::PrefixSpan> algorithm;
        if(flags.useThreads){
            std::cout << "Using multithreading" << std::endl;
            algorithm = std::make_unique<PrefixSpan::PrefixSpan>(minSupport, maxPatternSize, out, PrefixSpan::MIN_MEMORY_USAGE + 1, threads);
        }
        else{
            std::cout << "Single thread" << std::endl;
            algorithm = std::make_unique<PrefixSpan::PrefixSpan>(minSupport, maxPatternSize, out);
        } 
        PrefixSpan::Data data;

        data.load(files[i]);
        stats.timeIntervals_.snapshot("Start PrefixSpan");
        //algorithm.prefixProject(data, false);
        algorithm->prefixProject(data, flags);
        stats.timeIntervals_.snapshot("End PrefixSpan");

        auto report = stats.getReport();
        std::lock_guard<std::mutex> guard(stats.outputSynch);
        std::cout << report.str() << std::endl;

        out.close();
    }
    

    std::cout << "Program ends" << std::endl;
    return 0;
}