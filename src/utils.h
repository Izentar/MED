#ifndef UTILS_H
#define UTILS_H

#include<stdlib.h>
#include <sys/resource.h>
#include <vector>
#include <sstream>
#include <chrono>

std::string timeToHuman(std::chrono::_V2::system_clock::time_point timePoint){
    auto tmp = std::chrono::system_clock::to_time_t(timePoint);
    return std::string(std::ctime(&tmp));
}

class SystemStats{    
    

public:
    class MemoryUsage{
        std::vector<size_t> reportedMemUsage_;
        std::vector<std::chrono::_V2::system_clock::time_point> reportedTime_;

    public:
        void snapshot(){
            struct rusage myusage;
            getrusage(RUSAGE_SELF, &myusage);

            reportedMemUsage_.push_back(myusage.ru_maxrss);
            reportedTime_.push_back(std::chrono::system_clock::now());
        }

        std::stringstream getReport(){
            std::stringstream ss;
            ss << "Reported memory usage:\nTime\tMemory (KB)\n-----------------------------------------\n";
            for(std::size_t i = 0; i < reportedMemUsage_.size(); ++i){
                
                ss << timeToHuman(reportedTime_[i]) << "\t" << reportedMemUsage_[i] << '\n';
            }

            return ss;
        }
    };

    class TimeIntervals{
        std::vector<std::chrono::_V2::system_clock::time_point> reportedTime_;
        std::vector<std::string> note_;

    public:
        void snapshot(const std::string& note){
            note_.push_back(note);
            reportedTime_.push_back(std::chrono::system_clock::now());
        }

        std::stringstream getReport(){
            std::stringstream ss;
            ss << "Reported time:\nTime\tNote\n-----------------------------------------\n";
            for(size_t i = 0; i < reportedTime_.size(); ++i){
                
                ss << timeToHuman(reportedTime_[i]) << "\t" << note_[i] << '\n';
            }

            return ss;
        }
    };


    static SystemStats& getInstance(){
        static SystemStats instance;
        return instance;
    }

    MemoryUsage memoryUsage_;
    TimeIntervals timeIntervals_;

    std::stringstream getReport(){
        std::stringstream ss;
        ss << memoryUsage_.getReport().rdbuf();
        ss << "\n===================================================\n";
        ss << timeIntervals_.getReport().rdbuf();

        return ss;
    }

private:
    SystemStats() = default;
    
};

#endif