#include "sdrulegenerator.h"

#include "Rule.h"
#include <dep/sd/include/tst.h>

#include <iostream>

void SdRuleGenerator::buildAttributes() {
    int code_count = storage.getCodesCount();
    int * levels = new int[code_count];

    for (int i = 0; i < code_count; ++i) {
        levels[i] = i;
    }

    this->attributes = new ATTR [attributes_count];

    for (size_t i = 0; i < attributes_count; ++i) {
        PATTR attr = &(attributes[i]);

        attr->name = (char*) malloc(6);
        attr->name = "test";

        attr->bordersz = sizeof(int);
        attr->objoffset = i * sizeof(int);

        attr->atype = 0;
        attr->borders = levels;
        attr->bnum = code_count - 1;
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
    PPRED concls = (PPRED) malloc(attributes_count * (storage.getCodesCount()) * sizeof(PRED));

    for (size_t i = 0; i < attributes_count; ++i) {
        for (size_t j = 0; j < storage.getCodesCount(); ++j) {
            concls[j + i * (storage.getCodesCount())].scale_val = j;
            concls[j + i * (storage.getCodesCount())].par_idx = i;
        }
    }


    hypotesys = SDPenetratedYuleOneD(engine,
                                     concls,
                                     attributes_count * storage.getCodesCount(),
                                     full_depth,
                                     fisher,
                                     &hypotesys_count,
                                     yule_freq,
                                     yule_critlvl);

    // Assign pointer to generator in all hypotesys to access generator in hypotesys found callback
    for (size_t i = 0; i < hypotesys_count; ++i) {
        hypotesys[i].thread_arg = this;
    }

}
