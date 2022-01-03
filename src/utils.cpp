#include "utils.h"

SystemStats* SystemStats::stats_ = nullptr;
std::mutex SystemStats::lock_;
SystemStats::Destroyer SystemStats::destroyer_;

int64_t timeDiff(TimePoint start, TimePoint end){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

std::string timeToHuman(TimePoint timePoint){
    auto tmp = std::chrono::system_clock::to_time_t(timePoint);
    std::string s(31, '\0');
    std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S ", std::localtime(&tmp));
    return s;
/*
    auto tmp2 = std::string(std::ctime(&tmp));

    auto* buf = std::ctime(&tmp);
    auto tmp2 = std::put_time(std::localtime(&tmp), "");
    auto tmp2 = strftime(std::ctime(&tmp), );
    tmp2.pop_back();// remove new line
    return tmp2; */
}

void SystemStats::MemoryUsage::snapshot(){
    std::lock_guard<std::mutex> guard(lock_);
    struct rusage myusage;
    getrusage(RUSAGE_SELF, &myusage);

    reportedMemUsage_.push_back(myusage.ru_maxrss);
    reportedTime_.push_back(std::chrono::high_resolution_clock::now());
    threadID_.push_back(std::this_thread::get_id());
}

std::stringstream SystemStats::MemoryUsage::getReport(){
    std::lock_guard<std::mutex> guard(lock_);
    std::stringstream ss;
    ss << "Reported memory usage\nTime diff;Memory (KB);Thread id\n";
    for(std::size_t i = 0; i < reportedMemUsage_.size(); ++i){
        
        ss << timeDiff(reportedTime_[0], reportedTime_[i]) << ';' << reportedMemUsage_[i] << ";" << threadID_[i] << '\n';
    }

    return ss;
}

void SystemStats::TimeIntervals::snapshot(const std::string& note){
    std::lock_guard<std::mutex> guard(lock_);
    note_.push_back(note);
    threadID_.push_back(std::this_thread::get_id());
    reportedTime_.push_back(std::chrono::high_resolution_clock::now());
}

std::stringstream SystemStats::TimeIntervals::getReport(){
    std::lock_guard<std::mutex> guard(lock_);
    std::stringstream ss;
    ss << "Reported time;\nTime diff;Thread id;Note\n";
    for(size_t i = 0; i < reportedTime_.size(); ++i){
        
        ss << timeDiff(reportedTime_[0], reportedTime_[i]) << ";" << threadID_[i] << ";" << note_[i] << '\n';
    }

    return ss;
}


std::stringstream SystemStats::getReport(){
    std::stringstream ss;
    ss << memoryUsage_.getReport().rdbuf();
    ss << "\n===================================================\n";
    ss << timeIntervals_.getReport().rdbuf();

    return ss;
}

SystemStats& SystemStats::getInstance(){
    if(stats_ == nullptr){
        std::lock_guard<std::mutex> guard(lock_);
        if(stats_ == nullptr){
            stats_ = new SystemStats();
            destroyer_.setSystemStats(stats_);
        }
    }
    return *stats_;
}

SystemStats::Destroyer::Destroyer(SystemStats* ptr)
:systemStats_(ptr)
{}

SystemStats::Destroyer::~Destroyer(){
    if(systemStats_ != nullptr){
        delete systemStats_;
    }
}

void SystemStats::Destroyer::setSystemStats(SystemStats* ptr){
    systemStats_ = ptr;
}