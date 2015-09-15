#include <string>
#include <vector>
#include <iostream>
#include <numeric>

#include <cstddef>
#include <cassert>

#include "SEQStorage.h"
#include "Sequence.h"
#include "NaiveGenerator.h"
#include "Classificator.h"
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
    std::cout << rule_strings.size() << std::endl;
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

void ScidiWrapper::makeClasses() {
    ClassificatorSettings settings = {
        /* .IdealizType= */ __IDEALIZTYPE_OFFICIAL_,
        /* .ObjsSource= */ __IDEALSOURCE_ORIGINOBJS_
        /* .nRecClass= */
        /* .RecSourcePos= */
        /* .RecSourceNeg= */
    };

    Classificator clf;

    ideal_storage = new CIdelObject*[ data->getLength() ];

    data->ClearClasses();

    clf.Create(data, rule_storage, ideal_storage);

    clf.lObjsStorageCounter = 0;
    clf.SetSource(settings.ObjsSource);
    clf.SetType(settings.IdealizType);

    sdEvent * dummy_event = new sdEvent();
    clf.setCallback(dummy_event);


    Thread * dummy_thread = new Thread();
    ThreadCommand * dummy_command = new ThreadCommand(dummy_thread);
    clf.setCommand(dummy_command);

    clf.GenClasses();
    ideal_storage_size = clf.GetOutputSize();

    delete dummy_thread;
    delete dummy_command;
    delete dummy_event;
}

std::vector<int> ScidiWrapper::getClasses() {
    std::vector<int> classes(data->getLength());
    for (size_t i = 0; i < data->getLength(); ++i) {
        classes[i] = data->getClass(i);
    }
    return classes;
}

std::vector<std::vector<std::string> > ScidiWrapper::getIdealObjects() {
    std::vector<std::vector<std::string> > ideals;

    for (size_t class_id = 0; class_id < ideal_storage_size; ++class_id) {

        CIdelObject* o = ideal_storage[class_id];

        std::vector<std::string> current_ideal;

        for (auto l = 0; l < data->getWidth(); ++l)
        {
            std::vector<std::string> possible_values;

            for (auto c = 0; c < data->getCodesCount(); ++c) {
                if (o->isBelong(l, c))
                {
                    std::string v(data->Decode(c));
                    possible_values.push_back(v);
                }
            }

            string concatenated_possible_values =
                    accumulate( possible_values.begin(), possible_values.end(), string("|") );
            current_ideal.push_back(concatenated_possible_values);
        }

        ideals.push_back(current_ideal);
    }
    return ideals;
}

