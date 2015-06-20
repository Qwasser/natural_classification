#ifndef SEQSElemH
#define SEQSElemH
/*---------------------------------------------------------------------------
When a raw object has been parsed store values in this class
Store coded and separated values of object
Also store class id
---------------------------------------------------------------------------*/
#include "../setup.h"
#include <string.h>
#include <string>

class SCIDI_API SEQSElem
{
public:
	SEQSElem();
	SEQSElem(long size);
	//copy semantics for std::vector
	SEQSElem(const SEQSElem& S) : m_Size(S.m_Size), m_ClassId(S.m_ClassId), last_pos(S.last_pos)
	{
		m_Seq = new code_t[S.m_Size];
		memcpy(m_Seq, S.m_Seq, sizeof(code_t) * S.m_Size); //for(unsigned long i = 0; i < S.m_Size; i++) m_Seq[i] = S.m_Seq[i];
	};
	virtual ~SEQSElem();

	void AddValue(code_t v);
	long GetSize(){ return m_Size; };
	code_t& operator[](unsigned long Index);
	std::string Print();
	long GetClassId(){ return m_ClassId; };
	void SetClassId(long id){ m_ClassId = id; };
	void IncClassId(){ m_ClassId++; };
	void DecClassId(){ m_ClassId--; };
private:
	code_t* m_Seq;
	long m_Size;
	long m_ClassId;
	long last_pos;

};

#endif
