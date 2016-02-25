#include "wrapper/ruleparser.h"

std::vector<RuleSection> RuleParser::getRuleSections(std::string rule_string)
{
    int premise_end = rule_string.find("=>");
    RuleSection target = predFromString(rule_string.substr(premise_end + 2));

    rule_string = rule_string.substr(0, premise_end);

    std::vector<Predicate> predicates;

    int start_pos = 0;
    int and_pos = rule_string.find('&');
    while (and_pos >= 0) {
        predicates.push_back(predFromString(rule_string.substr(start_pos, and_pos - start_pos)));
        start_pos = and_pos + 1;
        and_pos = rule_string.substr(start_pos).find('&');

        if (and_pos < 0) {
            break;
        }

        and_pos += start_pos;
    }
    predicates.push_back(predFromString(rule_string.substr(start_pos)));
    predicates.push_back(target);

    return predicates;
}

RuleSection RuleParser::predFromString(std::string pred_string) {
    RuleSection p;
    std::string not_string = "not";

    int not_pos = pred_string.find(not_string);
    int eq_pos = pred_string.find('=');

    p.Shift = stoi(pred_string.substr(0, eq_pos)) - 1;

    std::string val_str;

    if (not_pos > 0) {
        p.Sign = -1;
        val_str = pred_string.substr(not_pos + not_string.length());
    } else {
        p.Sign = 1;
        val_str = pred_string.substr(eq_pos + 1);
    }

    size_t first = val_str.find_first_not_of(' ');
    size_t last = val_str.find_last_not_of(' ');
    val_str =  val_str.substr(first, (last-first+1));

    p.Value = this->storage.getCode(val_str.c_str());

    return p;
}
