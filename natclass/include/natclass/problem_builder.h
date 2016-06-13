#ifndef PROBLEM_BUILDER_H
#define PROBLEM_BUILDER_H

#include "problem.h"
#include <vector>
#include <map>

/*!
@brief class for meta description of categorical datasets. Implements builder pattern.
 */
class ProblemBuilder {
 public:
    //! Constructor
    ProblemBuilder() {}

    //! Value treatment modes
    enum Mode {
        SEPARATE_VALUES, /*!< Treat values for each feature in separate */
        COMMON_VALUES    /*!< Consider all values to be common for all features */
    };

    //! Extracts features and values from raw data. Builds problem metadata.
    void fromData(std::vector<std::vector<std::string>> data, Mode mode = SEPARATE_VALUES);

    //! Adds new feature without values
    void addFeature();

    //! Adds new feature with values from vector
    void addFeatureWithValues(std::vector<std::string> values);

    //! Add value to feature by id
    void addValue(size_t feature_id, std::string value);

    //! Adds value to all features
    void addValueAll(std::string value);

    //! Add collection of values to feature by id
    void addValues(size_t feature_id, std::vector<std::string> values);

    //! Removes feature bu ID
    void removeFeature(size_t feature_id);

    //! Get problem instance from builder
    Problem getResult();


    //! Get feature count
    size_t getFeatureCount();

private:
    std::vector<size_t> value_counters;
    std::vector<std::map<std::string, int>> value_mappings;
};

#endif
