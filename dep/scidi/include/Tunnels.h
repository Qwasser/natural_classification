#ifndef TunnelsH
#define TunnelsH

#include <string>
#include "RulesStorage.h"
#include "SEQStorage.h"

struct RuleSettings
{
	unsigned long StartPos;
	unsigned long FinishPos;
	double FisherSumBound;
	double YuleQUpBound;
	double MinProb;
	bool DeclinePremise;
	bool DeclineDecision;
	unsigned char SearchDepth;
	RulesStorage* RulesContainer;
};

struct ClassificatorSettings
{
    int IdealizType;
    int ObjsSource;
    
    int nRecClass;
    int RecSourcePos;
    int RecSourceNeg;
};

struct ProjectSettings
{
	std::string	chFileName;
	std::string chFilePath;
	int RulesSearchType;
	int IdealizeType;
};

#endif
