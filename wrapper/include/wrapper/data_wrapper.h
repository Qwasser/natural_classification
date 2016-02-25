#ifndef DATA_WRAPPER_H
#define DATA_WRAPPER_H

#include <vector>
#include <memory>

#include "SEQStorage.h"

class ObjectWrapper {
public:
    ObjectWrapper(SEQSElem obj) : obj(obj) {}
    ObjectWrapper(ObjectWrapper const & other) : obj(other.obj) {}
private:
    SEQSElem obj;
};

class DataWrapper {
public:
    DataWrapper (const std::vector<std::vector<std::string>> & data);

    std::vector<std::vector<std::string>> asStringMatrix();
    std::vector<std::string> getObjectAsStrinVector(size_t index);

    ObjectWrapper getObjectByIndex(size_t index);

    size_t getCodesCount();
    size_t size();
    size_t getWidth();

    SEQStorage * getStoragePointer();
private:
    std::shared_ptr<SEQStorage> storage;
};
#endif
