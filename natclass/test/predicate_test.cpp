#include <iostream>

#include "gtest/gtest.h"

#include "natclass/predicate.h"
#include "natclass/problem_builder.h"


TEST(predicate_tests, constructor_test) {
    int test_val = 0;
    size_t test_feature = 1;
    bool test_sign = false;

    Predicate pred(test_val, test_feature, test_sign);

    EXPECT_EQ(test_val, pred.getValue());
    EXPECT_EQ(test_feature, pred.getFeatureID());
    EXPECT_EQ(test_sign, pred.getSign());
}


TEST(predicate_tests, json_test) {
    int test_val = 0;
    size_t test_feature = 1;
    bool test_sign = false;

    Predicate pred(test_val, test_feature, test_sign);

    Predicate pred2(pred.toJSON());

    EXPECT_EQ(test_val, pred2.getValue());
    EXPECT_EQ(test_feature, pred2.getFeatureID());
    EXPECT_EQ(test_sign, pred2.getSign());
}

void stringSerrializationTest(bool sign) {

    std::string test_val = "not";
    size_t test_feature = 0;

    ProblemBuilder pb;
    pb.addFeature();
    pb.addValue(test_feature,
                test_val);
    Problem p = pb.getResult();

    Predicate pred(p.encode(test_feature, test_val),
                   test_feature,
                   sign);

    Predicate pred2(pred.toString(p), p);

    EXPECT_EQ(p.encode(test_feature, test_val), pred2.getValue());
    EXPECT_EQ(test_feature, pred2.getFeatureID());
    EXPECT_EQ(sign, pred2.getSign());
}


TEST(predicate_tests, string_test) {
    stringSerrializationTest(true);
    stringSerrializationTest(false);
}
