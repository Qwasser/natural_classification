#include <cassert>
#include <vector>
#include <string>
#include <iostream>

#include <wrapper/sdrulegenerator.h>
#include <wrapper/data_wrapper.h>
#include <wrapper/rules_wrapper.h>

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


TEST(RulesWrapperTests, GenRulesTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    DataWrapper data_wrapper(test_data);

    RulesWrapper rules_wrapper(0.25, 0.5, 0.7, 2, data_wrapper);

    std::vector<std::string> rules = rules_wrapper.asStringVectors();
    ASSERT_GE (rules.size(),  0);
}



TEST(DataWrapperTests, TeatSetData) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    DataWrapper wrapper(test_data);

    std::vector<std::vector<std::string> > result_data = wrapper.asStringMatrix();

    for (size_t i = 0; i < result_data.size(); ++i) {
        for (size_t j = 0; j < result_data[i].size(); ++j) {
            std::string a = result_data[i][j];
            std::string b = test_data[i][j];
            EXPECT_EQ(a, b) << "Vectors x and y differ at index ";
        }
    }

    ASSERT_EQ(wrapper.size(), result_data.size());
}

TEST(RulesWrapperTests, RuleParseTest) {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    DataWrapper data_wrapper(test_data);

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

    RulesWrapper rules_wrapper(rule_strings, data_wrapper);

    std::vector<std::string> rules = rules_wrapper.asStringVectors();
    for (size_t i = 0; i < rule_strings.size(); ++i) {
        ASSERT_TRUE (rules[i] == rule_strings[i]);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


