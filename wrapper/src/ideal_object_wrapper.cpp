#include <stdexcept>

#include <wrapper/ideal_object_wrapper.h>

IdealObjectWrapper::IdealObjectWrapper (ObjectWrapper obj, size_t values_count) {
    SEQSElem elem = obj.getObj();
    object.Create(&elem, values_count);
}

std::vector<std::vector<bool>> IdealObjectWrapper::asBooleanMatrix(DataWrapper data) {
    size_t width = data.getWidth();
    size_t codes_count = data.getCodesCount();

    std::vector<std::vector<bool>> result_matrix(width,
                                                 std::vector<bool>(codes_count, false));

    bool ** ideal_ptr = object.getObjAsVector(width, codes_count);

    if (!ideal_ptr) {
       throw std::out_of_range("Wrong metadata!");
    }

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < codes_count; ++j) {
            result_matrix[i][j] = ideal_ptr[i][j];
        }
    }

    return result_matrix;
}

std::vector<std::string> IdealObjectWrapper::idToValueVector(DataWrapper data) {
    std::vector<std::string> value_names(data.getCodesCount());

    for (size_t value_id = 0; value_id < data.getCodesCount(); ++value_id) {
        value_names[value_id] = data.decodeValueId(value_id);
    }
    return value_names;
}
