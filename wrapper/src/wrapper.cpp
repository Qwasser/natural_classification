#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <numeric>
#include <iterator>

#include <cstddef>
#include <cassert>

#include "SEQStorage.h"
#include "Sequence.h"
#include "NaiveGenerator.h"
#include "Tunnels.h"
#include "Thread.h"
#include "Callback.h"

#include "wrapper/wrapper.h"

SEQStorage * ScidiWrapper::makeDataStorage(const std::vector<std::vector<std::string> > & input_data) {
    Sequence * storage = new Sequence [input_data.size()];
    for (size_t i = 0; i < input_data.size(); ++i) {
        storage[i] = Sequence();
        for (size_t j = 0; j < input_data[i].size(); ++j) {
            storage[i].Append(input_data[i][j].c_str());
        }
    }

    SEQStorage * res_storage = new SEQStorage(storage, input_data.size());
    delete [] storage;
    return res_storage;
}

void ScidiWrapper::setData(const std::vector<std::vector<std::string> > & input_data) {
    data = makeDataStorage(input_data);
}

std::vector<std::vector<std::string> > ScidiWrapper::getData() {
    std::vector<std::vector<std::string> > result(data->getLength());
    for (int i = 0; i < data->getLength(); ++i) {
        result[i] = std::vector<std::string>(data->getWidth());
        for (int j = 0; j < data->getWidth(); ++j) {
            int elem = 0;
            data->getElem_c(i, j, elem);
            const char * decoded_elem = data->Decode(elem);
            result[i][j] = std::string(decoded_elem);
        }
    }
    return result;
}

void ScidiWrapper::intitRuleStorage() {
    if (rule_storage) {
        delete rule_storage;
    }
    this->rule_storage = new RulesStorage(data->getWidth(), data->getCodesCount());
}

void ScidiWrapper::makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth) {
    current_rule_settings.SearchDepth = (int)depth;
    current_rule_settings.FisherSumBound = conf_int_value;
    current_rule_settings.MinProb = min_cp;
    current_rule_settings.YuleQUpBound = yule_value;
    current_rule_settings.StartPos = 0;
    current_rule_settings.FinishPos = data->getWidth() - 1;

    current_rule_settings.DeclineDecision = true;
    current_rule_settings.DeclinePremise = true;

    intitRuleStorage();
    current_rule_settings.RulesContainer = this->rule_storage;

    NaiveRuleGenerator rule_generator = NaiveRuleGenerator();
    rule_generator.SetInput(data);
    rule_generator.SetSettings(current_rule_settings);

    Thread * dummy_thread = new Thread();
    ThreadCommand * dummy_command = new ThreadCommand(dummy_thread);
    rule_generator.setCommand(dummy_command);

    sdEvent * dummy_event = new sdEvent();
    rule_generator.setCallback(dummy_event);

    dummy_thread->start();
    rule_generator.GenRules();

    delete dummy_thread;
    delete dummy_command;
    delete dummy_event;
}

std::vector<std::string> ScidiWrapper::getRules(){
    std::vector<std::string> rule_strings;
    for (ruleID iter = rule_storage->begin(); iter != rule_storage->end(); ++iter) {
        rule_strings.push_back(iter->getChainStr());
    }
    return rule_strings;
}

std::vector<double> ScidiWrapper::getCriteriaValues(bool need_yule) {
    std::vector<double> criteria_values(rule_storage->getSize());
    unsigned long m[2][2];

    double other_criteria;
    size_t counter = 0;
    for (ruleID iter = rule_storage->begin(); iter != rule_storage->end(); ++iter) {
        if (need_yule) {
            rule_storage->ConvertFromLinkToRule(&(*iter))->GetFisher(data, m, std::vector<int>(0), other_criteria, criteria_values[counter]);
        } else {
            rule_storage->ConvertFromLinkToRule(&(*iter))->GetFisher(data, m, std::vector<int>(0), criteria_values[counter], other_criteria);
        }
        ++counter;
    }

    return criteria_values;
}

std::vector<double> ScidiWrapper::getFisher() {
    return getCriteriaValues();
}

std::vector<double> ScidiWrapper::getYule() {
    return getCriteriaValues(true);
}

std::vector<double> ScidiWrapper::getCP() {
    std::vector<double> cp_values(rule_storage->getSize());

    size_t counter = 0;
    for (ruleID iter = rule_storage->begin(); iter != rule_storage->end(); ++iter) {
        cp_values[counter] = iter->getCP();
        ++counter;
    }

    return cp_values;
}


void ScidiWrapper::addRuleFromString(std::string rule_str) {
    Rule * rule = parseRule(rule_str);

    rule->Probability(this->data);
    rule_storage->Add(rule);
}

Rule * ScidiWrapper::parseRule(std::string rule_str) {
    RuleLink * rule_link = RuleParser(*(this->data)).parseRuleLink(rule_str);
    return rule_storage->ConvertFromLinkToRule(rule_link);
}

//void ScidiWrapper::makeRulesWithSDGenerator(unsigned int full_depth,
//                                            double fisher,
//                                            unsigned int yule_freq,
//                                            double yule_critlvl) {
//    SdRuleGenerator gen(*(data));
//    std::list<RuleLink *> rule_links = gen.generateAllRules(full_depth,
//                                                              fisher,
//                                                              yule_freq,
//                                                              yule_critlvl);
//    intitRuleStorage();
//    for (RuleLink * link_ptr : rule_links) {
//        Rule * rule = rule_storage->ConvertFromLinkToRule(link_ptr);
//        rule->Probability(data);
//        rule_storage->Add(rule);
//    }
//    rule_storage->MakePointersArray();
//}
