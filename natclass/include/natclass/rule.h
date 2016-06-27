#ifndef RULE_H
#define RULE_H

#include <vector>

#include "json.hpp"
using json = nlohmann::json;

#include "predicate.h"

/*!
 * \brief The Rule class represents logical rule
 *
 * Rule consists of several predicates in premise and single predicate in conclusion.
 * Predicates cannot be overwritten directly.
 * Criterion values (Yule, Fisher, conditional probability) are also associated with rule but can be overwriten
 * becouse they depend on actual data.
 */
class Rule {
public:
    Rule(const std::vector<Predicate> & premise, const Predicate & conclusion);

    //! Deserrialization from json
    Rule(json & j);
    Rule(json && j);

    //! Serrialization from json
    json toJSON() const;

    //! Serrialization to human readable string
    std::string toString(const Problem & p) const;


    //! Getters for criterion values
    double getConditionalProbability() const;
    double getFisher() const;
    double getYule() const;

    //! Setters for criterion values
    void setConditionalProbability(double value);
    void setFisher(double value);
    void setYule(double value);

    //! Getters for predicates
    const Predicate & getConslusion();
    const std::vector<Predicate> & getPremise();

    //! Operators
    //! Ignores criterion values
    bool operator==(const Rule &other) const;

    bool operator!=(const Rule &other) const {
      return !(*this == other);
    }

private:
    double conditional_probabiliy = 0;
    double yule_lower_bound = 0;
    double fisher_value = 0;

    Predicate conclusion;
    std::vector<Predicate> premise;

    void fromJSON(json & j);

    static const std::string FISHER_FIELD_NAME;
    static const std::string YULE_FIELD_NAME;
    static const std::string CONDITIONAL_PROB_FIELD_NAME;

    static const std::string PREMISE_FIELD_NAME;
    static const std::string CONCLUSION_FIELD_NAME;
};

class RuleBuilder {
public:
    /*!
     * Sets builder premise and conclusion to premise and conclusion
     * of given rule
     */
    void initFromRule(Rule & r);
    void initFromRule(Rule && r);

    //! conclusion setter
    void setConclusion(const Predicate & conclusion);
    void setConclusion(Predicate && conclusion);
    void setConclusion(size_t feature_id, int value, bool sign);

    //! Adds predicate to the end of premise
    void pushToPremise(const Predicate & pred);
    void pushToPremise(Predicate && pred);
    void pushToPremise(size_t feature_id, int value, bool sign);

    //! Removes predicate from the end of premise
    void popFromPremise(const Predicate & pred);
    void popFromPremise(Predicate && pred);

    //! Gets rule with current premise and conclusion
    Rule getResult();

private:
    Predicate conclusion;
    std::vector<Predicate> premise;

};

#endif
