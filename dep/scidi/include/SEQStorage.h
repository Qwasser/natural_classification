// SEQStorage.h: interface for the SEQStorage class.
//
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef SEQStorageH
#define SEQStorageH
//---------------------------------------------------------------------------
#include <math.h>
#include <vector>
#include "Sequence.h"
#include "SEQSElem.h"
#include "CodeTable.h"

typedef SEQSElem c_object; /* coded object*/
typedef SEQSElem* p_object; /* pointer on a object*/
typedef std::vector<c_object> c_objects; /* coded objects*/
typedef c_objects* p_objects; /* pointer on coded objects*/

class SEQStorage
{
public:
	SEQStorage();
	SEQStorage(Sequence* SeqsArray, long SeqsArraySize);
	SEQStorage(const Sequence* SeqsArray, long SeqsArraySize, c_objects Storage);
	SEQStorage(char* arr, unsigned long nWidth_, unsigned long nLength_);
	virtual ~SEQStorage();

	// use singleton Code table as ancestor
	void Parse(Sequence* s, SEQSElem &e){ CodeTable::Instance()->Parse(s, e); };
	short hasLacks() { return CodeTable::Instance()->hasLacks(); };
	input_t setGaps(short flag){ return CodeTable::Instance()->setGaps(flag); };
	unsigned int getCodesCount(){ return CodeTable::Instance()->getCodesCount(); };
	code_t getCode(input_t Token){ return CodeTable::Instance()->getCode(Token); };
	input_t Decode(code_t Code){ return CodeTable::Instance()->Decode(Code); };
    UINT GetPredicatePos(int Shift, int Sign, int SendToken)
    {
        return (2*Shift + ((1-Sign)/2))*this->getCodesCount() + SendToken;
    }

	int putSEQ(const char* SourceSeq);

	void Add(const Sequence* a, long size, std::string name){};
	c_objects* GetByName(std::string name);

	void RemoveNewStorage(bool CtrlPos = false){}; //deprecated!

    bool getElem(int oj, int xi, int &el);
	bool getElem_c(int oj, int xi, int &el);

	void CreateElem(unsigned from, SEQSElem *dest);
	unsigned long getLength() { return nLength; }
	unsigned long getItemsCount() { return nLength; }
	unsigned long getWidth() { return nWidth; }
	unsigned long getObjectWidth() { return nWidth; }
	void setLength(long lLength) { this->nLength = lLength; }
	void setWidth(int nWidth) { this->nWidth = nWidth; }
	double getFreq(long n, int a);
	std::string printSeq(p_object);
	input_t printSeq(c_objects::size_type i, char * c_string);
	input_t printSeq_c(c_objects::size_type i, char * output);
	std::vector<bool> getOneExecute(int Shift, int Sign, int SendToken);

	void ClearClasses();
	void setClass(int id, int nCL);
	int getClass(int id);
	void CountFrequencies(unsigned long size);
	void Conclude();

	//! keep here, but remove in the future
	c_objects LrnNegStorage;
	c_objects CtrlPosStorage;
	c_objects NewStorage;
	unsigned long NewStoreLength;
	unsigned long LrnNegStoreLength;
	unsigned long CtrlPosStoreLength;
private:
	void CountExecutable();

	unsigned long nLength;
	unsigned long nWidth;

	c_objects Storage;
	
	unsigned int nCodesCount;

	std::vector< std::vector<bool> > ExecutObj;
	std::vector< std::vector<double> > TTFrequncies;

	int* m_classes;
};

/*
namespace Sequences {
	void Convert(c_objects a, char* b)
	{
		
	};
}
*/

#endif
