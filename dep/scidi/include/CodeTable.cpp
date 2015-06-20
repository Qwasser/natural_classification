//---------------------------------------------------------------------------
#include "CodeTable.h"
//---------------------------------------------------------------------------
CodeTable::CodeTable()
{
		CodesCount = 0;
		EmptiesCount = 0;
		Tokens.clear();
		Codes.clear();
}
//---------------------------------------------------------------------------
CodeTable::~CodeTable()
{
		Tokens.clear();
		Codes.clear();
}
//---------------------------------------------------------------------------
CodeTable* CodeTable::m_instance = 0;

unsigned int CodeTable::CodesCount = 0;
std::vector<input_t> CodeTable::Tokens;
std::vector<code_t> CodeTable::Codes;
PredicateArray CodeTable::ppred = 0;

CodeTable* CodeTable::Instance()
{
	if(m_instance == 0){
		m_instance = new CodeTable;
	}
	return m_instance;
}

PredicateArray CodeTable::InitPredicates(unsigned long n, unsigned long alf)
{
	if (CodesCount == 0) return 0;
	if (alf == 0) alf = CodesCount;

	ppred = new Predicate[2 * alf*n];
	for (unsigned long j = 0; j<n; j++)
		for (unsigned long i = 0; i<alf; i++)
		{
		ppred[2 * j*alf + i].Shift = j;
		ppred[2 * j*alf + i].Sign = 1;
		ppred[2 * j*alf + i].Value = i;

		ppred[(2 * j + 1)*alf + i].Shift = j;
		ppred[(2 * j + 1)*alf + i].Sign = -1;
		ppred[(2 * j + 1)*alf + i].Value = i;
		}
	
	return ppred;
}

//Процедура создает код для кодирования последовательностей
void CodeTable::Parse(input_t* SeqsArray, long SeqsArraySize)
{
	for (long ArrayIndex = 0; ArrayIndex < SeqsArraySize; ArrayIndex++)
		Code(SeqsArray[ArrayIndex]);
}

void CodeTable::Parse(std::vector<input_t> SeqsArray)
{
	for (std::vector<input_t>::size_type ArrayIndex = 0; ArrayIndex < SeqsArray.size(); ArrayIndex++)
		Code(SeqsArray[ArrayIndex]);
}

void CodeTable::Parse(Sequence* s, SEQSElem &e)
{
	for (unsigned int ArrayIndex = 0; ArrayIndex < s->GetLength(); ArrayIndex++)
	{
		std::string value = (*s)[ArrayIndex];
		char* c_value = new char[value.length()+1];
		strcpy(c_value, value.c_str());
		e.AddValue(Code(c_value));
	}
}
//---------------------------------------------------------------------------
//Процедура определяет новый это символ или нет, нужен ли под нее код или уже имеется
#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

code_t CodeTable::Code(input_t CurrChar)
{
	for (unsigned TableIndex = 0; TableIndex < CodesCount; TableIndex++)
	{
		if(stricmp(Tokens[TableIndex], CurrChar) == 0)
			return Codes[TableIndex];
	}
	Tokens.push_back(CurrChar);
	Codes.push_back(CodesCount);
	CodesCount++;
	return CodesCount-1;
}

//---------------------------------------------------------------------------
//Выдает код по символу
code_t CodeTable::getCode(input_t Token)
{
		for (unsigned TableIndex=0; TableIndex < CodesCount+EmptiesCount; TableIndex++)
		{
			if(stricmp(Tokens[TableIndex], Token) == 0)
			{
				return Codes[TableIndex];
			}
		}
		return -1000;
}

//---------------------------------------------------------------------------
//Количество различных симоволов (мощность алфавита)
unsigned int CodeTable::getCodesCount()
{
	return CodesCount;
}
//---------------------------------------------------------------------------
input_t CodeTable::setGaps(short flag)
{
	EmptiesCount = flag;
	CodesCount--;
	return Decode( CodesCount );
}
//---------------------------------------------------------------------------
short CodeTable::hasLacks()
{
	return EmptiesCount;
}
//---------------------------------------------------------------------------
//Возвращает символ по коду
input_t CodeTable::Decode(code_t Code)
{
	if ( Code < 0 || (unsigned)Code >= CodesCount+EmptiesCount )
	{
			return '\0';
	}

	for (unsigned TableIndex = 0; TableIndex < this->CodesCount+EmptiesCount; TableIndex++)
	{
		if ( Codes[TableIndex] == Code )
		{
			return Tokens[TableIndex];
		}
	}
	return '\0';
}
//---------------------------------------------------------------------------

