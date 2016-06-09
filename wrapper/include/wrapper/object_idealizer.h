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
                    TieBreakingAction action=insert);

    bool idealizationStep(bool brute = false);

    void idealizeToMaximumGamma();

    IdealObjectWrapper getIdealObject() {
        return IdealObjectWrapper(ideal_object);
    }

    double getGamma() {
        return current_gamma;
    }

    double getLastInsertionGamma() {
        return last_insertion_gamma_change;
    }

    double getLastDeletionGamma() {
        return last_deletion_gamma_change;
    }

    RulesWrapper getApplicableRules() {
        return RulesWrapper(applicable_rules, data);
    }

    RulesWrapper getNonApplicableRules() {
        return RulesWrapper(not_applicable_rules, data);
    }

    // Debug methods
    RulesWrapper getLastApplicableRules() {
        return RulesWrapper(last_applicable_rules, data);
    }

    RulesWrapper getLastBrokenRules() {
        return RulesWrapper(last_broken_rules, data);
    }

    RulesWrapper getLastPredictingRules() {
        return RulesWrapper(last_predicting_rules, data);
    }

    std::vector<double> getLastApplicableRulesGamma() {
        return last_applicable_rules_gamma;
    }

    std::vector<double> getLastBrokenRulesGamma() {
        return last_broken_rules_gamma;
    }

    std::vector<double> getLastPredictingRulesGamma() {
        return last_predicting_rules_gamma;
    }

    TokenWrapper getLastMovedPredicate() {
        return TokenWrapper(last_moved_predicate);
    }

private:
    DataWrapper data;

    bool strong_negation_mode;
    TieBreakingAction action;

    CIdelObject ideal_object;
    double current_gamma = 0;
    double last_insertion_gamma_change = 0;
    double last_deletion_gamma_change = 0;

    bool isPredicateApplicable(SToken & predicate,
                               bool strong_negetion);

    bool isPredicateApplicableCons(SToken & predicate);

    bool isRuleApplicable(RuleLink & rule);
    double computeGamma(RuleLink & rule);

    void flipFeature(size_t attribute, size_t value);

    RulesWrapper initial_rules;
    
    std::vector<RuleLink *> applicable_rules;
    std::vector<bool> app_rules_consequence_applicable;
    std::vector<RuleLink *> not_applicable_rules;

    double computeGammaChangeOnAction(size_t attribute, size_t value, bool record_rules_distribution = false);
    double computeGammaChangeOnActionBrute(size_t attribute, size_t value);

    void splitRulesByApplicability();

    SToken getConsequence(RuleLink & rule);

    // Rules, took part in last decision, keep for debugging
    SToken last_moved_predicate;

    std::vector<RuleLink *> last_applicable_rules;
    std::vector<RuleLink *> last_broken_rules;
    std::vector<RuleLink *> last_predicting_rules;

    std::vector<double> last_applicable_rules_gamma;
    std::vector<double> last_broken_rules_gamma;
    std::vector<double> last_predicting_rules_gamma;
};

#endif // OBJECT_IDEALIZER_H
