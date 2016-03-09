#include <cassert>
#include <stdexcept>

#include "wrapper/data_wrapper.h"

DataWrapper::DataWrapper (const std::vector<std::vector<std::string> > &data) {
    Sequence * temp_storage = new Sequence [data.size()];
    for (size_t i = 0; i < data.size(); ++i) {
        temp_storage[i] = Sequence();
        for (size_t j = 0; j < data[i].size(); ++j) {
            temp_storage[i].Append(data[i][j].c_str());
        }
    }

    storage.reset(new SEQStorage(temp_storage, data.size()));
    delete [] temp_storage;
}

std::vector<std::vector<std::string>> DataWrapper::asStringMatrix() {
    std::vector<std::vector<std::string> > result(storage->getLength());
    for (int i = 0; i < storage->getLength(); ++i) {
        result[i] = std::vector<std::string>(storage->getWidth());
        for (int j = 0; j < storage->getWidth(); ++j) {
            int elem = 0;
            storage->getElem_c(i, j, elem);
            const char * decoded_elem = storage->Decode(elem);
            result[i][j] = std::string(decoded_elem);
        }
    }
    return result;
}

std::vector<std::string> DataWrapper::getObjectAsStrinVector(size_t index) {
    assert(index < storage->getLength());

    std::vector<std::string> result(storage->getWidth());

    for (size_t j = 0; j < storage->getWidth(); ++j) {
        int elem = 0;
        storage->getElem_c(index, j, elem);
        const char * decoded_elem = storage->Decode(elem);
        result[j] = std::string(decoded_elem);
    }

    return result;
}

ObjectWrapper DataWrapper::getObjectByIndex (size_t index) {
    SEQSElem elem(storage->getWidth());
    storage->CreateElem(index, &elem);
    return ObjectWrapper(elem);
}

size_t DataWrapper::getCodesCount() {
    return storage->getCodesCount();
}

size_t DataWrapper::size() {
    return storage->getLength();
}

size_t DataWrapper::getWidth() {
    return storage->getWidth();
}

std::string DataWrapper::decodeValueId(size_t value_id) {
    if (value_id >= getCodesCount()) {
        throw std::out_of_range("Value id is out of range!");
    }

    return std::string(storage->Decode(value_id));
}

SEQStorage * DataWrapper::getStoragePointer() {
    return storage.get();
}
