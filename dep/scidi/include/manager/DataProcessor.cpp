
#include "DataProcessor.h"
#include "../Callback.h"

DataProcessor::DataProcessor(void)
{
	_thread = 0;
}

DataProcessor::~DataProcessor(void)
{
	if (_thread) delete _thread;
}

void DataProcessor::start(ProcessorSettings* set)
{
	if (Carcass::m_FinishStorage != NULL) delete Carcass::m_FinishStorage;
	Carcass::m_FinishStorage = new RulesStorage(m_SEQStorage->getWidth(), m_SEQStorage->getCodesCount());
	set->RegSet.RulesContainer = Carcass::m_FinishStorage;

	NaiveRuleGenerator* gen	= new NaiveRuleGenerator();
	gen->SetSettings(set->RegSet);
	gen->setCallback(set->Callback);
	gen->SetInput(m_SEQStorage);


	std::shared_ptr<Runnable> r(new DataRunnable(gen));
	if (_thread) delete _thread;
	_thread = new TerminalThread(r);
	_thread->setCallback(set->Callback);
	gen->setCommand(new ThreadCommand(_thread));
	_thread->start();
}

void DataProcessor::terminate()
{
	if (_thread) _thread->terminate();
}

bool DataProcessor::is_completed()
{
	if (_thread)
		return _thread->is_completed();
	else
		return false;
}

bool DataProcessor::in_progress()
{
	if (_thread)
		return _thread->in_progress();
	else
		return false;
}