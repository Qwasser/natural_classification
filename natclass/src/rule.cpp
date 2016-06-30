#include "natclass/rule.h"

#include <sstream>
#include <iostream>

Rule::Rule(const std::vector<Predicate> & premise, const Predicate & conclusion) : premise(premise), conclusion(conclusion) {

}

void Rule::fromJSON(json & j) {
    fisher_value = j[FISHER_FIELD_NAME];
    yule_lower_bound = j[YULE_FIELD_NAME];
    conditional_probabiliy = j[CONDITIONAL_PROB_FIELD_NAME];

    conclusion = Predicate(json(j[CONCLUSION_FIELD_NAME]));

    premise.reserve(j[PREMISE_FIELD_NAME].size());
    for (auto val : j[PREMISE_FIELD_NAME]) {
        premise.push_back(Predicate(json(val)));
    }
}

Rule::Rule(json & j) {
    fromJSON(j);
}

Rule::Rule(json && j) {
    fromJSON(j);
}

json Rule::toJSON() const {
    json j;

    j[FISHER_FIELD_NAME] = fisher_value;
    j[YULE_FIELD_NAME] = yule_lower_bound;
    j[CONDITIONAL_PROB_FIELD_NAME] = conditional_probabiliy;

    j[CONCLUSION_FIELD_NAME] = conclusion.toJSON();

    json p;
    for (auto pred : premise) {
        p.push_back(pred.toJSON());
    }

    j[PREMISE_FIELD_NAME] = p;

    return j;
}

std::string Rule::toString(const Problem & p) const {
    std::stringstream ss;

    for (size_t i = 0; i < premise.size(); ++i) {
        ss << '(' << premise[i].toString(p) << ')';
        if (i != premise.size() - 1) {
            ss << " & ";
        }
    }

    ss << " => ";
    ss << '(' << conclusion.toString(p) << ')';
    return ss.str();
}

double Rule::getConditionalProbability() const {
    return conditional_probabiliy;
}

double Rule::getFisher() const {
    return fisher_value;
}

double Rule::getYule() const {
    return yule_lower_bound;
}

void Rule::setConditionalProbability(double value) {
    conditional_probabiliy = value;
}

void Rule::setFisher(double value) {
    fisher_value = value;
}

void Rule::setYule(double value) {
    yule_lower_bound = value;
}

const Predicate & Rule::getConslusion() {
    return conclusion;
}

const std::vector<Predicate> & Rule::getPremise() {
    return premise;
}

bool Rule::operator==(const Rule &other) const {
    return conclusion == other.conclusion &&
            premise == other.premise;
}

const std::string Rule::FISHER_FIELD_NAME = "fisher";
const std::string Rule::YULE_FIELD_NAME = "yule";
const std::string Rule::CONDITIONAL_PROB_FIELD_NAME = "cp";
const std::string Rule::PREMISE_FIELD_NAME = "premise";
const std::string Rule::CONCLUSION_FIELD_NAME = "conclusion";

void RuleBuilder::initFromRule(Rule & r) {
    const std::vector<Predicate> & p = r.getPremise();
    premise.clear();
    premise.insert(premise.begin(), p.begin(), p.end());

    conclusion = r.getConslusion();
}

void RuleBuilder::initFromRule(Rule && r) {
    initFromRule(r);
}

Rule RuleBuilder::getResult() {
    return Rule(premise, conclusion);
}

void RuleBuilder::pushToPremise(const Predicate & pred) {
    premise.push_back(pred);
}

void RuleBuilder::pushToPremise(Predicate && pred) {
    pushToPremise(pred);
}

void RuleBuilder::pushToPremise(size_t feature_id, int value, bool sign) {
    pushToPremise(Predicate(feature_id, value, sign));
}
