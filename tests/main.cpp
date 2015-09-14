#include <cassert>
#include <vector>
#include <string>
#include <iostream>

#include "Sequence.h"

#include <wrapper/scidi_wrapper.h>

std::vector<std::vector<std::string> > makeTestInput() {
    std::vector<std::vector<std::string> > test_input;
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());

    test_input[0].push_back("a");
    test_input[0].push_back("b");
    test_input[0].push_back("c");
    test_input[0].push_back("c");

    test_input[1].push_back("a");
    test_input[1].push_back("b");
    test_input[1].push_back("c");
    test_input[1].push_back("c");

    test_input[2].push_back("a");
    test_input[2].push_back("b");
    test_input[2].push_back("b");
    test_input[2].push_back("d");

    test_input[3].push_back("a");
    test_input[3].push_back("b");
    test_input[3].push_back("b");
    test_input[3].push_back("d");

    return test_input;
}


void scidiLinkageTest() {
    Sequence test_sequence("test_sequence");
}

void setDataTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);

    std::vector<std::vector<std::string> > result_data = wrapper.getData();
    for (size_t i = 0; i < result_data.size(); ++i) {
        for (size_t j = 0; j < result_data[i].size(); ++j) {
            std::cout << result_data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void genRulesTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);
    std::cout << "Rules" << std::endl;
    std::vector<std::string> rules = wrapper.getRules();
    for (auto rule : rules) {
        std::cout << rule << std::endl;
    }
}

void genClassesTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);
    wrapper.makeClasses();
    std::vector<int> classes = wrapper.getClasses();
    for (int c : classes) {
        std::cout << c << std::endl;
    }
}

int main() {
    scidiLinkageTest();
    setDataTest();
//    genRulesTest();
//    genClassesTest();

    std::string r_string = "2=a & 3= not c => 1=a";

    std::cout << r_string << std::endl;
    Rule r;
    return 0;
}


