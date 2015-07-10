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

    test_input[1].push_back("a");
    test_input[1].push_back("b");
    test_input[1].push_back("c");

    test_input[2].push_back("a");
    test_input[2].push_back("b");
    test_input[2].push_back("c");

    test_input[3].push_back("a");
    test_input[3].push_back("b");
    test_input[3].push_back("c");

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
    for (int i = 0; i < result_data.size(); ++i) {
        for (int j = 0; j < result_data[i].size(); ++j) {
            std::cout << result_data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void genRulesTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.05, 0.5, 0.7, 2);
    std::vector<std::string> rules = wrapper.getRules();
    for (auto rule : rules) {
        std::cout << rule << std::endl;
    }
}

int main() {
    scidiLinkageTest();
    setDataTest();
    genRulesTest();
    return 0;
}


