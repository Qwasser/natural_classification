#include <string>
#include <vector>

#include <cstddef>

#include "SEQStorage.h"
#include "RulesStorage.h"
#include "Tunnels.h"
#include "CIdelObject.h"

class ScidiWrapper {
public:
    void setData(const std::vector<std::vector<std::string> > & data);
    std::vector<std::vector<std::string> > getData();

    void makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth);
    std::vector<std::string> getRules();

    void makeClasses();
    std::vector<int> getClasses();

    std::vector<double> getFisher();
    std::vector<double> getYule();

private:
    SEQStorage * data = NULL;
    RulesStorage * rule_storage = NULL;
    CIdelObject** ideal_storage = NULL;

    RuleSettings current_rule_settings;
    std::vector<int> classes;

    std::vector<double> getCriteriaValues(bool need_yule = false);
};
