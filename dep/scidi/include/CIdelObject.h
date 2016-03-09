#ifndef CIdelObjectH
#define CIdelObjectH
//---------------------------------------------------------------------------
#include <string>
#include <list>
#include <functional>
#include "Rule.h"

#define SIZE 32
#define CELL 20
#define MAX_ROW 8
#define DEPTH 8
#define BORDER 5
#define SCALE 5

struct SToken
{
	UINT nPos;
	int nValue;
	char Sign;
};


class SCIDI_API CIdelObject
{
public:
	CIdelObject();
	CIdelObject(const CIdelObject& replicant);
	CIdelObject& operator= (const CIdelObject& c);
	virtual ~CIdelObject();

	//
	bool Create(long lLength, int nCodesCount);
    bool Create(long nLength, int nCodesCount, SEQSElem* InitSeq);
	void Create(SEQSElem* InitSeq, unsigned int n_codes);
	void Create(long lLength, SEQSElem* InitSeq);
	bool Create(long lLength, int nCodesCount, Rule* R);
	//

	bool isIt(int i1, int i2);
	bool isBelong(SToken* Token);
	bool isBelong(RuleSection* Token);
	bool isBelong(unsigned long pos, unsigned long value);
	void setGamma(double dG){ dGammaValue = dG; };
	double getGamma(){ return dGammaValue; };
	int getObjAsStr(char* chObject);
	int getObjAsStr(char* chObject, std::function<input_t (code_t)> f_decode);

	int getValue(int Pos);

	void IncludeT(SToken* Token);
	void ExcludeT(SToken* Token);
	void Minimize();

	std::list<short> RulMarks;
	bool** getObjAsVector(int len, int code);

private:
	bool** IdelObject;
	unsigned long lLength;
	unsigned int nCodesCount;
	double dGammaValue;

};

#endif
