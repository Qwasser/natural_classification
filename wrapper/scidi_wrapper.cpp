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
#include "MyClassificator.h"
#include "Tunnels.h"
#include "Thread.h"
#include "Callback.h"

#include "scidi_wrapper.h"
#include "sdrulegenerator.h"

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

int ScidiWrapper::makeClasses(SEQStorage * storage, CIdelObject ** id_storage) {
    ClassificatorSettings settings = {
        /* .IdealizType= */ __IDEALIZTYPE_OFFICIAL_,
        /* .ObjsSource= */ __IDEALSOURCE_ORIGINOBJS_
        /* .nRecClass= */
        /* .RecSourcePos= */
        /* .RecSourceNeg= */
    };

    MyClassificator clf;

    storage->ClearClasses();
    clf.Create(storage, rule_storage, id_storage);

    clf.lObjsStorageCounter = 0;
    clf.SetSource(settings.ObjsSource);
    clf.SetType(settings.IdealizType);

    clf.GenClasses();

    return clf.GetOutputSize();
}

void ScidiWrapper::makeClasses() {
    ideal_storage = new CIdelObject*[ data->getLength() ];
    ideal_storage_size = makeClasses(data, ideal_storage);
}

std::vector<std::vector<std::string> > ScidiWrapper::getIdealsFromNewData(const std::vector<std::vector<std::string> > & new_data) {
    SEQStorage * new_storage = makeDataStorage(new_data);

    CIdelObject** new_ideal_storage = new CIdelObject*[ new_storage->getLength() ];
    int new_ideal_storage_size = makeClasses(new_storage, new_ideal_storage);

    std::vector<std::vector<std::string> > ideals = getIdealObjects(new_storage,
                                                                    new_ideal_storage,
                                                                    new_ideal_storage_size);

    delete new_storage;
    delete new_ideal_storage;
    return ideals;
}

std::vector<int> ScidiWrapper::getClasses() {
    std::vector<int> classes(data->getLength());
    for (size_t i = 0; i < data->getLength(); ++i) {
        classes[i] = data->getClass(i);
    }
    return classes;
}

std::vector<std::vector<std::string> > ScidiWrapper::getIdealObjects(SEQStorage * storage,
                                                                     CIdelObject ** i_storage,
                                                                     int i_storage_size) {
    std::vector<std::vector<std::string> > ideals;

    for (size_t class_id = 0; class_id < i_storage_size; ++class_id) {

        CIdelObject* o = i_storage[class_id];

        std::vector<std::string> current_ideal;

        for (auto l = 0; l < storage->getWidth(); ++l)
        {
            std::vector<std::string> possible_values;

            for (auto c = 0; c < storage->getCodesCount(); ++c) {
                if (o->isBelong(l, c))
                {
                    std::string v(storage->Decode(c));
                    possible_values.push_back(v);
                }
            }

            std::stringstream s;
            const char * sep = "|";
            copy(possible_values.begin(),
                 possible_values.end(),
                 std::ostream_iterator<std::string>(s, sep));

            current_ideal.push_back(s.str());
        }

        ideals.push_back(current_ideal);
    }
    return ideals;
}

std::vector<std::vector<std::string> > ScidiWrapper::getIdealObjects() {
    return getIdealObjects(data, ideal_storage, ideal_storage_size);
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

void ScidiWrapper::makeRulesWithSDGenerator(unsigned int full_depth,
                                            double fisher,
                                            unsigned int yule_freq,
                                            double yule_critlvl) {
    SdRuleGenerator gen(*(data));
    std::list<RuleLink *> rule_links = gen.generateAllRules(full_depth,
                                                              fisher,
                                                              yule_freq,
                                                              yule_critlvl);
    intitRuleStorage();
    for (RuleLink * link_ptr : rule_links) {
        Rule * rule = rule_storage->ConvertFromLinkToRule(link_ptr);
        rule->Probability(data);
        rule_storage->Add(rule);
    }
    rule_storage->MakePointersArray();
}
