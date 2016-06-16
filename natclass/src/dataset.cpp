#include "natclass/dataset.h"
#include "natclass/problem_builder.h"

DataSet::DataSet (std::vector<std::vector<std::string>>  & data, ProblemBuilder::Mode mode) {
    ProblemBuilder pb;
    pb.fromData(data, mode);
    problem = pb.getResult();

    initData(data);
}

DataSet::DataSet (std::vector<std::vector<std::string>> & data, Problem & p) : problem(p) {
    initData(data);
}

void DataSet::initData(std::vector<std::vector<std::string>> & data) {
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

json DataSet::toJSON() const {
    json j;
    j["problem"] = problem.toJSON();

    for (size_t i = 0; i < n_rows; ++i) {
        std::vector<int> row(n_cols);
        for (size_t j = 0; j < n_cols; ++j) {
            row[j] = getValue(i, j);
        }
        j["data"][std::to_string(i)] = json(row);
    }
    return j;
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

void DataSet::fromJSON(json & j) {
    problem = Problem(json(j["problem"]));

    n_rows = j["data"].size();
    n_cols = j["data"]["0"].size();
    initEncodedData();

    int row_counter = 0;
    for (auto& element : j["data"]) {
        for (size_t col_num = 0; col_num < n_cols; ++col_num) {
            encoded_data[col_num][row_counter] = element.at(col_num);
        }
        ++row_counter;
    }
}

DataSet::DataSet (json & j) {
    fromJSON(j);
}

DataSet::DataSet (json && j) {
    fromJSON(j);
}
