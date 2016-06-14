#include "natclass/dataset.h"
#include "natclass/problem_builder.h"

DataSet::DataSet (std::vector<std::vector<std::string>> data, ProblemBuilder::Mode mode) {
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

void DataSet::initEncodedData() {
    encoded_data = new int * [n_rows];
    for (size_t i = 0; i < n_rows; ++i) {
        encoded_data[i] = new int [n_cols];
    }
}

void DataSet::destroyEncodedData() {
    for (size_t i = 0; i < n_rows; ++i) {
        delete [] encoded_data[i];
    }
    delete [] encoded_data;
}

void DataSet::encodeData(std::vector<std::vector<std::string>> data) {
    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j) {
            encoded_data[i][j] = problem.encode(j, data[i][j]);
        }
    }
}

size_t DataSet::nRows() const {
    return n_rows;
}

size_t DataSet::nCols() const {
    return n_cols;
}

int const ** DataSet::getEncodedData() const {
    return const_cast<const int**>(encoded_data);
}

DataSet::~DataSet() {
    destroyEncodedData();
}
