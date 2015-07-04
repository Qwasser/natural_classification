#include <string>
#include <vector>

#include <cstddef>
#include <cassert>

#include "SEQStorage.h"
#include "Sequence.h"

#include "natural_classifier.h"

void ScidiWrappper::setData(const std::vector<std::vector<std::string> > & data) {

    Sequence * storage = new Sequence [data.size()];
    for (size_t i = 0; i < data.size(); ++i) {
        Sequence * new_sequence[i] = new Sequence(data[i]);
    }

    this->data = SEQStorage(storage, data.size());
    delete [] storage;
}
