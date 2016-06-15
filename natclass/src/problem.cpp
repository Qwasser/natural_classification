#include <stdexcept>

#include "natclass/problem.h"

void Problem::initCodeMappings() {
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

Problem::Problem (std::vector<std::map<std::string, int>> & feature_value_mappings) : value_mappings(feature_value_mappings) {
    initCodeMappings();
}

size_t Problem::getFeatureCount() const {
    return value_mappings.size();
}

void Problem::checkRange(size_t feature_id) const {
    if (feature_id >= value_mappings.size()) {
        throw std::out_of_range("No such feature!");
    }
}

int Problem::encode(size_t feature_id, std::string value) const {
    checkRange(feature_id);

    if (value_mappings[feature_id].find(value) == value_mappings[feature_id].end()) {
        throw std::out_of_range("Feature does not contain the value!");
    }

    int code = value_mappings[feature_id].find(value)->second;
    return code;
}

std::string Problem::decode(size_t feature_id, int code) const {
    checkRange(feature_id);

    if (code_mappings[feature_id].find(code) == code_mappings[feature_id].end()) {
        throw std::out_of_range("Feature does not contain the code!");
    }

    return code_mappings[feature_id].find(code)->second;
}

bool Problem::containsValue(size_t feature_id, std::string value) const {
    checkRange(feature_id);

    if (value_mappings[feature_id].find(value) == value_mappings[feature_id].end()) {
        return false;
    }
    return true;
}

Problem::code_iterator Problem::getBeginIter(size_t feature_id) const {
    checkRange(feature_id);
    return make_map_iterator(value_mappings[feature_id].cbegin());
}

Problem::code_iterator Problem::getEndIter(size_t feature_id) const {
    checkRange(feature_id);
    return make_map_iterator(value_mappings[feature_id].cend());
}

json Problem::toJSON() const {
    json j(this->value_mappings);
    return j;
}

void Problem::fromJSON(json & j) {
    value_mappings.resize(j.size());
    for (size_t i = 0; i < j.size(); ++i) {
        for (json::iterator it = j[i].begin(); it != j[i].end(); ++it) {
          value_mappings[i][it.key()] = it.value().get<int>();
        }
    }

    initCodeMappings();
}

Problem::Problem (json & j) {
    fromJSON(j);
}

Problem::Problem (json j) {
    fromJSON(j);
}

 bool Problem::operator==(const Problem &other) const {
     return value_mappings == other.value_mappings;
 }

