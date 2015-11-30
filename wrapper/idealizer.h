#ifndef IDEALIZER_H
#define IDEALIZER_H

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "CIdelObject.h"
#include "RulesStorage.h"
#include "Rule.h"


class Idealizer
{
public:
    Idealizer(SEQSElem & object, SEQStorage & train_storage, RulesStorage & rules);

    // Delete v_matrix
    ~Idealizer() {
        if (v_matrix != nullptr) {
            size_t feature_count = train_storage.getWidth();

            for (size_t column_id = 0; column_id < feature_count; column_id++)
            {
                delete [] v_matrix[column_id];
            }

            delete [] v_matrix;
        }
    }

    void runFastIdealization();
    void runStraightIdealization();

    CIdelObject getIdeal();
    double** getVmatrix();

    // Proxy to ideal
    double getGammaValue();

private:
    double** v_matrix = nullptr;
    CIdelObject ideal_object;

    std::vector<RuleLink*> applicable_rules;
    std::vector<RuleLink*> filtered_rules;
    std::set<RuleSection> applicable_rule_consequences;
    std::multimap<double, SToken> negative_modifications, positive_modifications;
    
    SEQSElem & object;
    RulesStorage & rules;
    SEQStorage & train_storage;
    
    void filterApplicableRules();
    void fillVMatrixFromApplicableRules();
    double logInverseProba(double x);

    void computeModificationsImpact();
    bool isApplicableRuleConsequence(SToken token);
    bool isAlone(SToken & token);

    double getRuleVValue(RuleLink * rule, bool reverse_belong = false);

    double excludeTest(SToken &token);
    double computeExcludeGammaChange(SToken& token);
    void filterApplicableRulesByConsequence(SToken &token);
    void filterRulesBrokenWithDelition(SToken &token);
    void filterNewApplicableRulesAfterDelition(SToken &token);

    double computeGammaChangeFromFilteredRules(bool reverse_belong = false);
};

#endif // IDEALIZER_H
