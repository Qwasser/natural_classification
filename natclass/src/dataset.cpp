#include "natclass/dataset.h"
#include "natclass/problem_builder.h"

DataSet::DataSet (std::vector<std::vector<std::string>>  & data, ProblemBuilder::Mode mode) {
    ProblemBuilder pb;
    pb.fromData(data, mode);
    problem = pb.getResult();

    n_rows = data.size();
    n_cols = data[0].size();

    initEncodedData();

    try {
        encodeData(data);
    }
    catch (std::exception & err) {
        destroyEncodedData();
        throw err;
    }
}

DataSet::DataSet (std::vector<std::vector<std::string>> & data, Problem & p) : problem(p) {

}

void DataSet::initEncodedData() {
    encoded_data = new int * [n_cols];
    for (size_t i = 0; i < n_cols; ++i) {
        encoded_data[i] = new int [n_rows];
    }
}

void DataSet::destroyEncodedData() {
    for (size_t i = 0; i < n_cols; ++i) {
        delete [] encoded_data[i];
    }
    delete [] encoded_data;
}

void DataSet::encodeData(std::vector<std::vector<std::string>> & data) {
    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j) {
            //! Encoded sata is transposed
            encoded_data[j][i] = problem.encode(j, data[i][j]);
        }
    }
}

size_t DataSet::nRows() const {
    return n_rows;
}

size_t DataSet::nCols() const {
    return n_cols;
}

int DataSet::getValue(size_t obj_num, size_t feature_num) const {
    if (obj_num >= n_rows || feature_num >= n_cols) {
        throw std::out_of_range("No such object or feature!");
    }
    //! Encoded sata is transposed
    return encoded_data[feature_num][obj_num];
}

Problem DataSet::getProblem() const {
    return problem;
}

DataSet::~DataSet() {
    destroyEncodedData();
}
