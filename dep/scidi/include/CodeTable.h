#ifndef CodeTableH
#define CodeTableH

#include <vector>
#include "Sequence.h"
#include "SEQSElem.h"

typedef unsigned int UINT;

// ----------------------------------------------------------------------------
// RuleSection
// ----------------------------------------------------------------------------
class RuleSection
{
public:
	UINT Shift;	// Смещение признака относительно целевого
	char Sign;	// Знак отклонения частоты встречаемости Y
	int Value;	// Значение признака (A=1,T=2,G=3,C=4)

	bool operator== (RuleSection y)
	{
		if (this->Shift == y.Shift &&
			this->Sign == y.Sign &&
			this->Value == y.Value)
			return true;
		else
			return false;
    }

	bool operator!= (RuleSection y)
	{
		if (this->Shift != y.Shift ||
			this->Sign != y.Sign ||
			this->Value != y.Value)
			return true;
		else
			return false;
	};

    inline bool operator<(const RuleSection& rhs) const {
      if (this->Shift < rhs.Shift) {
        return true;
      }
      else if (this->Sign < rhs.Sign) {
          return true;
      }
      else if (this->Value < rhs.Value) {
          return true;
      }
      return false;
	}
};

typedef RuleSection Predicate;
typedef Predicate* PredicateArray;
// ----------------------------------------------------------------------------
// TargetSection
// ----------------------------------------------------------------------------
class TargetSection
{
public:
	UINT Shift;	// Смещение признака относительно целевого
	int Value;	// Значение признака (A=1,T=2,G=3,C=4)

	bool operator== (TargetSection y)
	{
		if (this->Shift == y.Shift &&
			this->Value == y.Value)
			return true;
		else
			return false;
	};
	bool operator!= (TargetSection y)
	{
		if (this->Shift != y.Shift ||
			this->Value != y.Value)
			return true;
		else
			return false;
	};
};


class SCIDI_API CodeTable
{
protected:
	CodeTable();
	virtual ~CodeTable();
public:
	static CodeTable* Instance();
	
	PredicateArray InitPredicates(unsigned long n, unsigned long alf = 0);
	static Predicate& GetPredicate(UINT id) { return ppred[id]; };

	void Parse(input_t* SeqsArray, long SeqsArraySize);
	void Parse(std::vector<input_t> SeqsArray);
	void Parse(Sequence* s, SEQSElem &e);

	short hasLacks();
	input_t setGaps(short flag);
	
	unsigned int getCodesCount();
	
	code_t getCode(input_t Token);
	input_t Decode(code_t Code);
private:
	code_t Code(input_t CurrChar);

	static PredicateArray ppred;
	static std::vector<input_t> Tokens;
	static std::vector<code_t> Codes;
	static unsigned int CodesCount;
	short EmptiesCount;
	static CodeTable* m_instance;
};

#endif
