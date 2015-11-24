#include "idealizer.h"
#include <vector>
#include<cmath>

Idealizer::Idealizer(SEQSElem & object, SEQStorage & train_storage, RulesStorage & rules) : object(object),
                                                                                            train_storage(train_storage),
                                                                                            rules(rules)
{
    // Initializing v_matrix
    v_matrix = new double* [train_storage.getWidth()];
    
    size_t feature_count = train_storage.getWidth();
    size_t codes_count = train_storage.getCodesCount();
    
	for (size_t column_id = 0; column_id < feature_count; column_id++)
	{
		v_matrix[column_id] = new double[codes_count];
		for (size_t  row_id = 0; row_id < codes_count; row_id++)
		{
            v_matrix[column_id][row_id] = 0;
		}
	}
    
    // Init ideal object
    ideal_object.Create(&object, codes_count);
    
    // Compute v_matrix for original object
    filterApplicableRules();
    fillVMatrixFromApplicableRules();
}

void Idealizer::filterApplicableRules() {    
	//отбираем правила, содержащие признаки, т.е.
	//совпадает предикат и его значение из посылки с элементом ид объекта
	SToken current_token;
	bool is_applicable;
	long rule_length;
    
    applicable_rules.resize(0);
    
	for (ruleID rules_iterator = rules.begin(); rules_iterator != rules.end(); ++rules_iterator)
	{
        RuleLink* current_rule = &(*rules_iterator);
        
		is_applicable = true;
		rule_length = current_rule->getRuleLength();
        
		for (size_t token_id = 0; token_id < rule_length; ++token_id)
		{
			current_token.nPos = (*current_rule)[token_id].Shift;
			current_token.nValue = (*current_rule)[token_id].Value;
			current_token.Sign = (*current_rule)[token_id].Sign;

			if (current_token.Sign < 0)
			{
				if ( ideal_object.isBelong(&current_token))
				{
                    is_applicable = false;
				}
    		}    
            
			else if (!ideal_object.isBelong(&current_token))
	        {
		        is_applicable = false;
			}
            
            if (!is_applicable) {
                break;
            }
		}

		if (is_applicable)
		{
            applicable_rules.push_back( current_rule );
            RuleSection rule_consequence;

            rule_consequence.Shift = current_rule->getTTPos();
            rule_consequence.Value = current_rule->getTTValue();
            rule_consequence.Sign = current_rule->getTTSign();

            applicable_rule_consequences.insert(rule_consequence);
		}
	}    
}

void Idealizer::fillVMatrixFromApplicableRules() {
    for (RuleLink * rule : applicable_rules) {
        v_matrix[rule->getTTPos()][rule->getTTValue()] += getRuleVValue(rule);
    }
}

double Idealizer::logInverseProba(double x)
{
   return (-log(1-x));
}

double Idealizer::getGammaValue() {
    double gamma_value = 0;

    for ( unsigned long pos = 0; pos < train_storage.getWidth(); pos++)
        for ( int code_id = 0; code_id < train_storage.getCodesCount(); code_id++)
            gamma_value += v_matrix[pos][code_id];

    return gamma_value;
}

void Idealizer::runFastIdealization() {
    ideal_object.setGamma(getGammaValue());

    bool stop_flag = false;
    while (!stop_flag)
    {
        computeModificationsImpact();
//        stop_flag = tryApplyBestModification();
    }
//    deleteZeroFeatures();
}

void Idealizer::computeModificationsImpact() {
    SToken current_token;
    double gamma_change;
    negative_modifications.clear();
    positive_modifications.clear();

    for (current_token.nPos = 0; current_token.nPos < train_storage.getWidth(); current_token.nPos++) {
        for (current_token.nValue = 0; current_token.nValue < train_storage.getCodesCount(); current_token.nValue++) {
            current_token.Sign = -1;
            if ( ideal_object.isBelong(&current_token) && isApplicableRuleConsequence(current_token) ) {
                gamma_change = excludeTest(current_token);

                if ( gamma_change > 0. )
                {
                    negative_modifications.insert(std::make_pair(gamma_change, current_token));
                }
            }

            current_token.Sign = 1;
            if ( ideal_object.isBelong(&current_token) && isApplicableRuleConsequence(current_token) ) {
//                gamma_change = includeTest(current_token);

                if ( gamma_change > 0. )
                {
                    positive_modifications.insert(std::make_pair(gamma_change, current_token));
                }
            }
        }
    }
}

bool Idealizer::isApplicableRuleConsequence(SToken token) {
    RuleSection rs;
    rs.Shift = token.nPos;
    rs.Value = token.nValue;
    rs.Sign = token.Sign;

    if(applicable_rule_consequences.find(rs) != applicable_rule_consequences.end())
        return false;
    else
        return true;
}


double Idealizer::excludeTest(SToken& token) {
    double gamma_change = 0.;
    ideal_object.ExcludeT(&token);
    gamma_change = computeExcludeGammaChange(token);
    ideal_object.IncludeT(&token);
    return gamma_change;
}

double Idealizer::computeExcludeGammaChange(SToken &token) {
    double S1 = 0, S2 = 0, S4 = 0;

    filterApplicableRulesByConsequence(token);
    S1 = -2. * computeGammaChangeFromFilteredRules();

//    Filtr_Del1(token);
//    S2 = -1. * PartialSum();

//    Filtr_Del4(token);
//    S4 = PartialSum();

    return (S1 + S2 + S4);
}

void Idealizer::filterApplicableRulesByConsequence(SToken& token) {
    filtered_rules.resize(0);

    for (RuleLink * rule : applicable_rules) {
        if((rule->getTTPos() == token.nPos ) && (rule->getTTValue() == token.nValue)) {
            filtered_rules.push_back(rule);
        }
    }
}

double Idealizer::computeGammaChangeFromFilteredRules()
{
    double gamma_change = 0;
    for (RuleLink * rule : applicable_rules)
    {
        gamma_change += getRuleVValue(rule);
    }
    return gamma_change;
}

double Idealizer::getRuleVValue(RuleLink * rule) {
    SToken current_token;
    current_token.nPos = rule->getTTPos();
    current_token.nValue = rule->getTTValue();

    bool is_belong = !ideal_object.isBelong(&current_token);
    bool is_positive = rule->getTTSign() > 0;
    double cp_value = rule->getCP();

    double v_value;

    if ( is_positive && is_belong )
    {
        v_value = logInverseProba(cp_value); // если законмерность подверждается
    }
    else if ( !is_positive && !is_belong  )
    {
        v_value = logInverseProba(fabs(cp_value));  // если законмерность подверждается
    }
    else if ( is_positive && !is_belong )
    {
        v_value = -logInverseProba(cp_value);   // если законмерность опровергается
    }
    else if ( !is_positive && is_belong )
    {
        v_value = -logInverseProba(fabs(cp_value));   // если законмерность опровергается
    }

    return v_value;
}


void Idealizer::filterApplicableRulesByPremise(SToken& token)
{
    for (RuleLink * rule : applicable_rules)
    {
       long rule_length = rule->getRuleLength();
       bool isOld = false;
       for (long l=0; l < rule_length; l++)
       {
           SToken current_predicate;
           current_predicate.nPos = *rule[l].Shift;
           current_predicate.nValue = *rule[l].Value;
           current_predicate.Sign = *rule[l].Sign;

            if ( (current_predicate.Sign > 0 ) && (current_predicate.nPos == token->nPos ) && (current_predicate.nValue == token->nValue) )
            {
                isOld = true;
            }
            // проверим, удаляем ли мы последнее значение (stok) для признака
            // если удалили, то любой другой на его месте будет одинок
            if ( IsAlone(&current_predicate) && (current_predicate.Sign < 0 ) && (current_predicate.nPos == token->nPos ) && (current_predicate.nValue != token->nValue) )
            {
                isOld = true;
            }

       }
       if ( isOld )
       {
            filtered_rules.push_back(rule);
       }
    }
}
