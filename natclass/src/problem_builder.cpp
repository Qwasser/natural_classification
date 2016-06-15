#include <set>
#include <stdexcept>

#include "natclass/problem_builder.h"

size_t ProblemBuilder::getFeatureCount() {
    return value_mappings.size();
}

Problem ProblemBuilder::getResult() {
    return Problem(value_mappings);
}

void ProblemBuilder::addFeature() {
    value_mappings.push_back(std::map<std::string, int> ());
    value_counters.push_back(0);
}

void ProblemBuilder::addValue(size_t feature_id, std::string value) {
    if (feature_id > getFeatureCount()) {
        throw std::out_of_range("No such feature!");
    }

    if (value_mappings[feature_id].find(value) == value_mappings[feature_id].end()) {
        value_mappings[feature_id][value] = value_counters[feature_id];
        value_counters[feature_id]++;
    }
}

void ProblemBuilder::addValues(size_t feature_id, std::vector<std::string> & values) {
    for (auto val : values) {
        addValue(feature_id, val);
    }
}

void ProblemBuilder::addFeatureWithValues(std::vector<std::string> & values) {
    addFeature();
    addValues(value_mappings.size() - 1, values);
}

void ProblemBuilder::removeFeature(size_t feature_id) {
    if (feature_id > getFeatureCount()) {
        throw std::out_of_range("No such feature!");
    }

    value_mappings.erase(value_mappings.begin() + feature_id);
    value_counters.erase(value_counters.begin() + feature_id);
}

void ProblemBuilder::addValueAll(std::string value) {
    for (size_t i = 0; i < getFeatureCount(); ++i) {
        addValue(i, value);
    }
}

void ProblemBuilder::fromData(std::vector<std::vector<std::string>> & data, Mode mode) {
    size_t width = data[0].size();
    for (auto vec : data) {
        if (vec.size() != width) {
            throw std::logic_error("Dataset is not rectangular!");
        }
    }

    for (size_t i = 0; i < width; ++i) {
        addFeature();
    }

    for (auto vec : data) {
       for (size_t i = 0; i < width; ++i) {
           switch(mode) {

           case SEPARATE_VALUES:
               addValue(i, vec[i]);
               break;

           case COMMON_VALUES:
               if (value_mappings[0].find(vec[i]) == value_mappings[0].end()) {
                   addValueAll(vec[i]);
               }
               break;
           }
       }
    }
}

