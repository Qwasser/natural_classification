#include "sdrulegenerator.h"

#include "Rule.h"
#include <dep/sd/include/tst.h>

#include <iostream>
#include <unordered_set>
#include <algorithm>

void SdRuleGenerator::buildAttributes() {
    std::unordered_set<int> levels_set;

    this->attributes = new ATTR [attributes_count];

    for (size_t i = 0; i < attributes_count; ++i) {
        levels_set.clear();
        for (size_t obj_num = 0; obj_num < storage.getLength(); ++obj_num) {
            int elem_code = 0;
            storage.getElem_c(obj_num, i, elem_code);
            levels_set.insert(elem_code);
        }

        // revert to dummy levels
        size_t level_count = levels_set.size();
        int * levels = new int [level_count];

        size_t current_level = 0;
        for (int level_code : levels_set) {
            levels[current_level] = level_code;
            ++current_level;
        }

        std::sort(levels, levels + level_count);

        PATTR attr = &(attributes[i]);

        attr->name = (char*) malloc(6);
        attr->name = "test";

        attr->bordersz = sizeof(int);
        attr->objoffset = i * sizeof(int);

        attr->atype = 0;
        attr->borders = levels;
        attr->bnum = level_count - 1;
        attr->is_addrbased = 0;

        attr->cmpf = &cmp_int;

         *((DWORD*) attr->free_to_use) = 0xC0DE;
    }
}

void SdRuleGenerator::buildDataArray() {
    this->data = new int [this->attributes_count * this->object_count];
    for (size_t attr_num = 0; attr_num < attributes_count; ++attr_num) {
        for (size_t obj_num = 0; obj_num < object_count; ++obj_num) {
            int element_code = 0;
            storage.getElem_c(obj_num, attr_num, element_code);
            data[obj_num * attributes_count + attr_num] = element_code;
        }
    }
}

void SdRuleGenerator::initEngine() {
    engine = SDInitEngine(attributes, attributes_count);
    SDInitRawObjects(engine, data, attributes_count * sizeof(int), object_count);
}

void SdRuleGenerator::buildAllHypos(DWORD full_depth, double fisher, DWORD yule_freq, double yule_critlvl) {
    size_t conclusion_count = 0;
    for (size_t i = 0; i < attributes_count; ++i) {
        conclusion_count += attributes[i].bnum + 1;
    }

    PPRED concls = (PPRED) malloc(conclusion_count * sizeof(PRED));

    size_t conclusion_id = 0;
    for (size_t i = 0; i < attributes_count; ++i) {
        for (size_t level_id = 0; level_id <= attributes[i].bnum; ++level_id) {
            concls[conclusion_id].scale_val = level_id;
            concls[conclusion_id].par_idx = i;
            ++conclusion_id;
        }
    }


    hypotesys = SDPenetratedYuleOneD(engine,
                                     concls,
                                     conclusion_count,
                                     full_depth,
                                     fisher,
                                     &hypotesys_count,
                                     yule_freq,
                                     yule_critlvl);

    // Assign pointer to generator in all hypotesys to access generator in hypotesys found callback
    for (size_t i = 0; i < hypotesys_count; ++i) {
        hypotesys[i].thread_arg = this;
    }

    std::cout << "conclusion_count = " << conclusion_count << std::endl;
    std::cout << "hypotesys_count = " << hypotesys_count << std::endl;
}
