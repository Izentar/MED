#ifndef PREFIXSPAN_H
#define PREFIXSPAN_H

#include <fstream>
#include <vector>
#include <iostream>
#include <set>
#include <sstream>

#include "utils.h"

namespace PrefixSpan{

typedef unsigned short int IndexType;
typedef unsigned int TransactionIndexType;
typedef std::vector<IndexType> Pattern;
typedef std::vector<IndexType> TransactionData;
typedef std::pair<TransactionIndexType, TransactionData> Transaction;

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

    const Transaction& getTransaction(const TransactionIndexType index) const;
    const IndexType getIndeces(const IndexType index) const;
    void pushTransaction(const Transaction& transaction);
    void pushIndeces(const IndexType index);
    TransactionIndexType size() const;
    void clear();
};

class PrefixSpan{
    TransactionIndexType minSupport_;
    IndexType maxPatternSize_;
    std::fstream& outFile_;

    void saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose);

public:
    PrefixSpan(const TransactionIndexType minSupport, const IndexType maxPatternSize, std::fstream& outFile);
    void prefixProject(const DataProjection& database, bool verbose, Pattern prefixPattern = Pattern());
};

}

#endif