//---------------------------------------------------------------------------

#ifndef IStorageH
#define IStorageH

// ----------------------------------------------------------------------------
// IStorage
// ----------------------------------------------------------------------------
class SCIDI_API IStorage
{
public:
	virtual void Save(FILE* arh)=0;
	virtual void Load(Rule** Ref, long Size)=0;
	virtual Rule* GetAsArray()=0;
	virtual unsigned long GetSize()=0;
	virtual unsigned long GetActualSize()=0;
	virtual Rule* Remove(long RuleIndex)=0;
	virtual void Clear()=0;
	
	virtual int SetRuleClass(long Index, long nClass)=0;
	virtual long GetRuleClass(long Index)=0;

	virtual bool Add(Rule* R)=0;
	virtual bool IsFind(Rule* ptr_Rule)=0;
	virtual Rule& operator[](long lStoreIndex)=0;
	
};

#endif
