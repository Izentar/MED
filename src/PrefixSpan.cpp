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
        data_.push_back(Transaction(id, std::move(transData)));
        
        ++id;
    }
    return true;
}

PrefixSpan::PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile)
:minSupport_(minSupport), maxPatternSize_(maxPatternSize), outFile_(outFile)
{}

DataProjection::DataProjection(const Data& data)
:transactionsRef_(), startIndex_()
{
    startIndex_.resize(data.data_.size(), 0);

    transactionsRef_.resize(data.data_.size(), nullptr);
    for(TransactionIndexType itData = 0; itData != data.data_.size(); ++itData){
        transactionsRef_.push_back(&data.data_[itData]);
    }
}

const Transaction& DataProjection::getTransaction(const TransactionIndexType index) const{
    return *transactionsRef_[index];
}

const IndexType DataProjection::DataProjection::getIndeces(const IndexType index) const{
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

void PrefixSpan::saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose){
    std::stringstream tmp;
    SystemStats stats = SystemStats::getInstance();

    stats.memoryUsage_.snapshot();

    // print prefix
    for(const auto it : prefixPattern){
        tmp << it << " ";
    }
    
    // print all transaction in current database
    tmp << "\n(";
    for(TransactionIndexType i = 0; i < data.size(); ++i){
        tmp << data.getTransaction(i).first << " ";
    }
    tmp << ") #" << data.size() << '\n';

    std::string strtmp = tmp.str();
    outFile_ << strtmp;
    std::flush(outFile_);

    if(verbose){
        std::cout << strtmp;
        std::flush(std::cout);
    }
}

void PrefixSpan::prefixProject(const DataProjection& data, bool verbose, Pattern prefixPattern){
    std::set<IndexType> itemSet;
    DataProjection newData;

    if (data.size() < this->minSupport_){
        return;
    }    

    saveInfo(data, prefixPattern, verbose);

    if(this->maxPatternSize_ != 0 && prefixPattern.size() >= this->maxPatternSize_){
        return;
    }
    
    // sort items, that are in the dataset. Those that arent in the dataset wont be included.
    // it will speed up algorithm
    const auto dataSize = data.size();
    for(TransactionIndexType itTransaction = 0; itTransaction != dataSize; ++itTransaction){
        const auto& transaction = data.getTransaction(itTransaction).second;
        const auto transSize = transaction.size();
        for(auto itItem = data.getIndeces(itTransaction); itItem != transSize; ++itItem){
            itemSet.insert(transaction[itItem]);
        }
    }

    for(auto itItemCount : itemSet){

        // go through all transactions and add those to newData, that equals to evaluated item from itemSet
        // it must be noted, that all transactions in evaluated database have the same prefixes.
        for(TransactionIndexType itTransaction = 0; itTransaction != dataSize; ++itTransaction){
            const auto& transaction = data.getTransaction(itTransaction);

            // the next item may be not next to the previous item. For example having prefix <abc> and searching for <d>
            // will occur in <abcefghdpi>
            for(auto itItem = data.getIndeces(itTransaction); itItem != transaction.second.size(); ++itItem){
                if(transaction.second[itItem] == itItemCount){
                    newData.pushTransaction(transaction);
                    newData.pushIndeces(itItem + 1); // start searching for pattern at the next item for this transaction
                    break;
                }
            }
        }

        // after going through all, push eveluated item to the searched pattern, creating new prefix
        // and for all transactions with this new prefix call prefixProject
        prefixPattern.push_back(itItemCount);
        this->prefixProject(newData, verbose, prefixPattern);
        prefixPattern.pop_back();
        newData.clear();
    }
}


}