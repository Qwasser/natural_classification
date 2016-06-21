#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>

#include "json.hpp"
using json = nlohmann::json;

#include "natclass/predicate.h"

class Rule {
public:
    Rule(const std::vector<Predicate> & premise, const Predicate & conclusion);

    Rule(json & j);
    Rule(json && j);

    json toJSON() const;
    std::string toString() const;

    const Predicate conclusion;
    const std::vector<Predicate> premise;

    double getConditionalProbability() const;
    double getFisher() const;
    double getYule() const;

    void setConditionalProbability();
    void setFisher();
    void setYule();

    const Predicate & getConslusion();
    const std::vector<Predicate> & getPremise();

private:
    double conditional_probabiliy;
    double yule_lower_bound;
    double fisher_value;
};

class RuleBuilder {

};

#endif
