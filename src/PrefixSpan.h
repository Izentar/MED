#ifndef PREFIXSPAN_H
#define PREFIXSPAN_H

#include <string>
#include <vector>
#include <iostream>
#include <set>

namespace PrefixSpan{

typedef std::vector<unsigned int> Pattern;
typedef std::pair<unsigned int, std::vector<unsigned int>> Transaction;

class DataIterator;

struct Data{
    std::vector<Transaction> data_;

    Data() = default;

    bool load(bool findMaxItem = true);
};

class DataProjection{
    std::vector<const Transaction*> transactionsRef_;
    std::vector<unsigned int> startIndex_;

public:
    DataProjection() = default;
    DataProjection(const Data& data);

    const Transaction& getTransaction(unsigned int index) const;
    const unsigned int getIndeces(unsigned int index) const;
    void pushTransaction(const Transaction& transaction);
    void pushIndeces(const unsigned int index);
    unsigned int size() const;
    void clear();
};

class PrefixSpan{
    unsigned int minSupport_;
    unsigned int maxPatternSize_;
    std::vector<Pattern> patterns_;
    //Pattern searchedPattern_;

    void saveInfo(const DataProjection& data, const Pattern& prefixPattern, bool verbose);

public:
    void prefixProject(const DataProjection& database, Pattern prefixPattern, bool verbose = true);
};

}

#endif