#include <stdexcept>
#include "natclass/problem.h"

Problem::Problem (std::vector<std::map<std::string, int>> feature_value_mappings) : value_mappings(feature_value_mappings) {
    for (auto mapping : value_mappings) {
        code_mappings.push_back(std::map<int, std::string>());

        for (auto key_val : mapping) {
            std::map<int, std::string> & current_map = code_mappings[code_mappings.size() - 1];

            if (current_map.find(key_val.second) != current_map.end()) {
                throw std::logic_error("Two values of one feature has same encoding!");
            }

            current_map[key_val.second] = key_val.first;
        }
    }
}
