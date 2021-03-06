#include "Rule.h"
#include "NaiveGenerator.h"
#include "Tunnels.h"
#include "Thread.h"
#include "Callback.h"

#include <wrapper/rules_wrapper.h>
#include <wrapper/ruleparser.h>
#include <wrapper/scidi_logger.h>

RulesWrapper::RulesWrapper(std::vector<std::string> text_rules, DataWrapper data) : data(data) {
    storage.reset(new RulesStorage(data.getWidth() , data.getCodesCount()));

    for (auto str : text_rules) {
        addRuleFromString(str);
    }
    storage->MakePointersArray();
}

RulesWrapper::RulesWrapper(std::vector<RuleLink *> rules, DataWrapper data) : data(data) {
    storage.reset(new RulesStorage(data.getWidth() , data.getCodesCount()));

    for (RuleLink * rule_link : rules) {
        Rule * rule = storage->ConvertFromLinkToRule(rule_link);
        rule->Probability(data.getStoragePointer());
        storage->Add(rule);
    }
    
    storage->MakePointersArray();
}


void RulesWrapper::addRuleFromString(std::string rule_str) {
    Rule * rule = parseRule(rule_str);

    rule->Probability(data.getStoragePointer());
    storage->Add(rule);
}

Rule * RulesWrapper::parseRule(std::string rule_str) {
    RuleLink * rule_link = RuleParser(*(data.getStoragePointer())).parseRuleLink(rule_str);
    return storage->ConvertFromLinkToRule(rule_link);
}

std::vector<std::string> RulesWrapper::asStringVectors() {
    std::vector<std::string> rule_strings;
    for (ruleID iter = storage->begin(); iter != storage->end(); ++iter) {
        rule_strings.push_back(iter->getChainStr());
    }
    return rule_strings;
}

RulesWrapper::RulesWrapper(double conf_int_value,
                           double yule_value,
                           double min_cp,
                           size_t depth,
                           DataWrapper data,
                           bool negate_premise,
                           bool negate_consequence) : data(data) {
    storage.reset(new RulesStorage(data.getWidth() , data.getCodesCount()));

    RuleSettings current_rule_settings;

    current_rule_settings.SearchDepth = (int)depth;
    current_rule_settings.FisherSumBound = conf_int_value;
    current_rule_settings.MinProb = min_cp;
    current_rule_settings.YuleQUpBound = yule_value;
    current_rule_settings.StartPos = 0;
    current_rule_settings.FinishPos = data.getWidth() - 1;

    current_rule_settings.DeclineDecision = negate_consequence;
    current_rule_settings.DeclinePremise = negate_premise;

    current_rule_settings.RulesContainer = storage.get();

    sdEvent* rule_generator_event = new sdEvent();

    shared_ptr<ScidiLogger> reporter = std::make_shared<ScidiLogger>();

    std::function<void(Rule*)> f1 = std::bind(&ScidiLogger::NewRegularity, reporter.get(), std::placeholders::_1);
    rule_generator_event->connect("regularity", f1);

    NaiveRuleGenerator rule_generator = NaiveRuleGenerator();
    rule_generator.SetInput(data.getStoragePointer());
    rule_generator.SetSettings(current_rule_settings);

    Thread * dummy_thread = new Thread();
    ThreadCommand * dummy_command = new ThreadCommand(dummy_thread);
    rule_generator.setCommand(dummy_command);

    rule_generator.setCallback(rule_generator_event);

    dummy_thread->start();
    rule_generator.GenRules();

    delete dummy_thread;
    delete dummy_command;
    delete rule_generator_event;
}

std::vector<double> RulesWrapper::getCriteriaValues(bool need_yule) {
    std::vector<double> criteria_values(storage->getSize());
    unsigned long m[2][2];

    double other_criteria;
    size_t counter = 0;
    for (ruleID iter = storage->begin(); iter != storage->end(); ++iter) {
        if (need_yule) {
            storage->ConvertFromLinkToRule(&(*iter))->GetFisher(data.getStoragePointer(), m, std::vector<int>(0), other_criteria, criteria_values[counter]);
        } else {
            storage->ConvertFromLinkToRule(&(*iter))->GetFisher(data.getStoragePointer(), m, std::vector<int>(0), criteria_values[counter], other_criteria);
        }
        ++counter;
    }

    return criteria_values;
}

std::vector<double> RulesWrapper::getFisher() {
    return getCriteriaValues(false);
}

std::vector<double> RulesWrapper::getYule() {
    return getCriteriaValues(true);
}

std::vector<double> RulesWrapper::getCP() {
    std::vector<double> cp_values(storage->getSize());

    size_t counter = 0;
    for (ruleID iter = storage->begin(); iter != storage->end(); ++iter) {
        cp_values[counter] = iter->getCP();
        ++counter;
    }

    return cp_values;
}

std::vector<TokenWrapper> RulesWrapper::getRuleAsTokenArray(unsigned int rule_id) {
    RulesStorage & s = *(storage.get());
    RuleLink & rule = s[rule_id];

    size_t rule_length = rule.getRuleLength();

    std::vector<TokenWrapper> token_representation(rule_length + 1);

    SToken current_token;

    for (size_t token_id = 0; token_id < rule_length; ++token_id)
    {
        current_token.nPos = rule[token_id].Shift;
        current_token.nValue = rule[token_id].Value;
        current_token.Sign = rule[token_id].Sign;
        token_representation[token_id].setToken(current_token);
    }

    SToken consequence;
    consequence.nPos = rule.getTTPos();
    consequence.nValue = rule.getTTValue();
    consequence.Sign = rule.getTTSign();

    token_representation[rule_length].setToken(consequence);

    return token_representation;
}
