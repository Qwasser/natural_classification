#include <cassert>
#include <vector>
#include <string>
#include <iostream>

#include "Sequence.h"

#include <dep/sd/include/tst.h>

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
    test_input[0].push_back("a");

    test_input[1].push_back("a");
    test_input[1].push_back("b");
    test_input[1].push_back("c");
    test_input[1].push_back("c");
    test_input[1].push_back("a");

    test_input[2].push_back("a");
    test_input[2].push_back("b");
    test_input[2].push_back("b");
    test_input[2].push_back("d");
    test_input[2].push_back("d");

    test_input[3].push_back("a");
    test_input[3].push_back("b");
    test_input[3].push_back("b");
    test_input[3].push_back("d");
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
            assert(result_data[i][j] == test_data[i][j]);
        } 
    }

    std::cout << "Data set test passed!" << std::endl;
}

void genRulesTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);

    std::vector<std::string> rules = wrapper.getRules();
    assert(rules.size > 0);

    std::cout << "Rule generation test passed!" << std::endl;
}

void genClassesTest() {
    std::vector<std::vector<std::string> > test_data = makeTestInput();
    ScidiWrapper wrapper;
    wrapper.setData(test_data);
    wrapper.makeRules(0.25, 0.5, 0.7, 2);
    wrapper.makeClasses();
    std::vector<int> classes = wrapper.getClasses();
    for (size_t i = 0; i < classes.size(); ++i) {
        assert(classes[i] == expexted_classes[i]);
    }

    std::cout << "Class generation test passed!" << std::endl;
}

void ruleParseTest() {
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
        assert(rules[i] == rule_strings[i]);
    }

    int expected_classes [4] = {0, 0, 1, 1};

    wrapper.makeClasses();

    std::vector<int> classes = wrapper.getClasses();
    for (size_t i = 0; i < classes.size(); ++i) {
        assert(classes[i] == expexted_classes[i]);
    }

    std::cout << "Rule parsing test passed!" << std::endl;
}

/* Sd hypotesys handler that simply prints rules */
void manageReg (PHYPO hyp, PPRED const prs, DWORD hit, DWORD total)
{
    DWORD cusd = hyp->used_count;

    for (size_t i = 0; i < cusd - 1; ++i){
        std::cout << prs[i].par_idx << " : " << prs[i].scale_val << "; " ;
    }

    std::cout << std::endl;
    std::cout << "Last prob: " << hyp->last_prob << std::endl;
    std::cout << "Hit: " << hit << std::endl;
    std::cout << "Total: " << total << std::endl;
}

void generateDataForSd() {
    /* Assume simple data table
     * 0 0 1 1 2
     * 1 0 1 0 1
     * 0 0 0 0 0
     * 0 0 0 0 0
     * 1 0 1 0 1
     * 0 0 1 1 2 */

    /* Create attributes in amount of 5 */

    size_t attributes_count = 5;
    size_t factor_count = 2;
    size_t object_count = 5;

    ATTR attributes [attributes_count];

    char data [] = {0, 0, 1, 1, 2,
                    1, 0, 1, 0, 1,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    1, 0, 1, 0, 1,
                    0, 0, 1, 1, 2};

    char borders[factor_count + 1] = {0, 1, 2};

    for (size_t i = 0; i < attributes_count; ++i) {
        PATTR attr = &(attributes[i]);
        attr->name = (char*) malloc(6);
        attr->name = "test";

        attr->bordersz = sizeof(char);
        attr->objoffset = i * sizeof(char);

        attr->atype = 0;
        attr->borders = borders;
        attr->bnum = factor_count;
        attr->is_addrbased = 0;

        attr->cmpf = &cmp_char;

         *((DWORD*) attr->free_to_use) = 0xC0DE;
    }

    PSDEngine engine = SDInitEngine(attributes, attributes_count);
    SDInitRawObjects(engine, data, attributes_count * sizeof(char), object_count);

    PPRED concls = (PPRED) malloc(attributes_count * (factor_count + 1) * sizeof(PRED));

    for (size_t i = 0; i < attributes_count; ++i) {
        for (size_t j = 0; j <= factor_count; ++j) {
            concls[j + i * (factor_count + 1)].scale_val = j;
            concls[j + i * (factor_count + 1)].par_idx = i;
        }
    }

    unsigned int hypcnt = 0;

    PHYPO hypos = SDPenetratedYuleOneD(engine,
                                     concls,
                                     attributes_count * (factor_count + 1),
                                     3,
                                     0.6,
                                     &hypcnt,
                                     2,
                                     0.7);

    std::cout << "hyppocount " << hypcnt << std::endl;

    for (size_t i = 0; i < hypcnt; ++i) {
        SDBust(engine, hypos + i, &manageReg);
    }

}

int main() {
    scidiLinkageTest();
    ruleParseTest();

    setDataTest();
    genRulesTest();
    genClassesTest();
    generateDataForSd();
    return 0;
}


