#pragma once

#include "../Classificator.h"
#include "../Runnable.h"
#include "Carcass.h"
#include "StringIProcessor.h"

class ObjRunnable :
	public Runnable 
{
public:
	ObjRunnable(Classificator* classificator) : m_Classificator(classificator) {}
	virtual void* run() {
	    m_Classificator->GenClasses();
		return reinterpret_cast<void*>(m_Classificator);
	}
private:
    Classificator* m_Classificator;
};


class SCIDI_API ObjProcessor :
	public Carcass, public StringIProcessor
{
private:
	Classificator* myClassificator;
	TerminalThread* _thread;

    long OneIter(SEQSElem* NextSeq, long IterCounter);
	long OneIter(Rule* NextReg, long IterCounter);
    double CountF(SEQSElem* NextSeq,  int nSeqNumb, int nClNumber );
    int CreateClassificator();
        
public:
    ObjProcessor(void);
	ObjProcessor(Classificator* pC);
	~ObjProcessor(void);

    void start(ProcessorSettings* set);
	void terminate();
	bool in_progress();
	bool is_completed();

    std::string GetName(){ return std::string("BaseObjProcessor"); };
    std::string GetParentName(){ return std::string("ObjProcessor"); };
    
	int StartNewIdeal();
	void EndNewIdeal();
	void NewIdealThread();
	
    int nRecClass;
    int EC_FirstClass;
    int EC_LastClass;
    bool EC_OneClass;
    bool ControlRecognized;
    int ErrCountPosSource;
    int ErrCountNegSource;
    
    void RecognitionThread();
	int NewIdealThreadStart();
    int StartRecognition(int nRecClass = -1, int RecSourcePos = -1, int RecSourceNeg = -1);
    void EndRecognition(int LSCount);
    int NextRecognition();

    int StartRecDrawing(long nClassNumber);
    long CheckClNumber;

    double* LDScoreArray;
    void ReCount(double FstErr);
    void RecErrCountDraw( int LnStCount );
    void ScoreControlData(double score);
    void ScoreLearningData(double score);
	void DrawHopfieldData();

    void CountPercents();

    double* DrawArray;

};

PLUMA_INHERIT_PROVIDER(ObjProcessor, StringIProcessor);
