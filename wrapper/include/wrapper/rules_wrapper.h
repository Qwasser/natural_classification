#ifndef RULES_WRAPPER_H
#define RULES_WRAPPER_H

#include <vector>
#include <memory>

#include "RulesStorage.h"

#include "wrapper/data_wrapper.h"

class RulesWrapper {
public:
  RulesWrapper(std::vector<std::string> text_rules, DataWrapper data);
  RulesWrapper(std::vector<RuleLink *> rule_links, DataWrapper data);
  RulesWrapper(double conf_int_value,
               double yule_value,
               double min_cp,
               size_t depth,
               DataWrapper data,
               bool negate_premise = false,
               bool negate_consequence = false);

  std::vector<double> getFisher();
  std::vector<double> getYule();
  std::vector<double> getCP();

  std::vector<std::string> asStringVectors();

  RulesStorage * getStoragePointer() {
      return storage.get();
  }

  long size() {
      return storage->getSize();
  }

private:
  std::shared_ptr<RulesStorage> storage;
  DataWrapper data;

  void addRuleFromString(std::string rule_str);
  Rule * parseRule(std::string rule_str);

  std::vector<double> getCriteriaValues(bool need_yule);
};
#endif
