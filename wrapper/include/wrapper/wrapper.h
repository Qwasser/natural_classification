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

    std::vector<double> getFisher();
    std::vector<double> getYule();
    std::vector<double> getCP();

    void setRulesFromStringVector(std::vector<std::string> rule_strings) {
        intitRuleStorage();
        for (auto str : rule_strings) {
            addRuleFromString(str);
        }
        rule_storage->MakePointersArray();
    }



private:
    SEQStorage * data = NULL;
    RulesStorage * rule_storage = NULL;


    RuleSettings current_rule_settings;

    CIdelObject** ideal_storage = NULL;
    size_t ideal_storage_size;

    std::vector<double> getCriteriaValues(bool need_yule = false);

    void addRuleFromString(std::string rule_str);
    Rule * parseRule(std::string rule_str);

    void intitRuleStorage();

    SEQStorage * makeDataStorage(const std::vector<std::vector<std::string> > & data);
    int makeClasses(SEQStorage * storage, CIdelObject ** ideal_storage);
    std::vector<std::vector<std::string> > getIdealObjects(SEQStorage * storage,
                                                         CIdelObject ** i_storage,
                                                         int i_storage_size);
};
