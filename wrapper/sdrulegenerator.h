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

    std::list<Rule *> rules;

public:
    SdRuleGenerator(SEQStorage & storage) : storage(storage) {
        attributes_count = storage.getWidth();
        object_count = storage.getLength();
    }

    std::list<Rule *> generateAllRules(DWORD full_depth, double fisher, DWORD yule_freq, double yule_critlvl) {

        buildAttributes();
        std::cout << "attrs built" << std::endl;
        buildDataArray();
        std::cout << "attrray built" << std::endl;
        initEngine();
        std::cout << "engine done" << std::endl;
        buildAllHypos(full_depth, fisher, yule_freq, yule_critlvl);
        std::cout << "hypotesys done" << std::endl;

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

        for (size_t i = 0; i < cusd - 1; ++i){
            std::cout << prs[i].par_idx << " : " << prs[i].scale_val << "; " ;
        }

        std::cout << std::endl;
        std::cout << "Last prob: " << hyp->last_prob << std::endl;
        std::cout << "Hit: " << hit << std::endl;
        std::cout << "Total: " << total << std::endl;
    }
};



#endif // SDRULEGENERATOR_H
