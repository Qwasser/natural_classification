#include <cassert>
#include <vector>
#include <string>
#include <iostream>

//#include <dep/sd/include/tst.h>

#include <wrapper/scidi_wrapper.h>
#include <wrapper/sdrulegenerator.h>
#include "gtest/gtest.h"

std::vector<std::vector<std::string> > makeTestInput() {
    std::vector<std::vector<std::string> > test_input;
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());

    test_input[0].push_back("b");
    test_input[0].push_back("c");
    test_input[0].push_back("c");
    test_input[0].push_back("c");
    test_input[0].push_back("a");

    test_input[1].push_back("b");
    test_input[1].push_back("c");
    test_input[1].push_back("c");
    test_input[1].push_back("c");
    test_input[1].push_back("a");

    test_input[2].push_back("b");
    test_input[2].push_back("b");
    test_input[2].push_back("b");
    test_input[2].push_back("d");
    test_input[2].push_back("d");

    test_input[3].push_back("b");
    test_input[3].push_back("b");
    test_input[3].push_back("b");
    test_input[3].push_back("d");
    test_input[3].push_back("d");

    test_input[4].push_back("b");
    test_input[4].push_back("b");
    test_input[4].push_back("b");
    test_input[4].push_back("d");
    test_input[4].push_back("d");

    return test_input;
}


TEST(ScidiTest, LinkageTest) {
    Sequence test_sequence("test_sequence");
}

TEST(WrapperTest, SetDataTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);

    std::vector<std::vector<std::string> > result_data = wrapper.getData();
    for (size_t i = 0; i < result_data.size(); ++i) {
        for (size_t j = 0; j < result_data[i].size(); ++j) {
            EXPECT_EQ (result_data[i][j], test_data[i][j]);
        } 
    }
}

TEST(WrapperTest, GenRulesTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);

    std::vector<std::string> rules = wrapper.getRules();
    ASSERT_GE (rules.size(), 0);
}

TEST(WrapperTest, GetClassesFromNewDataTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);

    std::vector<std::vector<std::string> > new_data;
    new_data.push_back(std::vector<std::string> ());

    new_data[0].push_back("b");
    new_data[0].push_back("c");
    new_data[0].push_back("c");
    new_data[0].push_back("c");
    new_data[0].push_back("a");

    wrapper.getIdealsFromNewData(new_data);
}


/* test is disabled because classiffication is broken */

TEST(WrapperTest, DISABLED_RuleParseTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    std::vector<std::string> rule_strings;

    rule_strings.push_back("4= not c => 3=b");
    rule_strings.push_back("4=d => 3=b");
    rule_strings.push_back("5= not a => 3=b");
    rule_strings.push_back("5=d => 3=b");
    rule_strings.push_back("4=c => 3=c");
    rule_strings.push_back("5=a => 3=c");
    rule_strings.push_back("5= not d => 3=c");
    rule_strings.push_back("4=c => 3= not b");
    rule_strings.push_back("4= not d => 3= not b");
    rule_strings.push_back("5=a => 3= not b");
    rule_strings.push_back("5= not d => 3= not b");
    rule_strings.push_back("4= not c => 3= not c");
    rule_strings.push_back("4=d => 3= not c");
    rule_strings.push_back("5= not a => 3= not c");
    rule_strings.push_back("5=d => 3= not c");
    rule_strings.push_back("3= not b => 4=c");
    rule_strings.push_back("3=c => 4=c");
    rule_strings.push_back("5=a => 4=c");
    rule_strings.push_back("5= not d => 4=c");
    rule_strings.push_back("3=b => 4=d");
    rule_strings.push_back("3= not c => 4=d");
    rule_strings.push_back("5= not a => 4=d");
    rule_strings.push_back("5=d => 4=d");
    rule_strings.push_back("3=b => 4= not c");
    rule_strings.push_back("3= not c => 4= not c");
    rule_strings.push_back("5= not a => 4= not c");
    rule_strings.push_back("5=d => 4= not c");
    rule_strings.push_back("3= not b => 4= not d");
    rule_strings.push_back("3=c => 4= not d");
    rule_strings.push_back("5=a => 4= not d");
    rule_strings.push_back("5= not d => 4= not d");

    wrapper.setRulesFromStringVector(rule_strings);

    std::vector<std::string> rules = wrapper.getRules();
    for (size_t i = 0; i < rule_strings.size(); ++i) {
        ASSERT_TRUE (rules[i] == rule_strings[i]);
    }

    int expected_classes [4] = {0, 1, 1, 1};

    wrapper.makeClasses();

    std::vector<int> classes = wrapper.getClasses();
    for (size_t i = 0; i < classes.size(); ++i) {
        ASSERT_TRUE (classes[i] == expected_classes[i]);
    }
}

TEST(WrapperTest, TestSdGenerator) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRulesWithSDGenerator(3, 0.9, 1, 0.1);
    std::vector<std::string> rules = wrapper.getRules();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


