//---------------------------------------------------------------------------

#ifndef ReporterH
#define ReporterH

#include <iostream>
#include "Rule.h"
#include "Callback.h"

class SCIDI_API Reporter
{
public:
	virtual void NewRegularity(Rule* rule){
		//rule->
	};
	virtual void PrintLog(std::string message) { std::cout << message << std::endl; };
	virtual void UpdateStatus(std::string message) { std::cout << message << std::endl; };
	virtual void NewObject(std::string object, double rating) {
		std::cout << "object: " << object << " (" << rating << ")" << std::endl;
	};
};


class SCIDI_API EmbedReporter
{
public:
	sdEvent* Report(){ return _callback; };
	void setCallback(sdEvent* c) { _callback = c; };
private:
	sdEvent* _callback;
};


#endif
