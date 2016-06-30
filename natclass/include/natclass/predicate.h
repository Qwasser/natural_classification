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
    Predicate() {}

    //! Copy constructor
    Predicate(const Predicate & other);

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
    std::string toString(const Problem & p) const;

    //! value getter
    int getValue() const;

    //! featiure id getter
    size_t getFeatureID() const;

    //! sign getter
    bool getSign() const;

    //! Operators
    bool operator==(const Predicate &other) const;

    bool operator!=(const Predicate &other) const {
      return !(*this == other);
    }
private:
    int value = 0;
    size_t feature_id = 0;
    bool sign = true;

    void fromJSON(json & j);

    static const std::string SIGN_FIELD_NAME;
    static const std::string VALUE_FIELD_NAME;
    static const std::string FEATURE_FIELD_NAME;
};
#endif
