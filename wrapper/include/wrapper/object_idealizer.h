#ifndef OBJECT_IDEALIZER_H
#define OBJECT_IDEALIZER_H

#include <vector>
#include <map>

#include <wrapper/data_wrapper.h>
#include <wrapper/ideal_object_wrapper.h>
#include <wrapper/rules_wrapper.h>

#include "CIdelObject.h"
#include "RulesStorage.h"
#include "Rule.h"

class ObjectIdealizer
{
public:
    enum TieBreakingAction {insert, remove};

    ObjectIdealizer(ObjectWrapper object,
                    RulesWrapper rules,
                    DataWrapper data,
                    bool strong_negation=false,
                    TieBreakingAction action=remove);

    bool idealizationStep();
    void idealizeToMaximumGamma();

    IdealObjectWrapper getIdealObject() {
        return IdealObjectWrapper(ideal_object);
    }

    double getGamma() {
        return current_gamma;
    }

    RulesWrapper getApplicableRules() {
        return RulesWrapper(applicable_rules, data);
    }

private:
    DataWrapper data;

    bool strong_negation;
    TieBreakingAction action;

    CIdelObject ideal_object;
    double current_gamma = 0;

    bool isPredicateApplicable(SToken & predicate);
    bool isRuleApplicable(RuleLink & rule);
    double computeGamma(RuleLink & rule);

    void flipFeature(size_t attribute, size_t value);

    RulesWrapper initial_rules;
    
    std::vector<RuleLink *> applicable_rules;
    std::vector<bool> app_rules_consequence_applicable;
    std::vector<RuleLink *> not_applicable_rules;

    double computeGammaChangeOnAction(size_t attribute, size_t value);

    void splitRulesByApplicability();

    SToken getConsequence(RuleLink & rule);
};

#endif // OBJECT_IDEALIZER_H
