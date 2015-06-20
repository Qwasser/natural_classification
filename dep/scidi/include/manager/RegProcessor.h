#pragma once

#include <iostream>

#include "../Classificator.h"
#include "../Runnable.h"
#include "../Reporter.h"
#include "Carcass.h"
#include "StringIProcessor.h"

class RegRunnable :
	public Runnable 
{
public:
	RegRunnable(Classificator* classificator) : m_classificator(classificator) {}
	virtual void* run() {
	    m_classificator->GenClasses();
		return reinterpret_cast<void*>(m_classificator);
	}
private:
    Classificator* m_classificator;
};


class SCIDI_API RegProcessor :
	public Carcass,
	public StringIProcessor
{
public:
	RegProcessor(void);
	~RegProcessor(void);
    void start(ProcessorSettings* set);
	void terminate();
	bool in_progress();
	bool is_completed();

    std::string GetName(){ return std::string("BaseRegProcessor"); };
    std::string GetParentName(){ return std::string("RegProcessor"); };
    
	int StartClassSearch(int nClassNumber);
	int CountRecPredMatrix(int Rec_or_Pred,int nClassNumber);
	int DeleteIdealObject(int nClassNumber);
	void MarkRules(int);
	void FindSubideals();
    long LoadAllIdelObjs();
    
private:
    std::ostringstream _status;
    Classificator* m_classificator;
	TerminalThread* _thread;
};

PLUMA_INHERIT_PROVIDER(RegProcessor, StringIProcessor);
