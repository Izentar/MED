#ifndef PREFIXSPAN_UTILS_H
#define PREFIXSPAN_UTILS_H

#include<stdlib.h>
#include <sys/resource.h>
#include <vector>
#include <sstream>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>
#include <iostream>

typedef std::chrono::_V2::high_resolution_clock::time_point TimePoint;

std::string timeToHuman(TimePoint timePoint);

class SystemStats{    
public:
    std::mutex outputSynch;

    class MemoryUsage{
        std::vector<size_t> reportedMemUsage_;
        std::vector<TimePoint> reportedTime_;
        std::vector<std::thread::id> threadID_;
        std::mutex lock_;

    public:
        void snapshot();
        void clear();

        std::stringstream getReport();
    };

    class TimeIntervals{
        std::vector<TimePoint> reportedTime_;
        std::vector<std::thread::id> threadID_;
        std::vector<std::string> note_;
        std::mutex lock_;

    public:
        void snapshot(const std::string& note);
        void clear();

        std::stringstream getReport();
    };

    class Destroyer{
        SystemStats *systemStats_;

        public:
            Destroyer(SystemStats* ptr = nullptr);
            ~Destroyer();
            void setSystemStats(SystemStats* ptr);
    };

    friend class Destroyer;

    static SystemStats& getInstance();
    static void deleteInstance();

    MemoryUsage memoryUsage_;
    TimeIntervals timeIntervals_;

    std::stringstream getReport();
    void clear();

    SystemStats(const SystemStats&) = delete;
    SystemStats& operator=(const SystemStats&) = delete;

private:
    SystemStats() = default;
    ~SystemStats() = default;

    static SystemStats* stats_;
    static Destroyer destroyer_;
    static std::mutex lock_;    
};

#endif