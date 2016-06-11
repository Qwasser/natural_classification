#include "gtest/gtest.h"

#include <natclass/problem.h>

std::vector<std::map<std::string, int>> genterateTestMapping() {
    std::vector<std::map<std::string, int>> res;

    std::map<std::string, int> feature_1;
    feature_1["a"] = 0;
    feature_1["b"] = 1;

    std::map<std::string, int> feature_2;
    feature_2["c"] = 0;
    feature_2["d"] = 1;

    res.push_back(feature_1);
    res.push_back(feature_2);

    return res;
}


TEST(problem_test, construction_test) {
    Problem p(genterateTestMapping());
}
