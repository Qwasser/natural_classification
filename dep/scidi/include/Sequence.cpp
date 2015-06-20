#include "Sequence.h"

Sequence::Sequence(std::string Name)
{
	m_Length = 0;
	m_Name = Name;
}

Sequence::Sequence(std::vector<std::string> Seq, std::string Name)
{
	m_Seq = Seq;
	m_Name = Name;
}

Sequence::Sequence(input_t* Seq, unsigned long Length, std::string Name)
{
	m_Seq = std::vector<std::string>(Length);
	for(unsigned long i = 0; i < Length; i++)
		m_Seq.push_back(std::string(Seq[i]));
	m_Length = Length;
	m_Name = Name;
}

Sequence::~Sequence()
{
}

void Sequence::Append(input_t token)
{
	m_Seq.push_back(token);
	m_Length++;
}

void Sequence::Append(std::string token)
{
	m_Seq.push_back(token);
	m_Length++;
}

std::string Sequence::GetSeq()
{
	std::string seq;
	for(std::vector<std::string>::iterator i = m_Seq.begin(); i != m_Seq.end(); i++)
		seq += *i + " ";
	return seq.substr(0, seq.length() - 1);
}
