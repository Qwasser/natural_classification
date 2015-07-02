#include <cassert>
#include <vector>
#include <string>

#include "Sequence.h"

std::vector<std::vector<std::string> > makeTestInput() {
    std::vector<std::vector<std::string> > test_input;
    test_input.push_back(std::vector<std::string> ());
    test_input.push_back(std::vector<std::string> ());

    test_input[0].push_back("a");
    test_input[0].push_back("b");
    test_input[0].push_back("c");

    test_input[1].push_back("d");
    test_input[1].push_back("e");
    test_input[1].push_back("f");
}


void scidiLinkageTest() {
    Sequence test_sequence("test_sequence");
}

int main() {
    scidiLinkageTest();
    return 0;
}


