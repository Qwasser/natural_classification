#pragma once

#include "Tunnels.h"
#include "Sequence.h"
#include "Rule.h"
#include "Reporter.h"
#include "Thread.h"


class DataReporter :
	public EmbedReporter
{
public:
	void ReportNewRegularity(void) { Report()->signal("regularity"); };
};


class SCIDI_API RuleManager :
	public DataReporter
{
public:
	virtual void SetInput(Sequence*, long) = 0;
	virtual Rule* GetOutput() = 0;
	virtual unsigned long GetOutputSize() = 0;
	virtual void GenRules(RuleSettings p) = 0;
	bool IsGeneratorTerminated() { return CheckTermination->Execute(); };
	void setCommand(ThreadCommand* c) { CheckTermination = c; };
private:
	ThreadCommand* CheckTermination;
};
