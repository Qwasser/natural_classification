#ifndef PROBLEM_H
#define PROBLEM_H

#include <map>
#include <vector>

#include "json.hpp"
using json = nlohmann::json;

#include "utils.h"

/*!
 * Class represents metainformation for machine learning problem with categorical data
 * It conatins features, values for each feature and their integer encodings.
 */
class Problem {


public:
    //! Construction
    Problem (std::vector<std::map<std::string, int>> & feature_value_mappings);
    Problem (json & j);
    Problem (json j);

    Problem () {}

    //! Serrialization
    json toJSON() const;

    //! Checs and mappings
    size_t getFeatureCount() const;
    int encode(size_t feature_id, std::string value) const;
    std::string decode(size_t feature_id, int code) const;
    bool containsValue(size_t feature_id, std::string value) const;

    //! Iteration
    typedef MapValueIterator<std::map<std::string, int>::const_iterator> code_iterator;
    code_iterator getBeginIter(size_t feature_id) const;
    code_iterator getEndIter(size_t feature_id) const;

    //! Operators
    bool operator==(const Problem &other) const;

    bool operator!=(const Problem &other) const {
      return !(*this == other);
    }

private:
    std::vector<std::map<std::string, int>> value_mappings;
    std::vector<std::map<int, std::string>> code_mappings;

    void checkRange(size_t feature_id) const;
    void initCodeMappings();

    void fromJSON(json & j);
};

#endif
