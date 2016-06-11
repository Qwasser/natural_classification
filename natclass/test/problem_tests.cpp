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
    EXPECT_EQ(p.getFeatureCount(), 2);
}


TEST(problem_test, encode_test) {
    Problem p(genterateTestMapping());

    EXPECT_EQ(p.encode(0, "a"), 0);
    EXPECT_EQ(p.encode(0, "b"), 1);
    EXPECT_EQ(p.encode(1, "c"), 0);
    EXPECT_EQ(p.encode(1, "d"), 1);

    try {
       p.encode(2, "a");
       FAIL() << "Expected std::out_of_range";
    }
    catch(std::out_of_range const & err) {
       EXPECT_EQ(err.what(),std::string("No such feature!"));
    }
    catch(...) {
       FAIL() << "Expected std::out_of_range";
    }

    try {
       p.encode(0, "aaaaa");
       FAIL() << "Expected std::out_of_range";
    }
    catch(std::out_of_range const & err) {
       EXPECT_EQ(err.what(),std::string("Feature does not contain the value!"));
    }
    catch(...) {
       FAIL() << "Expected std::out_of_range";
    }
}

