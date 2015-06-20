//---------------------------------------------------------------------------
#ifndef NaiveRuleGeneratorH
#define NaiveRuleGeneratorH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <map>
#include "RuleManager.h"
#include "RulesStorage.h"
#include "Tunnels.h"

class SCIDI_API NaiveRuleGenerator : public RuleManager
{
public:
	NaiveRuleGenerator();
	virtual ~NaiveRuleGenerator();
	
	void SetInput(Sequence* input, long size);
	void SetInput(SEQStorage* s);
	void ParseStorage();
	Rule* GetOutput();
	unsigned long GetOutputSize(){ return FinishRulesStorage->getSize(); };
	void GenRules(RuleSettings p);
	void SetSettings(RuleSettings p);
	void GenRules(void);

private:
	int Create(SEQStorage* mySEQStorage, RulesStorage* FinishStorage,
					double ConfInterval_Value,  double Yule, double CProb_MinLevel);
	void SelectCells(Rule* ptr_Rule);
	bool NextIter(Rule* ptr_Rule);
	int StartNextIter(long s_TTPos, long f_TTPos);
	int ShiftBusy(bool* ptr_myBusyVector, Rule* ptr_Rule);
	void AddRule(Rule* ptr_Rule, int Shift, int Sign, int SendToken);
	bool isUnique(Rule* ptr_Rule);
	void BaseRun(Rule* ptr_Rule);

	SEQStorage* ptr_mySEQStorage;//pointer
	double Conf_Interval;
	double Conf_Interval2;
	int Base_run;
	bool leftNeg;
	bool rightNeg;
	unsigned long StartPos;
	unsigned long FinishPos;

	int nCodesCount;
	
	Rule* StartRule;
	UINT tree_id;
	RulesStorage* FinishRulesStorage;
//	RulesStorage* PreRulesBuffer;//all rules
 //	RulesStorage* PostRulesBuffer;
	long sz_mySEQStorage;
	
	//std::map<int,double> NewToken;
	typedef std::map<int,double>::const_iterator CI;
	double CP_minLEVEL;
	
	char FValStr[32];
	char YValStr[32];
	char CPValStr[32];
	bool Changed;

	long nWidth;
};

#endif
