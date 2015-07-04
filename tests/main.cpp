#include <cassert>
#include <vector>
#include <string>

#include "Sequence.h"

#include <wrapper/natural_classifier.h>

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

void setDataTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);

}

int main() {
    scidiLinkageTest();
    return 0;
}


