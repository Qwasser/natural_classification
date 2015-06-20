#include "SEQSElem.h"


SEQSElem::SEQSElem()
{
    m_Seq = 0;
	m_Size = 0;
	m_ClassId = -1;
	last_pos = 0;
}

SEQSElem::SEQSElem(long size)
{
	m_Seq = new code_t[size];
	m_Size = size;
	m_ClassId = -1;
	last_pos = 0;
}

SEQSElem::~SEQSElem()
{
	delete[] m_Seq;
}

#include <assert.h>     /* assert */

code_t& SEQSElem::operator [](unsigned long index)
{
	if(index >= m_Size) assert("seqelem: indexation out of border");
	return m_Seq[index];
}

void SEQSElem::AddValue(code_t v)
{
	if(last_pos < m_Size)
		m_Seq[last_pos++] = v;
}

#include <iostream>
#include <sstream>
std::string SEQSElem::Print()
{
	std::ostringstream output;
	for(unsigned int len = 0; len < m_Size; len++)
		output << m_Seq[len] << " ";
	return output.str();
}
