#include <string>
#include <vector>
#include <iostream>

#include <cstddef>
#include <cassert>

#include "SEQStorage.h"
#include "Sequence.h"

#include "scidi_wrapper.h"

void ScidiWrapper::setData(const std::vector<std::vector<std::string> > & input_data) {
    Sequence * storage = new Sequence [input_data.size()];
    for (size_t i = 0; i < input_data.size(); ++i) {
        storage[i] = Sequence();
        for (size_t j = 0; j < input_data[i].size(); ++j) {
            storage[i].Append(input_data[i][j].c_str());
        }
    }

    data = new SEQStorage(storage, input_data.size());
    delete [] storage;
}

std::vector<std::vector<std::string> > ScidiWrapper::getData() {
    std::vector<std::vector<std::string> > result(data->getLength());
    for (int i = 0; i < data->getLength(); ++i) {
        result[i] = std::vector<std::string>(data->getWidth());
        for (int j = 0; j < data->getWidth(); ++j) {
            int elem = 0;
            data->getElem_c(i, j, elem);
            result[i][j] = std::string(data->Decode(elem));
        }
    }
    return result;
}
