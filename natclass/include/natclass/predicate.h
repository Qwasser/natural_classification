#ifndef PREDICATE_H
#define PREDICATE_H

#include "json.hpp"
using json = nlohmann::json;

#include "problem.h"

/*!
 * Class represents predicate from which logic rules a composed
 */
class Predicate {
public:
    //! Basic constructor
    Predicate(int val, size_t feature, bool sign);

    //! Construction from string representation
    Predicate(std::string str, Problem & p);

    //! Deserrialization from json
    Predicate(json & j);
    Predicate(json && j);

    //! Serrialization to json
    json toJSON() const;

    /*!
     * Serrialization to human readable string
     * "featrue_id" is {not} "val"
     */
    std::string toString(Problem & p) const;

    //! value getter
    int getValue() const;

    //! featiure id getter
    size_t getFeatureID() const;

    //! sign getter
    bool getSign() const;

private:
    int value;
    size_t feature_id;
    bool sign;

    void fromJSON(json & j);

    static const std::string SIGN_FIELD_NAME;
    static const std::string VALUE_FIELD_NAME;
    static const std::string FEATURE_FIELD_NAME;
};

const std::string Predicate::SIGN_FIELD_NAME = "sign";
const std::string Predicate::VALUE_FIELD_NAME = "val";
const std::string Predicate::FEATURE_FIELD_NAME = "feature";

#endif
