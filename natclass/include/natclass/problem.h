#ifndef PROBLEM_H
#define PROBLEM_H

#include <map>
#include <vector>

class Problem {
public:
    Problem (std::vector<std::map<std::string, int> value_mappings);
    Problem (std::string json);

    std::string toJSON() const;

    int getFeatureCount() const;

    int encode(size_t feature_id, std::string value) const;
    std::string decode(size_t feature_id, std::string code) const;

    bool containsValue(size_t feature_id, std::string value) const;

    // iterators through codes ?
    // iterators through predicates ?

private:
    std::vector<std::map<std::string, int>> value_mappings;
    std::vector<std::map<int, std::string>> code_mappings;
};

#endif
