#include "PrefixSpan.h"

namespace PrefixSpan{

DataProjection::DataProjection(const Data& data)
:transactionsRef_(), startIndex_()
{
    transactionsRef_.resize(data.data_.size(), nullptr);
    startIndex_.resize(data.data_.size(), 0);
    for(unsigned int itData = 0; itData != data.data_.size(); ++itData){
        transactionsRef_.push_back(&data.data_[itData]);
    }
}

const Transaction& DataProjection::getTransaction(unsigned int index) const{
    return *transactionsRef_[index];
}

const unsigned int DataProjection::DataProjection::getIndeces(unsigned int index) const{
    return startIndex_[index];
}

void DataProjection::pushTransaction(const Transaction& transaction){
    transactionsRef_.push_back(&transaction);
}

void DataProjection::pushIndeces(const unsigned int index){
    startIndex_.push_back(index);
}

unsigned int DataProjection::size() const{
    return transactionsRef_.size();
}

void DataProjection::clear(){
    transactionsRef_.clear();
    startIndex_.clear();
}

void PrefixSpan::saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose){
    patterns_.push_back(prefixPattern);

    if(verbose){
        // print prefix
        for(const auto it : prefixPattern){
            std::cout << it << " ";
        }
        
        // print all transaction in current database
        std::cout << "\n(";
        for(unsigned int i = 0; i < data.size(); ++i){
            std::cout << data.getTransaction(i).first << " ";
        }
        std::cout << ") #" << data.size() << std::endl;
    }
}

void PrefixSpan::prefixProject(const DataProjection& data, Pattern prefixPattern, bool verbose){
    std::set<unsigned int> itemCount; // bigger to prevent collisions
    DataProjection newData(data);

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
    for(unsigned int itTransaction = 0; itTransaction != dataSize; ++itTransaction){
        const auto& transaction = data.getTransaction(itTransaction).second;
        const auto transSize = transaction.size();
        for(unsigned int itItem = data.getIndeces(itTransaction); itItem != transSize; ++itItem){
            itemCount.insert(transaction[itItem]);
        }
    }

    for(auto itItemCount : itemCount){

        // go through all transactions and add those to newData, that equals to evaluated item from itemCount
        // it must be noted, that all transactions in evaluated database have the same prefixes.
        for(unsigned int itTransaction = 0; itTransaction != dataSize; ++itTransaction){
            const auto& transaction = data.getTransaction(itTransaction);

            // the next item may be not next to the previous item. For example having prefix <abc> and searching for <d>
            // will occur in <abcefghdpi>
            for(unsigned int itItem = data.getIndeces(itTransaction); itItem != transaction.second.size(); ++itItem){
                if(transaction.second[itItem] == itItemCount){
                    newData.pushTransaction(transaction);
                    newData.pushIndeces(itItem + 1); // start searching for pattern at next item for this transaction
                    break;
                }
            }
        }

        // after going through all, push eveluated item to the searched pattern, creating new prefix
        // and for all transactions with this new prefix call prefixProject
        prefixPattern.push_back(itItemCount);
        this->prefixProject(newData, prefixPattern, verbose);
        prefixPattern.pop_back();
        newData.clear();
    }
}


}