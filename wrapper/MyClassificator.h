//---------------------------------------------------------------------------

#ifndef ClassificatorH
#define ClassificatorH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "CIdelObject.h"
#include "RulesStorage.h"
#include "Reporter.h"
#include "Thread.h"

class ObjReporter :
	public EmbedReporter
{
public:
	void ReportNewObject(void) { Report()->signal("object"); };
	void ReportLog(std::string message) {
		Report()->SetArg("log", message);
		Report()->signal("log");
	};
};


class SCIDI_API MyClassificator : public ObjReporter
{
public:
    MyClassificator();
    MyClassificator(int IType);
    virtual ~MyClassificator();

	void Restore(SEQStorage* pSeqStorage, RulesStorage* pRulesStorage, CIdelObject** ObjsStorage, long SCounter);
	bool Create(SEQStorage* pSeqStorage, RulesStorage* pRulesStorage, CIdelObject** ObjsStorage);
	
	void SetSource(int src){ m_source = src; }
	void SetType(int type){ IdealizType = type; }
    void WorkUpData();
    void GenClasses();
	long GetOutputSize() {
		return lObjsStorageCounter;
	}
	long NextIter(SEQSElem* NextSeq, int NextRuleNumber);
	long NextIter(SEQSElem* NextSeq);
	long NextIter(Rule* NextRule);
	long OneIter(SEQSElem* NextSeq, long IterCounter);
	long OneIter(Rule* NextReg, long IterCounter);

	void OneIter1(CIdelObject* Pattern);
	bool OneIter2(CIdelObject* Pattern);
	bool One1Iter(SEQSElem* Pattern);
	double getGamma()
	{ 
		if(CurrIdelObject==NULL)
			return 0;
		else
			return CurrIdelObject->getGamma();
	}
	void getObjAsStr(SEQSElem* Pattern, char* chObject);

	double CountF(SEQSElem* NextSeq,  int nSeqNumb, int nClNumber );
	int DefineRule( int nRuleNumber, int nClNumber);

	void CountMatrix(int nClNumber);
	void CountMatrix(CIdelObject* Cl);
	void Pred_VMatrixVal(long pos, int cod, double& val );
	double Get_VMatrixVal(long pos, int cod );
	void Rec_VMatrixVal(long pos, int cod, double& val );
	double Count_GammaCryterion( );

	//viz feature
	void FindMax(CIdelObject* ideal, SToken& ct, bool &in, bool &ex);
	void Insider(CIdelObject* ideal, SToken& ct, bool &in, bool &ex);
	std::vector<std::string> GetWRules(CIdelObject* ideal, SToken s1);
	std::vector<std::string> GetAplicableRules();
	CodeTable* GetCodeTable(){ return CodeTable::Instance(); }
	long GetWidth(){ return pSeqStorage->getWidth(); }//capacity of features
	long lObjsStorageCounter;
	bool bObjIsUnique;
	char* chObject;

	unsigned long m_Thread;
	std::multimap<double, SToken> I_negative, I_positive;
	typedef std::multimap<double, SToken>::iterator GI;
	typedef std::multimap<double, SToken>::reverse_iterator rGI;
	char* InitObj, *CurrObj;
	double** VMatrix;
	std::string AllRules;
	
	void setCommand(ThreadCommand* c) { CheckTermination = c; };
	bool IsClassificatorTerminated() { return CheckTermination->Execute(); };
private:
	static int id;
	ThreadCommand* CheckTermination;
	std::ostringstream m_progress;

	long lSeqStoreSize;
	SEQStorage* pSeqStorage;

	long lRuleStoreSize;
	RulesStorage* pRulesStorage;
	std::vector<RuleSection> FiltrTargets;

	CIdelObject** ObjsStorage;
//	long lObjsStorageCounter;
    int m_source;
	SEQSElem* m_NextSeq;

	long FilterSize;
	std::vector<RuleLink*> p_AplicReg;
	long FilterSize2;
	std::vector<RuleLink*> p_SubsetRules;

	//bool isBelong(CIdelObject* obj, SToken* Token);
	double ProbFunc(double x);
	void DiscountRule(RuleLink* pRule, RuleSection &prev);
	int FillVMatrix();
	long Filtrovka();
	void SetObviousPred();
	

	void DrawRules();
	std::ofstream File;
	std::ostringstream tmpFile;
	void PrintRules(Rule* Massiv, long Size, std::streambuf *File);
	std::vector<std::string> RulesToStrArr(Rule* Massiv, long Size);
	std::vector<std::string> RulesToStrArr(std::vector<RuleLink*>& Massiv, long Size);

	double PartialSum();
	double FeatureSum();
	long Filtr_Premis(SToken* Stok);

	long Filtr_Del1(SToken* Stok);
	long Filtr_Del2(SToken* Stok);
	long Filtr_Del4(SToken* Stok);

	long Filtr_Ins2(SToken* Stok);
	long Filtr_Ins3(SToken* Stok);
	//long DelSubRules(Rule*& Array, int size);
	//long DelSubRules(std::vector<Rule*>& Array, int size);
	

	unsigned long lSeqLength;
	int nCodesCount;

	int IdealizType;
	CIdelObject* CurrIdelObject;

        bool IsAlone(SToken* TToken);
        bool IsAloneIdOb(SToken* TToken, int nClNumber);

	double IncludeTest(SToken* IncludeToken);
	double ExcludeTest(SToken* ExcludeToken);
	std::vector<std::string> IncludeTest(CIdelObject* ideal, SToken s1);
	std::vector<std::string> ExcludeTest(CIdelObject* ideal, SToken s2);

	double IncRulesTotalSum(SToken* TToken );
	double ExcRulesTotalSum(SToken* TToken);

	SToken m_Conflict;
	void SearchForAction(CIdelObject* i_Object);
	bool ActionExtremum(CIdelObject* i_Object);
	void DelZeroFeature(CIdelObject* i_Object);
	void InsFDelABuild();
	void DelFInsABuild();
	void DelInsBuild();
	void MaxDelInsBuild();
	void OfficialBuild();
	

	bool ObviousPred(SToken vij);

	long lIterCounter;
};

//---------------------------------------------------------------------------

#endif

