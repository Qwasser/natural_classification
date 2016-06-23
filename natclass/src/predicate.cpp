#include <sstream>
#include <iostream>

#include "natclass/predicate.h"

Predicate::Predicate(int val, size_t feature, bool sign) : value(val), feature_id(feature), sign(sign) {}

Predicate::Predicate(const Predicate & other) {
    value = other.value;
    feature_id = other.feature_id;
    sign = other.sign;
}

Predicate::Predicate(std::string str, Problem & p) {
    std::stringstream ss(str);
    ss >> feature_id;

    std::string token;
    ss >> token; //skipping 'is'

    ss >> token;

    sign = true;
    if (token == "not" && ss.tellg() != -1) {
        sign = false;
        ss >> token;
    }

    value = p.encode(feature_id, token);
}

Predicate::Predicate(json & j) {
    fromJSON(j);
}

Predicate::Predicate(json && j) {
    fromJSON(j);
}

json Predicate::toJSON() const {
    json j;

    j[VALUE_FIELD_NAME] = value;
    j[FEATURE_FIELD_NAME] = feature_id;
    j[SIGN_FIELD_NAME] = sign;

    return j;
}

std::string Predicate::toString(const Problem & p) const {
    std::stringstream ss;
    ss << feature_id << " is ";
    if (! sign) {
        ss << "not ";
    }

    ss << p.decode(feature_id, value);
    return ss.str();
}

int Predicate::getValue() const {
    return value;
}

size_t Predicate::getFeatureID() const {
    return feature_id;
}

bool Predicate::getSign() const {
    return sign;
}

void Predicate::fromJSON(json & j) {
    value = j[VALUE_FIELD_NAME];
    feature_id = j[FEATURE_FIELD_NAME];
    sign = j[SIGN_FIELD_NAME];
}

bool Predicate::operator==(const Predicate &other) const {
    return (value == other.value) && (feature_id == other.feature_id) && (sign == other.sign);
}

const std::string Predicate::SIGN_FIELD_NAME = "sign";
const std::string Predicate::VALUE_FIELD_NAME = "val";
const std::string Predicate::FEATURE_FIELD_NAME = "feature";
