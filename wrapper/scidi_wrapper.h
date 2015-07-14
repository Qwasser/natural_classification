#include <string>
#include <vector>

#include <cstddef>

#include "SEQStorage.h"
#include "RulesStorage.h"
#include "Tunnels.h"

class ScidiWrapper {
public:
    void setData(const std::vector<std::vector<std::string> > & data);
    std::vector<std::vector<std::string> > getData();
//    std::vector<std::vector<int> > getEncodedData();

    void makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth);
    std::vector<std::string> getRules();

//    std::vector<int> makeClasses();

    std::vector<double> getFisher();
    std::vector<double> getYule();

private:
    SEQStorage * data = NULL;
    RulesStorage * rule_storage = NULL;

    RuleSettings current_rule_settings;
    std::vector<int> classes;

    std::vector<double> getCriteriaValues(bool need_yule = false);
};
