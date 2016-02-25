#include "wrapper/object_idealizer.h"

#include <cmath>
#include <map>

ObjectIdealizer::ObjectIdealizer(SEQSElem & object,
                                 RulesStorage & rules,
                                 size_t codes_count,
                                 bool strong_negation,
                                 TieBreakingAction action) : codes_count(codes_count),
                                                             strong_negation(strong_negation),
                                                             action(action),
                                                             initial_rules(rules)

{

}

bool ObjectIdealizer::isPredicateApplicable(SToken & predicate) {
    if (predicate.Sign >= 0) {
        return ideal_object.isBelong(&predicate);
    } else {
        if (strong_negation) {
            return !ideal_object.isBelong(&predicate);
        } else {
            SToken probe_predicate;
            probe_predicate.nPos = predicate.nPos;

            for (probe_predicate.nValue = 0; probe_predicate.nValue < codes_count; probe_predicate.nValue++)
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
    if (isPredicateApplicable(consequence)) {
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

        if(consequence_was_applicable != isPredicateApplicable(consequence)) {
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

void ObjectIdealizer::splitRulesByApplicability() {
    applicable_rules.resize(0);
    not_applicable_rules.resize(0);
    app_rules_consequence_applicable.resize(0);

    for (RuleLink rule : initial_rules)
    {
        if (isRuleApplicable(rule)) {
            applicable_rules.push_back(&rule);

            SToken consequence = getConsequence(rule);
            if (isPredicateApplicable(consequence)) {
                app_rules_consequence_applicable.push_back(true);
            } else {
                app_rules_consequence_applicable.push_back(false);
            }
        } else {
            not_applicable_rules.push_back(&rule);
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

bool ObjectIdealizer::idealizationStep() {
    double best_delition_gamma = 0;
    SToken best_delition_token;

    double best_insertion_gamma = 0;
    SToken best_insertion_token;

    SToken current_token;
    for (current_token.nPos = 0; current_token.nPos < attribute_count; ++current_token.nPos)
    {
        for (current_token.nValue = 0; current_token.nValue < codes_count; ++current_token.nValue)
        {
            double gamma_change = computeGammaChangeOnAction(current_token.nPos,
                                                      current_token.nValue);
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
