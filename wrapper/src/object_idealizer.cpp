#include "wrapper/object_idealizer.h"

#include <cmath>
#include <map>
#include <iostream>

ObjectIdealizer::ObjectIdealizer(ObjectWrapper object,
                                 RulesWrapper rules,
                                 DataWrapper data,
                                 bool strong_negation,
                                 TieBreakingAction action) : data(data),
                                                             strong_negation_mode(strong_negation),
                                                             action(action),
                                                             initial_rules(rules)

{
    SEQSElem obj = object.getObj();
    ideal_object.Create(&obj, data.getCodesCount());
    
    splitRulesByApplicability();

    for (RuleLink * rule : applicable_rules) {
        current_gamma += computeGamma(*rule);
    }
}

bool ObjectIdealizer::isPredicateApplicable(SToken & predicate,
                                            bool strong_negation) {
    if (predicate.Sign >= 0) {
        return ideal_object.isBelong(&predicate);
    } else {
        if (strong_negation_mode || strong_negation) {
            return !ideal_object.isBelong(&predicate);
        } else {
            SToken probe_predicate;
            probe_predicate.nPos = predicate.nPos;

            for (probe_predicate.nValue = 0;
                 probe_predicate.nValue < data.getCodesCount();
                 probe_predicate.nValue++)
            {
                if ((ideal_object.isBelong(&probe_predicate)) &&
                    (probe_predicate.nValue != predicate.nValue))
                {

                    return true;
                }
            }
            return false;
        }
    }
}

bool ObjectIdealizer::isPredicateApplicableCons(SToken & predicate) {
    return isPredicateApplicable(predicate, false);
}

bool ObjectIdealizer::isRuleApplicable(RuleLink & rule) {
    size_t rule_length = rule.getRuleLength();

    SToken current_token;
    for (size_t token_id = 0; token_id < rule_length; ++token_id)
    {
        current_token.nPos = rule[token_id].Shift;
        current_token.nValue = rule[token_id].Value;
        current_token.Sign = rule[token_id].Sign;

        if (!isPredicateApplicable(current_token)) {
            return false;
        }
    }

    return true;
}

double ObjectIdealizer::computeGamma(RuleLink & rule) {
    double cp = rule.getCP();

    SToken consequence = getConsequence(rule);
    if (isPredicateApplicableCons(consequence)) {
        return -log(1 - cp);
    } else {
        return log(1 - cp);
    }
}

void ObjectIdealizer::flipFeature(size_t attribute, size_t value) {
    SToken predicate;
    predicate.nPos = attribute;
    predicate.nValue = value;

    if (ideal_object.isBelong(&predicate)) {
        ideal_object.ExcludeT(&predicate);
    } else {
        ideal_object.IncludeT(&predicate);
    }
}

double ObjectIdealizer::computeGammaChangeOnAction(size_t attribute, size_t value) {

    flipFeature(attribute, value);
    double broken_consequenses_change = 0;

    for (size_t rule_num = 0; rule_num < applicable_rules.size(); ++rule_num) {

        RuleLink & rule = *(applicable_rules[rule_num]);

        SToken consequence = getConsequence(rule);

        bool consequence_was_applicable = app_rules_consequence_applicable[rule_num];

        if(consequence_was_applicable != isPredicateApplicableCons(consequence)) {
            broken_consequenses_change += 2 * computeGamma(rule);
        }
    }


    double new_applicable_rules_change = 0;
    for (RuleLink * rule : not_applicable_rules) {
        if(isRuleApplicable(*rule)) {
            new_applicable_rules_change += computeGamma(*rule);
        }
    }

    double broken_rules_change = 0;
    for (RuleLink * rule : applicable_rules) {
        if(!isRuleApplicable(*rule)) {
            broken_rules_change -= computeGamma(*rule);
        }
    }

    flipFeature(attribute, value);

    return  broken_consequenses_change +
            new_applicable_rules_change +
            broken_rules_change;
}

double ObjectIdealizer::computeGammaChangeOnActionBrute(size_t attribute, size_t value) {
    flipFeature(attribute, value);

    splitRulesByApplicability();

    double new_gamma = 0;
    for (RuleLink * rule : applicable_rules) {
        new_gamma += computeGamma(*rule);
    }

    flipFeature(attribute, value);

    return new_gamma - current_gamma;
}

void ObjectIdealizer::splitRulesByApplicability() {
    applicable_rules.resize(0);
    not_applicable_rules.resize(0);
    app_rules_consequence_applicable.resize(0);

    RulesStorage * storage_ptr = initial_rules.getStoragePointer();
    for (ruleID rules_iterator = storage_ptr->begin();
         rules_iterator != storage_ptr->end();
         ++rules_iterator) {
        RuleLink * rule = &(*rules_iterator);

        if (isRuleApplicable(*rule)) {
            applicable_rules.push_back(rule);

            SToken consequence = getConsequence(*rule);
            if (isPredicateApplicableCons(consequence)) {
                app_rules_consequence_applicable.push_back(true);
            } else {
                app_rules_consequence_applicable.push_back(false);
            }

        } else {
            not_applicable_rules.push_back(rule);
        }
    }
}

SToken ObjectIdealizer::getConsequence(RuleLink & rule) {
    SToken consequence;
    consequence.nPos = rule.getTTPos();
    consequence.nValue = rule.getTTValue();
    consequence.Sign = rule.getTTSign();
    return consequence;
}

bool ObjectIdealizer::idealizationStep(bool brute) {
    double best_delition_gamma = -INFINITY;
    SToken best_delition_token;

    double best_insertion_gamma = -INFINITY;
    SToken best_insertion_token;

    SToken current_token;

    for (current_token.nPos = 0; current_token.nPos < data.getWidth(); ++current_token.nPos)
    {
        for (current_token.nValue = 0; current_token.nValue < data.getCodesCount(); ++current_token.nValue)
        {
            double gamma_change;
            if (!brute) {
                gamma_change = computeGammaChangeOnAction(current_token.nPos,
                                                      current_token.nValue);
            }
            else {
                gamma_change = computeGammaChangeOnActionBrute(current_token.nPos,
                                                          current_token.nValue);
            }
            if (ideal_object.isBelong(&current_token) && gamma_change > best_delition_gamma) {
                best_delition_gamma = gamma_change;
                best_delition_token = current_token;    
            } else if (!ideal_object.isBelong(&current_token) && gamma_change > best_insertion_gamma){
                best_insertion_gamma = gamma_change;
                best_insertion_token = current_token;        
            }
        }
    }

    bool gamma_maximum_reached = true;

    last_deletion_gamma_change = best_delition_gamma;
    last_insertion_gamma_change = best_insertion_gamma;

    if (best_delition_gamma > 0 || best_insertion_gamma > 0) {
        gamma_maximum_reached = false;
    }

    if (best_delition_gamma > best_insertion_gamma) {
        ideal_object.ExcludeT(&best_delition_token);
        current_gamma += best_delition_gamma;

    } else if (best_delition_gamma < best_insertion_gamma) {
        ideal_object.IncludeT(&best_insertion_token);
        current_gamma += best_insertion_gamma;

    } else if (best_delition_gamma == best_insertion_gamma && best_insertion_gamma > 0) {
        if (action == insert) {
            ideal_object.IncludeT(&best_insertion_token);
            current_gamma += best_insertion_gamma;
        } else if (action == remove) {
            ideal_object.ExcludeT(&best_delition_token);
            current_gamma += best_delition_gamma;
        }
    }

    splitRulesByApplicability();
    return gamma_maximum_reached;
}

void ObjectIdealizer::idealizeToMaximumGamma() {
    while(!idealizationStep()) {};
}
