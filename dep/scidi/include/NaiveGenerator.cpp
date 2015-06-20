//---------------------------------------------------------------------------
#include "NaiveGenerator.h"
#include <time.h>
//---------------------------------------------------------------------------
NaiveRuleGenerator::NaiveRuleGenerator()
{
	FinishRulesStorage = NULL;
	sz_mySEQStorage = 0;
	StartRule = NULL;
	tree_id=-1;
}
//---------------------------------------------------------------------------
NaiveRuleGenerator::~NaiveRuleGenerator()
{
}
//---------------------------------------------------------------------------
int NaiveRuleGenerator::Create(SEQStorage* mySEQStorage, RulesStorage* FinishStorage,
						double ConfInterval_Value, double Yule, double CProb_MinLevel)
{
	Conf_Interval = ConfInterval_Value;
	Conf_Interval2 = Yule;
	Base_run = 1;
	CP_minLEVEL = CProb_MinLevel;
	leftNeg = true;
	rightNeg = true;

	ptr_mySEQStorage = mySEQStorage;
	FinishRulesStorage = FinishStorage;
	ParseStorage();
	StartPos = 0;
	FinishPos = nWidth - 1;
	return 0;
}

void NaiveRuleGenerator::GenRules()
{
	/* run search */
	StartNextIter(StartPos, FinishPos);
    FinishRulesStorage->MakePointersArray(); // finalize rules storage
}


void NaiveRuleGenerator::SetSettings(RuleSettings p)
{
	/* store settings */
	Conf_Interval = p.FisherSumBound;
	Conf_Interval2 = p.YuleQUpBound;
	CP_minLEVEL = p.MinProb;
	Base_run = p.SearchDepth;
	leftNeg = p.DeclinePremise;
	rightNeg = p.DeclineDecision;
	StartPos = p.StartPos;
	FinishPos = p.FinishPos;
	if (FinishRulesStorage != NULL && FinishRulesStorage != p.RulesContainer)
		delete FinishRulesStorage;
	FinishRulesStorage = p.RulesContainer;
}

void NaiveRuleGenerator::GenRules(RuleSettings p)
{
	SetSettings(p);
	ParseStorage();
	if (FinishRulesStorage == NULL)
		FinishRulesStorage = new RulesStorage(nWidth, nCodesCount);

	/* run search */
	StartNextIter(p.StartPos, p.FinishPos);
	FinishRulesStorage->MakePointersArray(); // finalize rules storage
}

void NaiveRuleGenerator::SetInput(Sequence* input, long size)
{
	ptr_mySEQStorage = new SEQStorage(input, size);
	ParseStorage();
}

void NaiveRuleGenerator::SetInput(SEQStorage* s)
{
	ptr_mySEQStorage = s;
	ParseStorage();
}

Rule* NaiveRuleGenerator::GetOutput()
{
	return FinishRulesStorage->c_array();
}

void NaiveRuleGenerator::ParseStorage()
{
	/* invoke requirement data */
	sz_mySEQStorage = ptr_mySEQStorage->getItemsCount();
	nWidth = ptr_mySEQStorage->getObjectWidth();
	nCodesCount = ptr_mySEQStorage->getCodesCount();
}
//---------------------------------------------------------------------------
//Ищет правила длины 1 с заданной позицией целевого признака
int NaiveRuleGenerator::StartNextIter(long s_TTPos, long f_TTPos)
{
	tree_id=-1;
	for(long nTT = s_TTPos; nTT < f_TTPos; nTT++)
	{
        for (int ST = 0; ST < nCodesCount; ST++)
		{
			if (IsGeneratorTerminated()) return -1;

			StartRule = new Rule();

			StartRule->setTTPos(nTT);
			StartRule->setTTValue(ST);
			StartRule->setTTSign(1);
			ReportNewRegularity(/* StartRule */);
			SelectCells(StartRule);

			delete StartRule;
			//denial

			if (IsGeneratorTerminated()) return -1;

			if(rightNeg)
			{
				StartRule = new Rule();

				StartRule->setTTPos(nTT);
				StartRule->setTTValue(ST);
				StartRule->setTTSign(-1);
				ReportNewRegularity(/* StartRule */);
				SelectCells(StartRule);

				delete StartRule;
			}
		}
	}
	return 0;
}
//Процедура берет закономерность
//пытается ее сократить, полученную закономерность проверяет на уникальность,
//соответсвие заданным пользователем параметрам условной вероятности
//и критерия Фишера и в положительном случае добавляет ее в финальное хранилище
bool NaiveRuleGenerator::NextIter(Rule* NextRule)
{
	bool NotNeedDel=false; //итерация удалась
	Changed = false;
	//VZ2 //$$$$$$$$
	Changed = NextRule->Minimize(ptr_mySEQStorage, Conf_Interval, Conf_Interval2);
	if ( !Changed )
	{
		if( fabs(NextRule->getCP()) >= CP_minLEVEL )
		{
			//WaitForSingleObject( hScreenMutex, INFINITE );
			if (FinishRulesStorage->Add(NextRule))
			{
				ReportNewRegularity(/* StartRule */);
				NotNeedDel=true; //итерация удалась, объект нужно сохранить
			}
			//ReleaseMutex( hScreenMutex );
		}
		//else
			//NextRule->Summary = RULE_LOW_LEVEL_PROBABILITY;

		SelectCells(NextRule);
	}
	return NotNeedDel;
}
//---------------------------------------------------------------------------
//Перебирает все возможности наращивания заданной закономерности
void NaiveRuleGenerator::SelectCells(Rule* ptr_Rule)
{
	if (IsGeneratorTerminated()) return;

	if ( ptr_Rule->getRuleLength() >= nWidth || ptr_Rule->getRuleLength() >= MAXDEFLEN )
			return;
/*	int l=ptr_Rule->getLength();
	if ( l >= nWidth )
			return;
	FILE *data = fopen("cp.txt", "a");
	fprintf(data,"(%i) %f\t",l,ptr_Rule->getCP());
	if(l==1) fprintf(data,"\n");
	fclose(data);
	*/
	bool* BusyVector = new bool[nWidth];
	ShiftBusy(BusyVector, ptr_Rule);
	long nTTPos = ptr_Rule->getTTPos();
	int LEFT_BOUND = 0;//ptr_Rule->getShift( ptr_Rule->getRuleLength() ) + 1;
	for (int Shift = LEFT_BOUND; Shift < nWidth; Shift++)
	{
		if (Shift!=nTTPos && !BusyVector[Shift])
		{
			for (int SendToken = 0; SendToken < nCodesCount; SendToken++)
			{	
						AddRule(ptr_Rule, Shift, 1, SendToken);
						if (leftNeg)
							AddRule(ptr_Rule, Shift, -1, SendToken);
			}

		}
	}

	delete[] BusyVector;
}

//---------------------------------------------------------------------------
//Отмечает позиции признаков, которые могут быть использованы при наращивании
int NaiveRuleGenerator::ShiftBusy(bool* ptr_myBusyVector, Rule* ptr_Rule)
{
	//инициализация
	for (int index = 0; index < nWidth; index++)
	{
		ptr_myBusyVector[index] = false;
	}
	//выкидываем целевой признак
	long nTTPos = ptr_Rule->getTTPos();
	ptr_myBusyVector[nTTPos] = true;
	//и признаки, уже в правиле
	for (long ChainIndex = 0; ChainIndex < ptr_Rule->getRuleLength(); ChainIndex++)
	{
		ptr_myBusyVector[ptr_Rule->getShift(ChainIndex)] = true;
	}

	return(1);
}
//---------------------------------------------------------------------------
// Наращивает закономерность ptr_Rule,
// добавляя предикат (shift, sign, sendtoken)), проверяет
// выполнимость условийй вероятностных закономерностей, уникальность
// и в положительном случае добавляет в резулт. мн-во
void NaiveRuleGenerator::AddRule(Rule* ptr_Rule, int Shift, int Sign, int SendToken)
{
	RuleSection Sent;
	Sent.Shift = Shift;
	Sent.Sign = Sign;
	Sent.Value = SendToken;
	Rule* NextRule = new Rule(ptr_Rule, FinishRulesStorage->GetPredicatePointer(Shift, Sign, SendToken));

	if(	FinishRulesStorage->isFind(NextRule) )
	{
		//NextRule->Summary = RULE_DUBLICATE;
		BaseRun(NextRule);
		return;
	}
	//check VZ1 //$$$$$$$$
					

	bool nullData = ( NextRule->Probability(ptr_mySEQStorage)==RULE_HAS_CP ) ? false : true;
	if( nullData )
	{
		//NextRule->Summary = RULE_NO_DATA;
		BaseRun(NextRule);
		return;
	}

	if ( fabs(NextRule->getCP()) <= fabs(ptr_Rule->getCP()) )
	{
		//NextRule->Summary = RULE_LOW_COND_PROBABILITY;
		BaseRun(NextRule);
		return;
	}
			
					
//	Fisher & Yule
//	return
//	DECLINE_GROW
//	or
//	ACCEPT_GROW
					//======

	//NextRule->SetLambda(InverseNormalCDF(1.-0.5*Conf_Interval));
	int hipoteza = NextRule->Criteria(ptr_mySEQStorage, Conf_Interval, Conf_Interval2);
	if( hipoteza==DECLINE_GROW )
	{
		//NextRule->Summary already written
		BaseRun(NextRule);
		return;
	}
	// if ACCEPT_GROW

	if( !NextIter(NextRule) )
		BaseRun(NextRule);
//	else 
//		delete NextRule;
}


//---------------------------------------------------------------------------
//Проверяет закономерность на уникальность
//=========================
bool NaiveRuleGenerator::isUnique(Rule* ptr_Rule)
{
	if (ptr_Rule->getRuleLength() == 0) return(false);
	if (FinishRulesStorage->isFind(ptr_Rule)) return(false);
	return(true);
}


//---------------------------------------------------------------------------
//Базовый перебор
//=========================
void NaiveRuleGenerator::BaseRun(Rule* ptr_Rule)
{
	if(ptr_Rule->getRuleLength() < Base_run)
		SelectCells(ptr_Rule);

	delete ptr_Rule;

}

