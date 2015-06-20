//---------------------------------------------------------------------------
#include <Pluma/Pluma.hpp>

#include "MainManager.h"
#include "Constants.h"
#include "manager/DataProcessor.h"
#include "manager/RegProcessor.h"
#include "manager/ObjProcessor.h"



//---------------------------------------------------------------------------
TMainManager::TMainManager()
{
	LoadPlugins(std::string("plugins"));
	ActivatePlugin("DataProcessor", "BaseDataProcessor");
	ActivatePlugin("RegProcessor", "BaseRegProcessor");
	ActivatePlugin("ObjProcessor", "BaseObjProcessor");
}
//---------------------------------------------------------------------------
TMainManager::~TMainManager()
{
	
}

void TMainManager::LoadPlugins(std::string path)
{
	pluma::Pluma pluma;
	pluma.acceptProviderType<StringIProcessorProvider>();
	// Add a default warrior provider
    pluma.addProvider(new DataProcessorProvider() );
	pluma.addProvider(new RegProcessorProvider() );
	pluma.addProvider(new ObjProcessorProvider() );
    // Load libraries
    pluma.loadFromFolder("plugins");
	// Get warrior providers into a vector
    std::vector<StringIProcessorProvider*> providers;
    pluma.getProviders(providers);

	Loaded_Processors.clear();
	// Create a Warrior from each provider
    std::vector<StringIProcessorProvider*>::iterator it;
    for (it = providers.begin() ; it != providers.end() ; ++it)
	{
        // Create a processor
        StringIProcessor * processor = (*it)->create();
		Loaded_Processors.insert(std::make_pair(processor->GetParentName(), processor));
        // don't forget to delete it
    }
	pluma.unloadAll();
}

void TMainManager::ActivatePlugin(std::string ProcType, std::string ProcName)
{
	processors p = Loaded_Processors.equal_range(ProcType);
	for (processor_iterator i = p.first; i != p.second; ++i)
		if ((*i).second->GetName() == ProcName)
		{
			Alive_Processors[ProcType] = (*i).second;
			break;
		}
}

int TMainManager::ExecProcessorTask(std::string ProcType, std::string TaskName)
{
	Alive_Processors[ProcType]->ExecFunc(TaskName);
	return RETURN_SUCCESS;
}

int TMainManager::ExecProcessorTask(std::string ProcType, std::string TaskName, ProcessorSettings* Settings)
{
	Alive_Processors[ProcType]->ExecFunc(TaskName, Settings);
	return RETURN_SUCCESS;
}

bool TMainManager::GetProcessorStatus(std::string ProcType, std::string StatusName)
{
	return Alive_Processors[ProcType]->GetStatus(StatusName);
}

//---------------------------------------------------------------------------
// Создает новый проект
int TMainManager::NewProject()
{
	return __IDMSG_PROJECTNOTSAVING_;
}
//---------------------------------------------------------------------------
// Открывает ранее сохраненный проект
RuleSettings TMainManager::OpenProject(ProjectSettings* pAppVariables)
{
	*pAppVariables = PParam;

	if(Carcass::m_FinishStorage!=NULL)
            delete Carcass::m_FinishStorage;

	
	FILE* arh = fopen("Storage.bin","rb");
	Carcass::m_FinishStorage = new RulesStorage( arh );     
	fclose(arh);
	
	if ( Carcass::m_FinishStorage != NULL)
	{

		//for (long i=0; i< this->m_FinishStorage->getSize(); i++)
		//{
		//		if ( m_FinishStorage->getRuleClass(i) >= 0)
		//		{
		//				this->RulesAreClassified = true;
		//				break;
		//		}
		//}
		;
	}
	if ( Carcass::m_SEQStorage != NULL)
	{
	/*		if ( !m_SEQStorage->NewStorage.empty() )
				this->CtrlNegSeqsAreClassified = true;
			else
				this->CtrlNegSeqsAreClassified = false;

			if ( !m_SEQStorage->CtrlPosStorage.empty() )
				this->CtrlPosSeqsAreClassified = true;
			else
				this->CtrlPosSeqsAreClassified = false;*/
		;
	}
	else
		Carcass::m_SEQStorage = new SEQStorage();


   int IdealizType = pAppVariables->IdealizeType;


	return RuleSettings();
}
//---------------------------------------------------------------------------
// Сохраняет проект
bool TMainManager::SaveProject()
{
	return false;
}
//---------------------------------------------------------------------------

// Сохраняет проект
bool TMainManager::SaveProject(ProjectSettings* pAppVariables)
{
	PParam = *pAppVariables;

	FILE* arh = fopen("Storage.bin","wb");
	Carcass::m_FinishStorage->Save( arh );
	fclose(arh);

	return true;
}

// Сохраняет проект со специфичным именем
bool TMainManager::SaveAsProject()
{
	return false;
}

//! TODO
// TMainManager::GetCurrentState

/*
if ( (m_SEQStorage == NULL) || (m_SEQStorage->getLength() == 0) )
return __IDMSG_NOSEQFORIDEALIZ_;
m_SEQStorage->Clear_classes();

if ( (m_FinishStorage == NULL) || (m_FinishStorage->getSize() == 0) )
return __IDMSG_NORULESFORIDEALIZ_;

if (m_FinishStorage->getSize() > 0 )
ClearObjects();
*/

//---------------------------------------------------------------------------
char* StringPlusNum(const char* str, int d)
{
	char* status = new char[strlen(str)+5+1];/* +1 - for \0 */
	sprintf(status, "%s%i", str, d);
	return status;
}
