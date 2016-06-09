#include <iostream>
#include "Reporter.h"

class ScidiLogger : public Reporter
{
public:
    ScidiLogger() {}
    virtual ~ScidiLogger() {}

    void NewRegularity(Rule* rule) {
        if (rule_count % 100 == 0) {
            std::cout <<  "Found " << rule_count << " rules" << std::endl;
        }
        rule_count += 1;
    }

    void PrintLog(std::string message) {}
    void UpdateStatus(std::string message) {}
    void NewObject(std::string object, double rating) {}

    void DataProcessCommand() {}
    void RegProcessCommand() {}
private:
    long rule_count = 0;
	FILE* _log;
};
