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

    void makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth);
    std::vector<std::string> getRules();

    void makeClasses();
    std::vector<int> getClasses();
    std::vector<std::vector<std::string> > getIdealObjects();

    std::vector<double> getFisher();
    std::vector<double> getYule();

    void setRulesFromStringVector(std::vector<std::string> rule_strings) {
        intitRuleStorage();
        for (auto str : rule_strings) {
            addRuleFromString(str);
        }
        rule_storage->MakePointersArray();
    }
    SEQStorage * data = NULL;
private:


    RulesStorage * rule_storage = NULL;
    RuleSettings current_rule_settings;

    CIdelObject** ideal_storage = NULL;
    size_t ideal_storage_size;

    std::vector<double> getCriteriaValues(bool need_yule = false);

    void addRuleFromString(std::string rule_str);
    Rule * parseRule(std::string rule_str);

    void intitRuleStorage();
};
