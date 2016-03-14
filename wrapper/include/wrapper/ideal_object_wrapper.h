#ifndef IDEAL_OBJECT_WRAPPER_H
#define IDEAL_OBJECT_WRAPPER_H

#include <vector>

#include <CIdelObject.h>
#include <wrapper/data_wrapper.h>

class IdealObjectWrapper {
public:
    IdealObjectWrapper (CIdelObject object) : object(object) {}

    IdealObjectWrapper (ObjectWrapper obj, size_t values_count);

    std::vector< std::vector<bool> > asBooleanMatrix(DataWrapper data);
    std::vector<std::string> idToValueVector(DataWrapper data);

    CIdelObject getObject() {
        return object;
    }

private:
    CIdelObject object;
};

#endif
