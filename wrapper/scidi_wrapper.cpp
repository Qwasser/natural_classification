#include <string>
#include <vector>
#include <iostream>

#include <cstddef>
#include <cassert>

#include "SEQStorage.h"
#include "Sequence.h"
#include "NaiveGenerator.h"
#include "Tunnels.h"
#include "Thread.h"
#include "Callback.h"

#include "scidi_wrapper.h"

void ScidiWrapper::setData(const std::vector<std::vector<std::string> > & input_data) {
    Sequence * storage = new Sequence [input_data.size()];
    for (size_t i = 0; i < input_data.size(); ++i) {
        storage[i] = Sequence();
        for (size_t j = 0; j < input_data[i].size(); ++j) {
            storage[i].Append(input_data[i][j].c_str());
        }
    }

    data = new SEQStorage(storage, input_data.size());
    delete [] storage;
}

std::vector<std::vector<std::string> > ScidiWrapper::getData() {
    std::vector<std::vector<std::string> > result(data->getLength());
    for (int i = 0; i < data->getLength(); ++i) {
        result[i] = std::vector<std::string>(data->getWidth());
        for (int j = 0; j < data->getWidth(); ++j) {
            int elem = 0;
            data->getElem_c(i, j, elem);
            result[i][j] = std::string(data->Decode(elem));
        }
    }
    return result;
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

    this->rule_storage = new RulesStorage(data->getWidth(), data->getCodesCount());
    current_rule_settings.RulesContainer = this->rule_storage;

    NaiveRuleGenerator rule_generator = NaiveRuleGenerator();
    rule_generator.SetInput(data);

    Thread * dummy_thread = new Thread();
    ThreadCommand * dummy_command = new ThreadCommand(dummy_thread);
    rule_generator.setCommand(dummy_command);

    sdEvent * dummy_event = new sdEvent();
    rule_generator.setCallback(dummy_event);
    rule_generator.GenRules(current_rule_settings);

    delete dummy_thread;
    delete dummy_command;
    delete dummy_event;
}

std::vector<std::string> ScidiWrapper::getRules(){
    std::vector<std::string> rule_strings;
    for (ruleID iter = rule_storage->begin(); iter != rule_storage->end(); ++iter) {
        rule_strings.push_back(iter->getChainStr());
    }
    std::cout << rule_strings.size() << std::endl;
    return rule_strings;
}
