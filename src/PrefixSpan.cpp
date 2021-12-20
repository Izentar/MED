#include "PrefixSpan.h"

namespace PrefixSpan{

bool Data::load(const std::string& filename){
    if(data_.size() != 0){
        return false;
    }
    std::ifstream input(filename);
    std::string line;
    TransactionIndexType id = 0;

    while(getline(input, line)){
        std::stringstream ll(line);
        IndexType number;
        TransactionData transData;
        while(ll >> number){
            transData.push_back(number);
        }
        data_.push_back(std::make_pair(id, std::move(transData)));
        
        ++id;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataProjection::DataProjection(const Data& data)
:transactionsRef_(), startIndex_()
{
    startIndex_.resize(data.data_.size(), 0);

    transactionsRef_.reserve(data.data_.size());
    for(TransactionIndexType itData = 0; itData != data.data_.size(); ++itData){
        transactionsRef_.push_back(&data.data_[itData]);
    }
}

DataProjection::DataProjection(const std::string& loadFromFile)
:transactionsRef_(), startIndex_()
{
    throw std::runtime_error("Not implemented " + loadFromFile);
}

std::string DataProjection::dumpToTmpFile(){
    std::string name("datatmp.");
    throw std::runtime_error("Not implemented");
}

const Transaction& DataProjection::getTransaction(const TransactionIndexType index) const{
    return *transactionsRef_[index];
}

IndexType DataProjection::DataProjection::getIndeces(const IndexType index) const{
    return startIndex_[index];
}

void DataProjection::pushTransaction(const Transaction& transaction){
    transactionsRef_.push_back(&transaction);
}

void DataProjection::pushIndeces(const IndexType index){
    startIndex_.push_back(index);
}

TransactionIndexType DataProjection::size() const{
    return transactionsRef_.size();
}

void DataProjection::clear(){
    transactionsRef_.clear();
    startIndex_.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrefixSpan::PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile)
:minSupport_(minSupport), maxPatternSize_(maxPatternSize), outFile_(outFile), threadPool_(nullptr), maxMemoryUsage_(0), numOfThreads_(1), semaphore_(0)
{}

PrefixSpan::PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile, size_t maxMemoryUsage, unsigned int numOfThreads)
:minSupport_(minSupport), maxPatternSize_(maxPatternSize), outFile_(outFile), threadPool_(nullptr), maxMemoryUsage_(maxMemoryUsage), numOfThreads_(numOfThreads), semaphore_(numOfThreads)
{}

void PrefixSpan::saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose, bool printTransNumb, bool useThreads){
    std::stringstream tmp;
    SystemStats& stats = SystemStats::getInstance();

    stats.memoryUsage_.snapshot();

    // print prefix
    for(const auto it : prefixPattern){
        tmp << it << " ";
    }
    tmp << '\n';
    
    // print all transaction in current database
    if(printTransNumb){
        tmp << "(";
        for(TransactionIndexType i = 0; i < data.size(); ++i){
            tmp << data.getTransaction(i).first << " ";
        }
        tmp << ") #" << data.size() << '\n';
    }

    auto flush = [&]{
        std::string strtmp = tmp.str();
        outFile_ << strtmp;
        std::flush(outFile_);

        if(verbose){
            std::cout << strtmp;
            std::flush(std::cout);
        }
    };

    if(useThreads){
        std::lock_guard<std::mutex> guard(stats.outputSynch);
        flush();
    }
    else{
        flush();
    }
}

void PrefixSpan::prefixProject(const Data& database, bool verbose, bool printTransNumb, bool useThreads){
    auto data = std::make_shared<const DataProjection>(database);

    if(useThreads && numOfThreads_ > 1){
        if(MIN_MEMORY_USAGE > maxMemoryUsage_){
            throw std::runtime_error("Bad max memory usage. Was maxMemoryUsage set for multithreading?");
        }
        this->threadPool_ = std::make_unique<ThreadPool>(numOfThreads_);
        threadPool_->addJob([this, data, verbose, printTransNumb, useThreads]{this->prefixProjectImpl(data, verbose, printTransNumb, useThreads, 0);}, 0);
        threadPool_->endThreadsWait();
    }
    else if(useThreads && numOfThreads_ <= 1){
        throw std::runtime_error("Set flag for multithreading but bad numOfThreads variable. Was numOfThreads set for multithreading?");
    }
    else{
        prefixProjectImpl(data, verbose, printTransNumb, useThreads, 0);
    }
    
}

PrefixSpan::~PrefixSpan(){}

void PrefixSpan::prefixProjectImpl(std::shared_ptr<const DataProjection> database, bool verbose, bool printTransNumb, bool useThreads, Priority recursiveLevel, Pattern prefixPattern){
    std::set<IndexType> itemSet;
    ++recursiveLevel;

    // new database with some prefix
    if (database->size() < this->minSupport_){
        return;
    }    

    saveInfo(*database, prefixPattern, verbose, printTransNumb, useThreads);

    // prefix has max lenght
    if(this->maxPatternSize_ != 0 && prefixPattern.size() >= this->maxPatternSize_){
        return;
    }
    
    // sort items, that are in the dataset. Those that arent in the dataset wont be included.
    // it will speed up algorithm
    const auto dataSize = database->size();
    for(TransactionIndexType itTransaction = 0; itTransaction != dataSize; ++itTransaction){
        const auto& transaction = database->getTransaction(itTransaction).second;
        const auto transSize = transaction.size();
        for(auto itItem = database->getIndeces(itTransaction); itItem != transSize; ++itItem){
            itemSet.insert(transaction[itItem]);
        }
    }

    // loop that says - check if in this database exist prefix + itItemCount where between sum can be other items that != itItemCount.
    // if so, add those to database, remember index and call recursive with new database.
    for(auto itItemCount : itemSet){
        
        if(useThreads){
            // doing so we save on memory, because no new database will be created. Most of the memory is used by transactions and not by itemSet.
            // threads prefer jobs that are deep into recursion, so it is possible that those would end sooner, freeing up some memory.
            threadPool_->addJob([this, database, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern, itItemCount, dataSize]
                    {this->prefixProjectImplWithLoopState(database, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern, itItemCount, dataSize);}, recursiveLevel);
        }
        else{
            this->prefixProjectImplWithLoopState(database, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern, itItemCount, dataSize);
        }
    }
}

void PrefixSpan::prefixProjectImplWithLoopState(std::shared_ptr<const DataProjection> database, bool verbose, bool printTransNumb, bool useThreads, \
    Priority recursiveLevel, Pattern prefixPattern, const IndexType itItemCount, const TransactionIndexType dataSize){

    std::shared_ptr<DataProjection> newData = std::make_shared<DataProjection>();
    SystemStats& stats = SystemStats::getInstance();

    // go through all transactions and add those to newData, that equals to evaluated item from itemSet
    // it must be noted, that all transactions in evaluated database have the same prefixes.
    for(TransactionIndexType itTransaction = 0; itTransaction != dataSize; ++itTransaction){
        const auto& transaction = database->getTransaction(itTransaction);

        // the next item may be not next to the previous item. For example having prefix <abc> and searching for <d>
        // will occur in <abcefghdpi>
        for(auto itItem = database->getIndeces(itTransaction); itItem != transaction.second.size(); ++itItem){
            if(transaction.second[itItem] == itItemCount){
                newData->pushTransaction(transaction);
                newData->pushIndeces(itItem + 1); // start searching for pattern at the next item for this transaction
                break;
            }
        }
    }

    // after going through all, push eveluated item to the searched pattern, creating new prefix
    // and for all transactions with this new prefix call prefixProjectImpl
    prefixPattern.push_back(itItemCount);
    stats.timeIntervals_.snapshot(std::string("Start iteration for pattern: ") + patternToStr(prefixPattern));
    if(useThreads){
        threadPool_->addJob([this, newData, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern]
                {this->prefixProjectImpl(newData, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern);}, recursiveLevel);
        // newData->clear(); - data will be cleared automatically by shared_ptr
    }
    else{
        this->prefixProjectImpl(newData, verbose, printTransNumb, useThreads, recursiveLevel, prefixPattern);
        // newData->clear(); - data will be cleared automatically by shared_ptr
    }
    stats.timeIntervals_.snapshot(std::string("End iteration for pattern: ") + patternToStr(prefixPattern));
    prefixPattern.pop_back();
}

////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadPool::worker(){
    while(true){
        ThreadFunDef fun;
        semaphoreJobs_.acquire();

        if(endAll_){
            return;
        }

        {
            std::lock_guard<std::mutex> guard(lock_);

            if(!jobs_.empty()){
                ++working_;
                #ifdef DEBUG
                std::cout << "Getting new job. Avaliable jobs: " << jobs_.size() << std::endl;
                #endif
                fun = std::move(jobs_.front().second);
                std::pop_heap(jobs_.begin(), jobs_.end(), CmpQueue());
                jobs_.pop_back();
            }
        }

        fun(); 

        {
            std::lock_guard<std::mutex> guard(lock_);
            // no more jobs from threads (do not include main thread)
            // and this thread is only one still working
            if(end_ && jobs_.empty() && working_ == 1){
                semaphoreWorkingThreads_.release();
                return;
            }
            else{
                --working_;
            }
        }

    }
}


ThreadPool::~ThreadPool(){
    endThreadsWait();
}

ThreadPool::ThreadPool(unsigned int numThreads)
:numThreads_(numThreads), pool_(), jobs_(), working_(0), end_(false), endAll_(false), lock_(), semaphoreJobs_(0), semaphoreWorkingThreads_(0)
{
    if(numThreads > std::thread::hardware_concurrency()){
        throw std::logic_error(std::string("Hardware number of threads do not meet expectation. System: ") + std::to_string(std::thread::hardware_concurrency()) + " User: " + std::to_string(numThreads));
    }

    pool_.reserve(numThreads);
    for (unsigned int i = 0; i < numThreads; i++)
    {
        pool_.push_back(std::thread(&ThreadPool::worker, this));
    }
}

void ThreadPool::endThreadsWait(){

    {
        std::lock_guard<std::mutex> guard(lock_);
        if(end_) return;

        end_ = true;
    }

    semaphoreWorkingThreads_.acquire();
    endAll_ = true;

    for(unsigned int i = 0; i < numThreads_; ++i){
        semaphoreJobs_.release();
    }

    for(auto& it : pool_){
        it.join();
    }
}

void ThreadPool::addJob(std::function<void()> fun, Priority priority){
    std::lock_guard<std::mutex> guard(lock_);
    jobs_.push_back(std::make_pair(priority, fun));
    std::push_heap(jobs_.begin(), jobs_.end(), CmpQueue());
    semaphoreJobs_.release();
}

int ThreadPool::threadOccupancy() const{
    std::lock_guard<std::mutex> guard(lock_);
    return semaphoreJobs_.getValue();
}

///////////////////////////////////////////////////////////////////////////

Semaphore::Semaphore(int value)
:lock_(), condition_(), counter_(value)
{}

int Semaphore::getValue(){
    std::lock_guard<std::mutex> guard(lock_);
    return counter_;
}

void Semaphore::release() {
    std::lock_guard<decltype(lock_)> guard(lock_);
    ++counter_;
    condition_.notify_one();
}

void Semaphore::acquire() {
    std::unique_lock<decltype(lock_)> guard(lock_);
    while(counter_ <= 0) // Handle spurious wake-ups.
        condition_.wait(guard);
    --counter_;
}

void Semaphore::decrement(){
    std::unique_lock<decltype(lock_)> guard(lock_);
    --counter_;
}

bool Semaphore::try_acquire() {
    std::lock_guard<decltype(lock_)> guard(lock_);
    if(counter_) {
        --counter_;
        return true;
    }
    return false;
}

bool ThreadPool::CmpQueue::operator()(const FunWithRank& a1, const FunWithRank& a2) const{
    return a1.first < a2.first;
}

std::string patternToStr(const Pattern& pattern){
    std::string str;
    for(const auto i : pattern){
        str += std::to_string(i) + " ";
    }
    return str;
}

}