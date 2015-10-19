#ifndef SDRULEGENERATOR_H
#define SDRULEGENERATOR_H

#include <list>
#include <iostream>

#include "Rule.h"
#include "tst.h"

class SdRuleGenerator
{
private:
    SEQStorage & storage;

    int attributes_count = 0;
    size_t object_count = 0;

    PATTR attributes = nullptr;
    int* data = nullptr;

    PSDEngine engine = nullptr;

    unsigned int hypotesys_count = 0;
    PHYPO hypotesys = nullptr;

    std::list<RuleLink *> rules;

public:
    SdRuleGenerator(SEQStorage & storage) : storage(storage) {
        attributes_count = storage.getWidth();
        object_count = storage.getLength();
    }

    std::list<RuleLink *> generateAllRules(DWORD full_depth, double fisher, DWORD yule_freq, double yule_critlvl) {
        buildAttributes();
        buildDataArray();
        initEngine();
        buildAllHypos(full_depth, fisher, yule_freq, yule_critlvl);

        for (size_t i = 0; i < hypotesys_count; ++i) {
            SDBust(engine, hypotesys + i, &parseReg);
        }

        return rules;
    }

private:
    void buildAttributes();
    void buildDataArray();
    void initEngine();
    void buildAllHypos(DWORD full_depth, double fisher, DWORD yule_freq, double yule_critlvl);

    static void parseReg(PHYPO hyp, PPRED const prs, DWORD hit, DWORD total)
    {
        DWORD cusd = hyp->used_count;
        SdRuleGenerator * gen = static_cast<SdRuleGenerator*>(hyp->thread_arg);

        std::vector<Predicate> predicates;

        for (size_t i = 0; i < cusd - 1; ++i){
            RuleSection p;
            p.Shift = prs[i].par_idx;
            std::cout << prs[i].par_idx << std::endl;
            p.Value = prs[i].scale_val;
            p.Sign = 1;

            predicates.push_back(p);
        }

        Predicate target;
        target.Shift = hyp->conc_idx;
        target.Value = hyp->conc_val;
        target.Sign = 1;

        std::cout << target.Shift << " " << target.Sign << " " << target.Value << std::endl;

        predicates.push_back(target);

        RuleSection * t = new RuleSection(predicates.back());
        UINT * chain = new UINT[predicates.size() + 2];

        chain[0] = predicates.size() - 1;
        chain[1] = 0;
        chain[2] = 0;

        for (size_t i = 0; i < predicates.size() - 1; ++i) {
            chain[i + 3] = gen->storage.GetPredicatePos(predicates[i].Shift,
                                                        predicates[i].Sign,
                                                        predicates[i].Value);
        }

        RuleLink * rule_link = new RuleLink();
        rule_link->setChain(chain);
        rule_link->setTarget(t);

        std::cout << rule_link->getChainStr();
    }
};
#endif // SDRULEGENERATOR_H
