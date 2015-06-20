#pragma once

#include <string>
#include "ProcessorSettings.h"
#include "../../setup.h"
#include <Pluma/Pluma.hpp>

class SCIDI_API IProcessor
{
public:
	//virtual void init(ProcessorSettings*)=0;
	virtual void start(ProcessorSettings*)=0;
	virtual bool in_progress()=0;
	virtual bool is_completed()=0;
	virtual void terminate() = 0;

	virtual std::string GetName()=0;
	virtual std::string GetParentName()=0;
    //virtual std::string GetDescription()=0;
};

PLUMA_PROVIDER_HEADER(IProcessor);