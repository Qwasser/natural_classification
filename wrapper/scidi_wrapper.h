#include <string>
#include <vector>

#include <cstddef>

#include "SEQStorage.h"
#include "RulesStorage.h"
#include "Tunnels.h"
#include "CIdelObject.h"

#include "ruleparser.h"

class ScidiWrapper {
public:
    void setData(const std::vector<std::vector<std::string> > & data);
    std::vector<std::vector<std::string> > getData();

    void intitRuleStorage();
    void makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth);
    std::vector<std::string> getRules();

    void makeClasses();
    std::vector<int> getClasses();
    std::vector<std::vector<std::string> > getIdealObjects();

    std::vector<double> getFisher();
    std::vector<double> getYule();

    void addRuleFromString(std::string rule_str) {
        Rule * rule = parseRule(rule_str);
        std::cout << "Rule info: " << std::endl;
        std::cout << "Length: " << rule->getRuleLength() << std::endl;
        std::cout << "Rule: " << rule->getChainStr() << std::endl;
        std::cout << "Is known: " << rule_storage->isFind(rule) << std::endl;
        rule->Probability(this->data);
        std::cout << "Prob: " << rule->getCP() << std::endl;

        long Target_p = rule->getTTPos();
        char Target_s = rule->getTTSign();
        int Target_v = rule->getTTValue();

        std::cout << "Prd pos: " << rule_storage->GetPredicatePos(Target_p, Target_s, Target_v) << std::endl;

        RuleSection chain = rule->Chain(0);

        std::cout << rule_storage->Add(rule) << std::endl;
        std::cout << rule_storage->getSize() << std::endl;
        std::cout << "Is known: " << rule_storage->isFind(rule) << std::endl;
    }

    Rule * parseRule(std::string rule_str) {
        RuleLink * rule_link = RuleParser(*(this->data)).parseRuleLink(rule_str);
        return rule_storage->ConvertFromLinkToRule(rule_link);
    }

private:
    SEQStorage * data = NULL;

    RulesStorage * rule_storage = NULL;
    RuleSettings current_rule_settings;

    CIdelObject** ideal_storage = NULL;
    size_t ideal_storage_size;

    std::vector<double> getCriteriaValues(bool need_yule = false);
};
