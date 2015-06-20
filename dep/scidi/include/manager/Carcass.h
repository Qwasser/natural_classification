/*
 * This class is used in current base computing units
 * Not required for plugins ... maybe in future - it hardcoded into gui, sorry :(
 */

#pragma once

#include "../RuleManager.h"
#include "../RulesStorage.h"
#include "../CIdelObject.h"
#include "../Tunnels.h"
#include "../Reporter.h"

class SCIDI_API Carcass : public EmbedReporter
{
protected:
	Carcass(void);
	virtual ~Carcass(void);
public:	
	static Carcass* Instance();

	int AddSeqs(const char* SourceFilePath, const char* regx, const char* tokenx);
	bool AddSeqs(const char* SourceFilePath);
	std::string getSeqName(int StoreType,long StoreIndex);
	std::string getSeq(int StoreType,long StoreIndex);
	int getLength(int StoreType, long StoreIndex);
	//long getSeqClass(long StoreIndex);
    long getObjCount() { return SeqCount; };
    void ClearObjects();
    	
	// NegSeqArray = new Sequence[NegSeqCount]
    // SeqArray_CrlNeg = new Sequence[NewSeqCount]
    // SeqArray_CrP = new Sequence[CtrlPosSeqCount]
	Sequence* SeqArray;
	Sequence* SeqArray1;
	Sequence* NegSeqArray;
	Sequence* SeqArray_CrlNeg;
	Sequence* SeqArray_CrP;
	long SeqCount;
	long NegSeqCount;
	long NewSeqCount;
	long CtrlPosSeqCount;
	
	static SEQStorage* m_SEQStorage;
	static RulesStorage* m_FinishStorage;
	static CIdelObject** ObjsStorage;
	static unsigned long lObjsStorageSize;
	static Carcass* m_instance;
};
