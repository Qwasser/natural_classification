#ifndef SEQSElemH
#define SEQSElemH
/*---------------------------------------------------------------------------
When a raw object has been parsed store values in this class
Store coded and separated values of object
Also store class id
---------------------------------------------------------------------------*/
#include "setup.h"
#include <string.h>
#include <string>
#include <vector>

class SCIDI_API SEQSElem
{
public:
	SEQSElem();
	SEQSElem(long size);

	SEQSElem(const SEQSElem& S) : m_ClassId(S.m_ClassId), 
                      m_Seq(S.m_Seq) {}

	virtual ~SEQSElem();

	void AddValue(code_t v);
    long GetSize(){ return m_Seq.size();}

	code_t& operator[](unsigned long Index);

	std::string Print();

    long GetClassId(){ return m_ClassId; }
    void SetClassId(long id){ m_ClassId = id; }
    void IncClassId(){ m_ClassId++; }
    void DecClassId(){ m_ClassId--; }
private:
	std::vector<code_t> m_Seq;
	long m_ClassId;
};

#endif
