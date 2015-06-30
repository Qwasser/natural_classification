#ifndef SequenceH
#define SequenceH
/*---------------------------------------------------------------------------
Store raw representation of objects
---------------------------------------------------------------------------*/
#include <vector>
#include <string>

typedef const char* input_t;

class Sequence
{
public:
		Sequence(std::string Name = "unnamed");
		Sequence(std::vector<std::string> Seq, std::string Name = "unnamed");
		Sequence(input_t* Seq, unsigned long Length, std::string Name = "unnamed");
		virtual ~Sequence();
		
		void Append(std::string token);
		void Append(input_t token);
		void SetName(std::string name){ m_Name = name; };
		long GetLength(){ return m_Length; };
		std::string GetName(){ return m_Name; };
		std::string GetSeq();
		std::vector<std::string> GetArray(){ return m_Seq; };
		std::string& operator[](unsigned long index) { return m_Seq[index]; };
private:
		unsigned long m_Length;
		std::string m_Name;
		std::vector<std::string> m_Seq;
};
#endif
