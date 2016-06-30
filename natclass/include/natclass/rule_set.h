#ifndef RULE_SET_H
#define RULE_SET_H

#include <iostream>
#include <iterator>

#include "rule.h"
#include "problem.h"

/*!
 * \brief The AbstractRuleSet class
 * Ruleset is used to store and access rules
 */

class AbstractRuleSet {
public:
    //! Adding single rule
    virtual void addRule(Rule & r) = 0;
    virtual void addRule(Rule && r) = 0;

    //! Adding vector of rules
    virtual void addRules(std::vector<Rule> & rules) = 0;

    //! Getters
    virtual const Rule & getRule(size_t rule_id) const = 0;
    virtual const Problem & getProblem();

    //! Iteration
    typedef Iterator std::iterator<std::bidirectional_iterator_tag, Rule>;
    const Iterator begin();
    const Iterator end();

    //! Serrialization
    //! Serrialize to preset ostream
    virtual void setOstream(std::ostream&) = 0;
    virtual void save() const = 0;

    //! Serrialize to given ostream
    virtual void save(std::ostream&) const = 0;

    virtual void load(std::istream&) const = 0;

    //! Destructor
    virtual ~AbstractRuleSet() = 0;
};

#endif
