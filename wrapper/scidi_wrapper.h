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
    std::vector<std::vector<int> > getEncodedData();

    void makeRules(double conf_int_value, double yule_value, double min_cp, size_t depth);
    std::vector<std::string> getRules();

private:
    SEQStorage * data = NULL;
    RulesStorage * rule_storage = NULL;

    RuleSettings current_rule_settings;
};
