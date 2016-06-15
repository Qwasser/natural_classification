#ifndef DATASET_H
#define DATASET_H

#include <istream>
#include "problem.h"
#include "problem_builder.h"

/*!
 * \brief The DataSet class associates encoded data with problem meta information
 */
class DataSet {
public:
    //! Deserrialization from json
    DataSet (std::string & json_str);

    //! Deserrialization from json stream (can pass file here)
    DataSet (std::istream & json_input);

    //! Constructs from raw data. Additionally creates problem metadata.
    DataSet (std::vector<std::vector<std::string>> & data, ProblemBuilder::Mode mode = ProblemBuilder::Mode::SEPARATE_VALUES);

    /*!
     * Constructing from data with given problem. If data not correspond the problem throws std::out_of_range
     */
    DataSet (std::vector<std::vector<std::string>> & data, Problem & p);

    //! Serrialization from json
    std::string toJson() const;

    //! Object count
    size_t nRows() const;

    //! Feature count
    size_t nCols() const;

    //! Returns code for value for particular feature and object
    int getValue(size_t obj_num, size_t feature_num) const;

    //! Get dataset meta
    Problem getProblem() const;

    //! Destructor
    ~DataSet();
private:
    Problem problem;

    /*!
     * \brief encoded_data
     * Each column is an object, each row is a value.
     * Should improve performance via caching.
     */
    int ** encoded_data;

    size_t n_rows;
    size_t n_cols;

    void initEncodedData();
    void destroyEncodedData();

    void encodeData(std::vector<std::vector<std::string>> & data);
};

#endif
