#pragma once

#include "../NaiveGenerator.h"
#include "../Runnable.h"
#include "Carcass.h"
#include "StringIProcessor.h"
#include "../Thread.h"


class DataRunnable :
	public Runnable 
{
public:
	DataRunnable(NaiveRuleGenerator* generator) : m_RegGen(generator) {}
	void* run() {
		m_RegGen->GenRules();
		return reinterpret_cast<void*>(m_RegGen);
	}
private:
	NaiveRuleGenerator* m_RegGen;
};


class SCIDI_API DataProcessor :
	public Carcass, public StringIProcessor
{
public:
	DataProcessor(void);
	~DataProcessor(void);
	void start(ProcessorSettings* set);
	void terminate();
	bool in_progress();
	bool is_completed();

    std::string GetName(){ return std::string("BaseDataProcessor"); };
    std::string GetParentName(){ return std::string("DataProcessor"); };
private:
	TerminalThread* _thread;
};

PLUMA_INHERIT_PROVIDER(DataProcessor, StringIProcessor);
