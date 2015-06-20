#pragma once

#include "../Tunnels.h"
#include "../Callback.h"
#include "../Classificator.h"

class SCIDI_API ProcessorSettings
{
public:
	ProcessorSettings(void);
	~ProcessorSettings(void);
	
	RuleSettings RegSet;
	ClassificatorSettings ClassSet;
	Classificator* myClassificator;
	sdEvent* Callback;
};