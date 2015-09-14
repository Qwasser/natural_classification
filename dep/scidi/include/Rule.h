//---------------------------------------------------------------------------
#ifndef RuleH
#define RuleH
//---------------------------------------------------------------------------
#include <sstream>
#include <stdio.h>
#include <cstring>
#include "CodeTable.h"
#include "SEQStorage.h"
#include "Constants.h"
#include "setup.h"
#include <iostream>

#define MAXDEFLEN 8

class RuleBase
{
public:
	virtual UINT getRuleLength() = 0;
	virtual RuleSection& Chain(UINT position) = 0;
	virtual RuleSection& operator[](UINT nSecIndex) = 0;
    virtual ~RuleBase() {}
	virtual long getTTPos() = 0;
	virtual int getTTValue() = 0;
	virtual float getCP() = 0;
	virtual char getTTSign() = 0;
	void getChainStr(char* ChainString)
    {
		ChainString[0] = '\0';
        strcpy(ChainString, getChainStr().c_str());
	}
    std::string getChainStr()
    {
        std::ostringstream out;
        CodeTable* Context = CodeTable::Instance();

        for (auto i = 0; i < getRuleLength(); i++)
        {
            Predicate Pi = Chain(i);
            out << (Pi.Shift + 1) << "=" << (Pi.Sign < 0 ? " not " : "") << Context->Decode(Pi.Value);
            out << (i < getRuleLength() - 1 ? " & " : "");
        }

        out << " => ";
        out << (getTTPos() + 1) << "=" << (getTTSign() < 0 ? " not " : "") << Context->Decode(getTTValue());

        std::string conv = out.str();
        if( conv.length() > MYMAXSTR )
            conv = conv.substr(0, MYMAXSTR-1);
        return conv;
    }
};


// ----------------------------------------------------------------------------
// Rule
// ----------------------------------------------------------------------------

class SCIDI_API Rule : public RuleBase
{
public:
	Rule();
	Rule(unsigned long Length);

    Rule(std::string rule_string)
    {
//        int premise_end = rule_string.find("=>");
//        RuleSection t = predFromString(rule_string.substr(premise_end + 2));

//        rule_string = rule_string.substr(0, premise_end);

//        this->TTSign = t.Sign;
//        this->nTTPos = t.Shift;
//        this->Yvalue = t.Value;

//        std::vector<Predicate> predicates;

//        int start_pos = 0;
//        int and_pos = rule_string.find('&');
//        while (and_pos >= 0) {
//            predicates.push_back(predFromString(rule_string.substr(start_pos, and_pos - start_pos)));
//            start_pos = and_pos + 1;
//            and_pos = rule_string.substr(start_pos).find('&');

//            if (and_pos < 0) {
//                break;
//            }

//            and_pos += start_pos;
//        }
//        predicates.push_back(predFromString(rule_string.substr(start_pos)));

//        this->Length = predicates.size();

//        prob_denominator = 0;
//        prob_numerator = 0;

//        chain.resize(10);
//        for(unsigned long i = 0; i < Length; i++)
//        {
//            chain[i] = new Predicate();
//            chain[i]->Shift = predicates[i].Shift;
//            chain[i]->Sign = predicates[i].Sign;
//            chain[i]->Value = predicates[i].Value;
//        }
    }

//    RuleSection predFromString(std::string pred_string) {
//        std::cout << pred_string << std::endl;
//        RuleSection p;
//        std::string not_string = "not";

//        int not_pos = pred_string.find(not_string);
//        int eq_pos = pred_string.find('=');

//        p.Shift = stoi(pred_string.substr(0, eq_pos)) - 1;

//        CodeTable* Context = CodeTable::Instance();

//        std::string val_str;

//        if (not_pos > 0) {
//            p.Sign = -1;
//            val_str = pred_string.substr(not_pos + not_string.length());
//        } else {
//            p.Sign = 1;
//            val_str = pred_string.substr(eq_pos + 1);
//        }

//        size_t first = val_str.find_first_not_of(' ');
//        size_t last = val_str.find_last_not_of(' ');
//        val_str =  val_str.substr(first, (last-first+1));

//        p.Value = Context->getCode(val_str.c_str());

//        return p;
//    }

	Rule(Rule* ptr_DestRule);
	Rule(Rule* ptr_DestRule, RuleSection* TokenStruct);  
    virtual ~Rule();

	bool Minimize(SEQStorage *SeqsStore, double CInt_Value, double CInt2_Value);
	int Probability(SEQStorage *SeqsStore);
	int Criteria(SEQStorage *SeqsStore, double, double);
	void GetFisher(SEQStorage* Data, unsigned long (&m)[2][2], std::vector<int> true_conj, double& fisher, double& yule);

	int getStringInfo(char* , char* );
	std::string PredStr(int PredShift);
	std::string ConclusionStr();
	int getShift(unsigned long ChainIndex) { return Chain(ChainIndex).Shift; }
	void AddSection(RuleSection* TokenStruct);
	
	long getTTPos(){ return nTTPos; };
	void setTTPos(long nTT){ nTTPos = nTT; };
	void setTTValue(int TTVal){ Yvalue = TTVal; };
	int getTTValue(){ return Yvalue; };
	void setTTSign(char Sign){ TTSign = Sign; };
	char getTTSign(){ return TTSign; };
	float getCP(){ 
		if (prob_denominator == 0)
			return 0;
		else
			return (float)((prob_numerator * 10000.) / (prob_denominator * 10001.));
	};
	void setCP(float CP){ 
		prob_denominator = 10000;
		prob_numerator = (UINT)(CP * prob_denominator);
	};
	UINT getRuleLength() { return Length; }
	RuleSection& Chain(UINT position);
	RuleSection& operator[](UINT nSecIndex);

private:
	int init(long Width, SEQStorage* SEQStore);
	void rulecpy(Rule* ptr_DestRule);

	double Faktor(long ifak);
	int FisherCriterion(unsigned long (&m)[2][2], double ZDev, double& clyq1);
	int YuleCriterion(unsigned long (&m)[2][2], double BelInt, double ZDev, double& clyq1);
	void CountDeps(SEQStorage *SeqsStore, unsigned long (&m)[2][2], std::vector<int> TrueToken);
	void ForceCorrelation(SEQStorage *SeqsStore, unsigned long (&m)[2][2], std::vector<int> true_conj = std::vector<int>());

	UINT Length;
	UINT nTTPos;
	char TTSign;
    int Yvalue;
    std::vector<RuleSection *> chain;

public:
	UINT prob_numerator;
	UINT prob_denominator;
	
};
//#pragma pack(pop)


// ----------------------------------------------------------------------------
// RuleChains
// ----------------------------------------------------------------------------
#define CHAIN_BLOCK 1000000
#define RULE_DATA 3

struct MiniRule
{
	UINT Length;
	UINT Numerator;
	UINT Denominator;
	std::vector<UINT> Chain;
};

class RuleChains
{
private:
	unsigned long m_size;//показатель заполнения блока
	UINT m_chains[CHAIN_BLOCK];//блок

public:
	RuleChains() { m_size = 0; }
	~RuleChains() {}

	//Записываем последовательно длину, вероятность и ссылки на предикаты
	// которые определятюся относительно стартового адреса
	int Store(MiniRule* minirule)
	{
		if( m_size == CHAIN_BLOCK )
				return 0;
		for (UINT l = 0; l < minirule->Length + RULE_DATA; l++)
		{
			if( l==0 )
				// mandatory first field - the length of premis
				m_chains[m_size] = minirule->Length;
			else if( l==1 )
				// second and third field : probability stored in intager values
				m_chains[m_size] = minirule->Numerator;
			else if( l==2 )
				m_chains[m_size] = minirule->Denominator;
			else
				// pointers to predicates. pointers saved in Rules storage and gets their beginning from 'start'
				m_chains[m_size] = minirule->Chain[l - RULE_DATA];

			if( m_size ++ == CHAIN_BLOCK )
				return l;
		}
		return -1;
	}
	unsigned long GetLastChain(){ return m_size; }
	UINT* GetChains(){ return &m_chains[0]; }

	//Проверяет, можно ли в данном блоке прочитать последовательность
	// с указанного места (которое должно хранить последующую продолжительность)
	//Возвращает индекс следующей последовательности
	// или 0 - что значит, нужно обратиться к следующему блоку
	unsigned long Load(unsigned long shift)
	{
		if( shift + m_chains[shift] + RULE_DATA >= CHAIN_BLOCK)
			return 0;
		else
			return m_chains[shift] + RULE_DATA;
	}
};


// ----------------------------------------------------------------------------
// RuleLink class

// Imlementation of Rule base with struct placed in fixed blocks
// one by one. Far from list but have smthg similar
// ----------------------------------------------------------------------------
class SCIDI_API RuleLink : public RuleBase
{
private:
	UINT* p_chain;
	Predicate* p_target;
public:
	RuleLink(){ p_chain = 0; p_target = 0; }
	void setChain(UINT* c){ p_chain = c; }
	void setTarget(RuleSection* t){ p_target = t; }
	UINT getRuleLength(){
		return p_chain[0];
	}
	
	RuleSection& Chain(UINT position)
	{
		//в rulechain сместившись на position хранится смещение относительно ppred
		UINT rs_shift = p_chain[RULE_DATA + position]; 
		return CodeTable::GetPredicate(rs_shift);
	}
	RuleSection& operator[](UINT nSecIndex){ return Chain(nSecIndex); }
	long getTTPos(){ return p_target->Shift; }
	int getTTValue(){ return p_target->Value; }
	char getTTSign(){ return p_target->Sign; }
	float getCP(){
		UINT prob_numerator = p_chain[1]; // *((UINT*)((UINT)p_chain - 2 * sizeof(UINT)));
		UINT prob_denominator = p_chain[2]; // *((UINT*)((UINT)p_chain - sizeof(UINT)));
		if (prob_denominator == 0)
			return .0;
		else
			return (float)((prob_numerator * 10000.) / (prob_denominator * 10001.));
	}
};


extern "C" double InverseNormalCDF(double alpha);


#endif

