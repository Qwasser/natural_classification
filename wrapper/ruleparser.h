#ifndef RULEPARSER_H
#define RULEPARSER_H

#include "Rule.h"
#include "SEQStorage.h"

#include <vector>

class RuleParser
{
private:
    SEQStorage & storage;

public:
    RuleParser(SEQStorage & storage) : storage(storage) {}

    RuleLink * parseRuleLink(std::string rule_string) {
        std::vector<RuleSection> sections = getRuleSections(rule_string);

        RuleSection * target = new RuleSection(sections.back());
        UINT * chain = new UINT[sections.size() + 2];

        chain[0] = sections.size() - 1;
        chain[1] = 0;
        chain[2] = 0;

        for (size_t i = 0; i < sections.size() - 1; ++i) {
            chain[i + 3] = storage.GetPredicatePos(sections[i].Shift, sections[i].Sign, sections[i].Value);
        }

        RuleLink * rule_link = new RuleLink();
        rule_link->setChain(chain);
        rule_link->setTarget(target);

        return rule_link;
    }

private:
    std::vector<RuleSection> getRuleSections(std::string rule_string);
    RuleSection predFromString(std::string pred_string);
};

#endif // RULEPARSER_H
