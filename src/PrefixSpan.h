#ifndef PREFIXSPAN_H
#define PREFIXSPAN_H

#include <fstream>
#include <vector>
#include <iostream>
#include <set>
#include <sstream>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstdio>

#include "utils.h"

namespace PrefixSpan{

using std::placeholders::_1;

typedef unsigned short int IndexType;
typedef unsigned int TransactionIndexType;

TransactionIndexType TransactionIndexMax = UINT32_MAX;
IndexType IndexTypeMax = UINT16_MAX;

typedef std::vector<IndexType> Pattern;
typedef std::vector<IndexType> TransactionData;
typedef std::pair<TransactionIndexType, TransactionData> Transaction;

typedef unsigned int Priority;
typedef std::function<void()> ThreadFunDef;
typedef std::pair<Priority, ThreadFunDef> FunWithRank;

static const size_t MIN_MEMORY_USAGE;

std::string patternToStr(const Pattern& pattern){
    std::string str;
    for(const auto i : pattern){
        str += std::to_string(i);
    }
    return str;
}


class DataIterator;

struct Data{
    std::vector<Transaction> data_;

    Data() = default;

    bool load(const std::string& filename);
};

class DataProjection{
    std::vector<const Transaction*> transactionsRef_;
    std::vector<IndexType> startIndex_;

public:
    DataProjection() = default;
    DataProjection(const Data& data);
    DataProjection(const std::string& loadFromFile);

    const Transaction& getTransaction(const TransactionIndexType index) const;
    IndexType getIndeces(const IndexType index) const;
    void pushTransaction(const Transaction& transaction);
    void pushIndeces(const IndexType index);
    TransactionIndexType size() const;
    void clear();

    std::string dumpToTmpFile();

};

class Semaphore{
    mutable std::mutex lock_;
    std::condition_variable condition_;
    int counter_;

public:
    Semaphore(int value);

    void release();
    void acquire();
    bool try_acquire();

    int getValue();
};

class ThreadPool{
    std::vector<std::thread> pool_;
    std::vector<FunWithRank> jobs_; // sorted as heap
    mutable std::mutex lock_;
    mutable Semaphore semaphore_;

    void worker();

public:
    struct CmpQueue{
        bool operator()(const FunWithRank& a1, const FunWithRank& a2){
            return a1.first < a2.first;
        }
    };

    ThreadPool(unsigned int numThreads);

    void addJob(std::function<void()> fun, Priority priority);
    int threadOccupancy() const;
};

class PrefixSpan{
    const TransactionIndexType minSupport_;
    const IndexType maxPatternSize_;
    std::fstream& outFile_;
    std::unique_ptr<ThreadPool> threadPool_;
    const size_t maxMemoryUsage_;
    unsigned int numOfThreads_;
    Semaphore semaphore_;

    void saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose);
    void prefixProjectImpl(std::shared_ptr<const DataProjection> database, bool verbose, bool useThreads, Priority recursiveLevel, Pattern prefixPattern = Pattern());
    std::shared_ptr<const DataProjection> prefixProjectImplWithLoopState(std::shared_ptr<const DataProjection> database, bool verbose, bool useThreads, \
        Priority recursiveLevel, Pattern prefixPattern, const IndexType itItemCount, const TransactionIndexType dataSize);

public:
    PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile);
    PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile, size_t maxMemoryUsage, unsigned int numOfThreads);
    void prefixProject(const Data& database, bool verbose = true, bool useThreads = false);
};

}

#endif