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
                    break;
				}

                is_applicable = false;

                if ( !isAlone(current_token))
                {
                    is_applicable = true;
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
        stop_flag = tryApplyBestModification();
    }
    deleteZeroFeatures();
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
                gamma_change = includeTest(current_token);

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
    S1 = -2. * computeGammaChangeFromFilteredRules(true);

    filterRulesBrokenWithDelition(token);
    S2 = -1. * computeGammaChangeFromFilteredRules();

    filterNewApplicableRulesAfterDelition(token);
    S4 = computeGammaChangeFromFilteredRules();

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

double Idealizer::computeGammaChangeFromFilteredRules(bool reverse_belong)
{
    double gamma_change = 0;
    for (RuleLink * rule : applicable_rules)
    {
        gamma_change += getRuleVValue(rule, reverse_belong);
    }
    return gamma_change;
}

double Idealizer::getRuleVValue(RuleLink * rule, bool reverse_belong) {
    SToken current_token;
    current_token.nPos = rule->getTTPos();
    current_token.nValue = rule->getTTValue();

    bool is_belong = ideal_object.isBelong(&current_token);
    if (reverse_belong) {
        is_belong = !is_belong;
    }

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


void Idealizer::filterRulesBrokenWithDelition(SToken& token)
{
    filtered_rules.resize(0);

    for (RuleLink * rule : applicable_rules)
    {
        long rule_length = rule->getRuleLength();
        for (long l=0; l < rule_length; l++)
        {
           SToken current_predicate;
           current_predicate.nPos = (*rule)[l].Shift;
           current_predicate.nValue = (*rule)[l].Value;
           current_predicate.Sign = (*rule)[l].Sign;

            if ((current_predicate.Sign > 0 ) &&
                (current_predicate.nPos == token.nPos ) &&
                (current_predicate.nValue == token.nValue))
            {
                filtered_rules.push_back(rule);
                break;
            }

            /* If we deleted the last value in attribute
             * and the is part of rule that negates of any value
             * in the attribute - that rule not appplies
             * REWRITE THIS!
             */

            if ( isAlone(current_predicate) &&
                 (current_predicate.Sign < 0 ) &&
                 (current_predicate.nPos == token.nPos ) &&
                 (current_predicate.nValue != token.nValue) )
            {
                filtered_rules.push_back(rule);
                break;
            }
        }
    }
}

void Idealizer::filterNewApplicableRulesAfterDelition(SToken& token)
{
    filtered_rules.resize(0);
    for (RuleLink rule : rules)
    {
       SToken current_token;
       int rule_length = rule.getRuleLength();
       bool is_new = false;
       bool is_applicable = true;
       long l = 0;

       while (is_applicable && (l < rule_length))
       {
            current_token.nPos = rule[l].Shift;
            current_token.nValue = rule[l].Value;
            current_token.Sign = rule[l].Sign;
            l++;

            if  (current_token.Sign > 0 )
            {
                if (!ideal_object.isBelong(&current_token)) {
                    is_applicable = false;
                }

                if ((current_token.nPos == token.nPos) &&
                    (current_token.nValue != token.nValue)) {
                    is_new = true;
                }
            }

            else if (current_token.Sign < 0)
            {
                if ((current_token.nPos == token.nPos) &&
                    (current_token.nValue == token.nValue)) {
                    is_new = true;
                }

                if (ideal_object.isBelong(&current_token)) {
                    is_applicable = false;
                    break;
                }

                is_applicable = false;
                if (!isAlone(current_token)) {
                    is_applicable = true;
                }
            }
       }

       if (is_new && is_applicable) {
            filtered_rules.push_back(&rule);
       }
    }
}

bool Idealizer::isAlone(SToken & token)
{
    SToken send_token;

    send_token.nPos = token.nPos;
    for (send_token.nValue = 0; send_token.nValue < train_storage.getCodesCount(); send_token.nValue++)
    {
        if ((ideal_object.isBelong(&send_token)) &&
            (send_token.nValue != token.nValue))
        {
            return false;
        }
    }
    return true;
}

double Idealizer::includeTest(SToken &token)
{
    double gamma_change = 0.;
    ideal_object.IncludeT(IncludeToken);
    gamma_change = computeIncludeGammaChange(IncludeToken);
    ideal_object.ExcludeT(IncludeToken);
    return gamma_change;
}

double Idealizer::computeIncludeGammaChange(SToken &token)
{
    double S1, S2, S3;

    filterApplicableRulesByConsequence(token);
    S1 = -2. * computeGammaChangeFromFilteredRules(true);

    filterNewApplicableRulesAfterInsert(token);
    S2 = computeGammaChangeFromFilteredRules(false);

    filterBrokenRulesAfterInsert(TToken);
    S3 = -1. * computeGammaChangeFromFilteredRules(false);
    return (S1 + S2 + S3);
}

void Idealizer::filterNewApplicableRulesAfterInsert(SToken &token)
{
    bool is_token_alone = isAlone(token);
    filtered_rules.resize(0);

    for (RuleLink rule : rules)
    {
        SToken current_token;
        long rule_length = rule.getRuleLength();
        bool is_new = false;
        bool is_applicable = true;
        long l = 0;

        /* make a method for filterint applicable rules7
         * use here and in filterApplicableRules(); function
         */
        while (is_applicable && (l < rule_length)) {
            current_token.nPos = rule[l].Shift;
            current_token.nValue = rule[l].Value;
            current_token.Sign = rule[l].Sign;
            l++;

            if (current_token.Sign > 0) {
                if ((current_token.nPos == token->nPos ) && (current_token.nValue == token->nValue)) {
                        is_new = true;
                }
                if(!ideal_object.isBelong(&current_token)) {
                        is_applicable = false;
                }
            }
            else if (current_token.Sign < 0) {
                if  (ideal_object.isBelong(&current_token)) {
                        is_applicable = false;
                        break;
                }

                if (is_token_alone) {
                    is_applicable = true;
                    if ((current_token.nPos == token->nPos ) && (current_token.nValue != token->nValue)) {
                        is_new = true;
                    }
                }
                else {
                    is_applicable = false;
                }
            }
        }

        if ((is_new) && (is_applicable)) {
            filtered_rules.push_back(&rule);
        }
    }
}

void MyClassificator::filterBrokenRulesAfterInsert(SToken &token)
{
    filtered_rules.resize(0);

    for (RuleLink * rule : applicable_rules)
    {
        long rule_length;
        bool is_old;
        rule_length = rule->getRuleLength();
        is_old = false;
        for (long l = 0; l < rule_length; l++)
        {
            SToken current_predicate;
            current_predicate.nPos = (*rule)[l].Shift;
            current_predicate.nValue = (*rule)[l].Value;
            current_predicate.Sign = (*rule)[l].Sign;

            if ((current_predicate.Sign < 0 ) &&
                (current_predicate.nPos == token.nPos ) &&
                (current_predicate.nValue == token.nValue)) {
                is_old = true;
            }
            else if ((current_predicate.Sign > 0 ) &&
                     (current_predicate.nPos == token.nPos ) &&
                     (current_predicate.nValue != token.nValue)) {
                is_old = true;
            }
        }
        if (is_old)
        {
            filtered_rules.push_back(rule);
        }
    }
}

bool Idealizer::tryApplyBestModification()
{
    double max_gamma_insert = 0, max_gamma_deletion = 0;
    bool is_extremum = true;

    auto max_ex = negative_modifications.rbegin();
    auto max_in = positive_modifications.rbegin();

    if( !negative_modifications.empty() )
        max_gamma_deletion = (*max_ex).first;
    if( !positive_modifications.empty() )
        max_gamma_insert = (*max_in).first;

    auto i1 = negative_modifications.equal_range(max_gamma_deletion);
    auto i2 = positive_modifications.equal_range(max_gamma_insert);


    // Possible cycle here!
    if ( max_gamma_insert < max_gamma_deletion )
    {
        is_extremum = false;
        for(auto j = i1.first; j != i1.second; j++)
        {
            ideal_object.ExcludeT(&(*j).second);
        }

        ideal_object.setGamma(ideal_object.getGamma() + max_gamma_deletion);
    }
    else if ( max_gamma_insert > 0. && max_gamma_insert == max_gamma_deletion )
    {
        is_extremum = false;

        for(GI j = i1.first; j != i1.second; j++)
        {
            ideal_object->ExcludeT(&(*j).second);
        }

        for(GI j = i2.first; j != i2.second; j++)
        {
            ideal_object.IncludeT(&(*j).second);
        }

        ideal_object.setGamma(ideal_object.getGamma() + max_gamma_insert);
    }
    else if ( max_gamma_insert > 0. )
    {
        is_extremum = false;

        for(GI j = i2.first; j != i2.second; j++)
        {
            ideal_object.IncludeT(&(*j).second);
        }

        ideal_object.setGamma(ideal_object.getGamma() + max_gamma_insert);
    }

    filterApplicableRules();

    return is_extremum;
}

void Idealizer::deleteZeroFeatures()
{
    SToken current_token;
    double dCurrGammaIncrease;

    for (current_token.nPos = 0; current_token.nPos < train_storage.getWidth(); current_token.nPos++)
    {
        for (current_token.nValue = 0; current_token.nValue < train_storage.getCodesCount(); current_token.nValue++)
        {
            if (ideal_object.isBelong(current_token))
            {
                dCurrGammaIncrease = computeExcludeGammaChange(current_token);
                if (fabs(dCurrGammaIncrease) == double(0))
                {
                    ideal_object.ExcludeT(&current_token);
                }
            }
        }
    }

    delete current_token;
}

