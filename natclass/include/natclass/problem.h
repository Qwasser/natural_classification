#ifndef PROBLEM_H
#define PROBLEM_H

#include <map>
#include <vector>

#include "utils.h"

class Problem {
    /*
     * Class represents metainformation for machine learning problem with categorical data
     * It conatins features, values for each feature and their integer encodings.
     */

public:
    // Construction
    Problem (std::vector<std::map<std::string, int> value_mappings);
    Problem (std::string json);

    // Serrialization
    std::string toJSON() const;

    // Checs and mappings
    int getFeatureCount() const;
    int encode(size_t feature_id, std::string value) const;
    std::string decode(size_t feature_id, std::string code) const;
    bool containsValue(size_t feature_id, std::string value) const;

    // Iteration
    typedef MapValueIterator<std::map<int, std::string>::const_iterator> code_iterator;
    code_value_iterator getBeginIter(size_t feature_id) const;
    code_value_iterator getEndIter(size_t feature_id) const;

private:
    std::vector<std::map<std::string, int>> value_mappings;
    std::vector<std::map<int, std::string>> code_mappings;
};

#endif
