#include <string>
#include <vector>

#include "SEQStorage.h"

class ScidiWrappper {
public:
    void setData(const std::vector<std::vector<std::string> > & data);
    std::vector<std::vector<std::string> > getData();
    std::vector<std::vector<int> > getEncodedData();

private:
    SEQStorage data;
};
