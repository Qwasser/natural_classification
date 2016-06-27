#include "gtest/gtest.h"

#include <natclass/rule.h>
#include <natclass/problem_builder.h>

#include "test_data.h"

TEST(rule_tests, constructor_test) {
    Predicate conclusion(0, 0, true);

    std::vector<Predicate> premise;
    premise.push_back(Predicate(0, 0, false));
    premise.push_back(Predicate(0, 1,  true));

    Rule r(premise, conclusion);

    EXPECT_TRUE(r.getPremise() == premise);
    EXPECT_TRUE(r.getConslusion() == conclusion);

    EXPECT_TRUE(r.getFisher() == 0);
    EXPECT_TRUE(r.getConditionalProbability() == 0);
    EXPECT_TRUE(r.getYule() == 0);
}

Rule makeTestRule() {
    Predicate conclusion(0, 0, true);

    std::vector<Predicate> premise;
    premise.push_back(Predicate(0, 0, false));
    premise.push_back(Predicate(0, 1,  true));

    Rule r(premise, conclusion);

    return r;
}

TEST(rule_tests, criterion_setters_test) {
    Rule r = makeTestRule();

    r.setConditionalProbability(1.5);
    r.setFisher(1.5);
    r.setYule(1.5);

    EXPECT_TRUE(r.getFisher() == 1.5);
    EXPECT_TRUE(r.getConditionalProbability() == 1.5);
    EXPECT_TRUE(r.getYule() == 1.5);
}

TEST(rule_tests, eq_test) {
    Rule r1 = makeTestRule();
    Rule r2 = makeTestRule();

    EXPECT_TRUE(r1 == r2);

    Predicate conclusion(0, 2, true);

    std::vector<Predicate> premise;
    premise.push_back(Predicate(0, 0, false));
    premise.push_back(Predicate(0, 1,  true));

    Rule r3(premise, conclusion);

    EXPECT_TRUE(r1 != r3);
}

TEST(rule_tests, json_test) {
    Rule r1 = makeTestRule();
    Rule r2(r1.toJSON());

    EXPECT_TRUE(r1 == r2);

    EXPECT_TRUE(r1.getFisher() == r2.getFisher());
    EXPECT_TRUE(r1.getConditionalProbability() == r2.getConditionalProbability());
    EXPECT_TRUE(r1.getYule() == r2.getYule());
}

TEST(rule_tests, to_string_test) {
    ProblemBuilder pb;
    pb.fromData(SimpleTestData().getData());

    Problem p = pb.getResult();

    Rule r1 = makeTestRule();
    std::cout << r1.toString(p) << std::endl;
}

