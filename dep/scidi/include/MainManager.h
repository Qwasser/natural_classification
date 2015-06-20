//---------------------------------------------------------------------------
#ifndef MainManagerH
#define MainManagerH
#define NO_WIN32_LEAN_AND_MEAN
//---------------------------------------------------------------------------
#include <math.h>
#include <iostream>
#include <map>

#include "manager/IProcessor.h"
#include "manager/StringIProcessor.h"
#include "manager/Carcass.h"
#include "FileManager.h"
#include "Classificator.h"
#include "NaiveGenerator.h"

typedef std::map<std::string, StringIProcessor*> processor_table;

typedef std::multimap<std::string, StringIProcessor*> processor_stock;
typedef processor_stock::const_iterator processor_iterator;
typedef std::pair<processor_iterator, processor_iterator> processors;


//=========================
//	Main Manager
//=========================
class SCIDI_API TMainManager
{
	std::ostringstream _status;
public:
	TMainManager();
	virtual ~TMainManager();

	int NewProject();
	RuleSettings OpenProject(ProjectSettings* pAppVariables);
	bool SaveProject();
	bool SaveProject(ProjectSettings* pAppVariables);
	bool SaveAsProject();
	
	void LoadPlugins(std::string path);
	void ActivatePlugin(std::string ProcType, std::string ProcName);
	int ExecProcessorTask(std::string ProcType, std::string TaskName);
	int ExecProcessorTask(std::string ProcType, std::string TaskName, ProcessorSettings* Settings);
	bool GetProcessorStatus(std::string ProcType, std::string StatusName);

private:

	processor_stock Loaded_Processors;
	processor_table Alive_Processors;

	ProjectSettings PParam;
	RuleSettings RParam;
	double ConfInt_Value;
	double CP_minLEVEL;
	int AlignmentType;
    long nWidth;
	int ObjsSource;
};

SCIDI_API char* StringPlusNum(const char* str, int dg);

#endif

