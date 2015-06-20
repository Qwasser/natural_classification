//---------------------------------------------------------------------------

#ifndef RulesStorageH
#define RulesStorageH
//---------------------------------------------------------------------------
#include <list>
#include "Rule.h"
#include <boost/functional/hash.hpp>

typedef std::list<RuleChains*>::iterator blocksID;
typedef std::list<RuleLink>::iterator ruleID;
typedef std::list<RuleLink>::reverse_iterator rulerevID;

#define ONE_STEP_SIZE 100000
#define TABLE_LIMIT 65000

// ----------------------------------------------------------------------------
// OneTargetTree
// ----------------------------------------------------------------------------
class OneTargetTree
{
private:
	UINT m_target;//что
	UINT Volume;//сколько
	unsigned long m_chains;//начиная откуда (нумерация общая для всех блоков - чтобы не было еще ) 
public:
	OneTargetTree()
	{
		m_target = 0; 
		Volume = 0;
		m_chains = 0;
	}
	//~OneTargetTree();//{ if( m_chains!=0 ) delete m_chains; }
	void SetTarget(UINT t){ m_target = t; }
	OneTargetTree& operator++ (){ Volume++; return *this; }
	OneTargetTree& operator++ (int){ Volume++; return *this;}
	void ConnectChain(unsigned long p){ m_chains = p; }
	UINT GetTarget(){ return m_target; }
	unsigned long GetChains(){ return m_chains; }
	UINT GetVolume(){ return Volume; }
};


// ----------------------------------------------------------------------------
// RulesStorage
// ----------------------------------------------------------------------------
class SCIDI_API RulesStorage
{
	unsigned long Charset;
	unsigned long Attrset;
	// part of 'mini rules' plugin (in plan)
	UINT tree_id;
	PredicateArray ppred;
	OneTargetTree* CBBTree;
	std::list<RuleChains*> m_blocks;
	// --

	unsigned long sz_Storage;
	std::list<RuleLink> Storage;

	ruleID currentRule;
	unsigned long currentRule_id;
	std::vector<std::size_t> hash_array;
	boost::hash<std::string> string_hash;
	
	bool DefSubRules(RuleLink* r1, RuleLink* r2);
	long DelSubRules(std::list<RuleLink>& Array, int size);//private
	void AddRuleHash(Rule* R);

public:
	RulesStorage();
	RulesStorage(FILE* arh);
	RulesStorage(unsigned long n, unsigned long alf);
	virtual ~RulesStorage();
	
	// part of 'mini rules' plugin (in plan) for using ppred
	RuleSection* GetPredicatePointer(int Shift, int Sign, int SendToken);
	UINT GetPredicatePos(int Shift, int Sign, int SendToken);
	UINT BlocksSize(){ return CHAIN_BLOCK; }
	UINT BlocksCount(){ return m_blocks.size(); }
	void MakePointersArray();
	std::list<RuleLink>* GetLinks(){ return &Storage; }
	Rule* ConvertFromLinkToRule(RuleLink* R);
	OneTargetTree* SetTargetInTree(UINT Id, UINT x)
	{
		if( Id < 2*Charset*Attrset ) 
		{
			CBBTree[Id].SetTarget( x ); 
			return &CBBTree[Id];
		}
		return 0;
	}
	UINT GetTargetMemoryShift(UINT Shift){ return Shift; }
	RuleSection* GetTargetByShift(UINT Shift){ return &ppred[Shift]; }
	// methods for inserting new rule
	void AddCorrectRule(Rule* ptr_Rule);
	UINT AddTail(Rule* ptr_Rule);
	void AddHead(UINT Id, unsigned long pChain);
	void UpdateHead( UINT Id ){ CBBTree[Id]++; }
	std::vector<UINT> PackTail(Rule* ptr_Rule);

	// --
	UINT GetHeadShift() {
		if (tree_id == -1) return 0;
		RuleSection* target_p = GetTargetByShift(CBBTree[tree_id].GetTarget());
		return target_p->Shift;
	};

	void Save(FILE* arh);
	void Load(Rule** Ref, long Size);
	Rule* c_array();

	std::list<Rule*>::allocator_type getAllocType(){ return Storage.get_allocator(); };
	unsigned long getSize() { return( Storage.size() /*sz_Storage*/); }
	unsigned long getActualSize() { return sz_Storage; }
	ruleID begin(){ return Storage.begin(); }
	ruleID end(){ return Storage.end(); }
	rulerevID rbegin(){ return Storage.rbegin(); }
	rulerevID rend(){ return Storage.rend(); }
	Rule* remove(long RuleIndex);
	void Clear();
	
	int setRuleClass(long Index, long nClass);
	long getRuleClass(long Index){ return(-1); }

	bool Add(Rule* R);
	bool isFind(Rule* ptr_Rule);
	RuleLink& operator[](UINT lStoreIndex);
	void DelSubRules();//public
	void CalcAgain(SEQStorage* Data, double ConfInterval, double ConfInterval2);//public

};

class StorageSorter : public RulesStorage
{
	std::vector<int> Sort;
	std::vector<int> FisherSort();
	std::vector<int> TargetSort();
	std::vector<int> ProbSort();
	//---------------------------------------------------------------------------
	// TODO: write methods something like below code based on commented chunk
	std::vector<int> DefaultSort()
	{
		Sort.resize(getSize());
		for ( int i = 0; i < getSize(); i++ )
		{
			Sort[i] = i;
		}
		return Sort;
	}
//---------------------------------------------------------------------------
//
//std::vector<int> RulesStorage::ProbSort()
//{
//	Sort.resize(sz_Storage);
//	for ( int i = 0; i < sz_Storage; i++ )
//	{
//		Sort[i] = i;
//	}
//
///*
//*	fisher by bubble sort
//*/
//
//	int temp;
//	bool t=true;
//	while(t)
//	{
//		t=false;
//		for ( int j = 0; j < sz_Storage-1; j++ )
//		{
//			if(Storage[Sort[j]]->CPLevel > Storage[Sort[j+1]]->CPLevel)
//			{
//				temp = Sort[j];
//				Sort[j] = Sort[j+1];
//				Sort[j+1] = temp;
//				t=true;
//			}
//		}
//
//	}
//
//	return Sort;
//}
//std::vector<int> RulesStorage::FisherSort()
//{
//	Sort.resize(sz_Storage);
//	for ( int i = 0; i < sz_Storage; i++ )
//	{
//		Sort[i] = i;
//	}
//
///*
//*	fisher by bubble sort
//*/
//
//	int temp;
//	bool t=true;
//	while(t)
//	{
//		t=false;
//		for ( int j = 0; j < sz_Storage-1; j++ )
//		{
//			if(Storage[Sort[j]]->FisherValue > Storage[Sort[j+1]]->FisherValue)
//			{
//				temp = Sort[j];
//				Sort[j] = Sort[j+1];
//				Sort[j+1] = temp;
//				t=true;
//			}
//		}
//
//	}
//
//
//
//	return Sort;
//}
//
////---------------------------------------------------------------------------
//std::vector<int> RulesStorage::TargetSort()
//{
//	Sort.resize(sz_Storage);
//	for ( int i = 0; i < sz_Storage; i++ )
//	{
//		Sort[i] = i;
//	}
///*
//*	target
//*/
//	int temp;
//	bool t=true;
//	while(t)
//	{
//		t=false;
//		for ( int j = 0; j < sz_Storage-1; j++ )
//		{
//			if( Storage[Sort[j]]->getTTPos() > Storage[Sort[j+1]]->getTTPos() )
//			{
//				temp = Sort[j];
//				Sort[j] = Sort[j+1];
//				Sort[j+1] = temp;
//				t=true;
//			}
//		}
//
//	}
//
//	t=true;
//	while(t)
//	{
//		t=false;
//		for ( int j = 0; j < sz_Storage-1; j++ )
//		{
//			if( Storage[Sort[j]]->getTTPos() == Storage[Sort[j+1]]->getTTPos()
//				&& Storage[Sort[j]]->CPLevel*Storage[Sort[j+1]]->CPLevel < .0
//				&& Storage[Sort[j]]->CPLevel > .0 )
//			{
//				temp = Sort[j];
//				Sort[j] = Sort[j+1];
//				Sort[j+1] = temp;
//				t=true;
//			}
//		}
//
//	}
//
//	t=true;
//	while(t)
//	{
//		t=false;
//		for ( int j = 0; j < sz_Storage-1; j++ )
//		{
//			if( Storage[Sort[j]]->getTTPos() == Storage[Sort[j+1]]->getTTPos()
//				&& Storage[Sort[j]]->CPLevel*Storage[Sort[j+1]]->CPLevel > 0.
//				&& Storage[Sort[j]]->getTTValue() > Storage[Sort[j+1]]->getTTValue() )
//			{
//				temp = Sort[j];
//				Sort[j] = Sort[j+1];
//				Sort[j+1] = temp;
//				t=true;
//			}
//		}
//
//	}
//
//	return Sort;
//}

};

// ----------------------------------------------------------------------------
// blocks
// ----------------------------------------------------------------------------
class blocks : public std::iterator<std::forward_iterator_tag, RuleChains*>
{
public:
    
    blocks(RuleChains** start, size_t size = 0) : current(start), increment(size) {}

    bool operator==(const blocks &rhs) const { return current == rhs.current; }
    bool operator!=(const blocks &rhs) const { return current != rhs.current; }
    RuleChains* operator*() const { return *current; }

    blocks &operator++()
    {
        current = &current[increment];
        return *this;
    }

private:
    RuleChains** current;
    size_t increment;
};


#endif


