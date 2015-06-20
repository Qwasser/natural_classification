
#include "../Thread.h"
#include "RegProcessor.h"

RegProcessor::RegProcessor(void)
{
	_thread = 0;
}

RegProcessor::~RegProcessor(void)
{
	if (_thread) delete _thread;
}

void RegProcessor::start(ProcessorSettings* set)
{
	m_classificator = new Classificator();
	ClearObjects(); // prepeare ObjsStorage
	m_classificator->Create(m_SEQStorage, m_FinishStorage, ObjsStorage);
	m_classificator->lObjsStorageCounter = lObjsStorageSize;
	m_classificator->SetSource(set->ClassSet.ObjsSource);
	m_classificator->SetType(set->ClassSet.IdealizType);
	m_classificator->setCallback(set->Callback);

	std::shared_ptr<Runnable> r(new RegRunnable(m_classificator));
	if (_thread) delete _thread;
	_thread = new TerminalThread(r);
	_thread->setCallback(set->Callback);
	m_classificator->setCommand(new ThreadCommand(_thread));
	_thread->start();
}

void RegProcessor::terminate()
{
	_thread->terminate();
}

bool RegProcessor::is_completed()
{
	return _thread->is_completed();
}

bool RegProcessor::in_progress()
{
	return _thread->in_progress();
}

//---------------------------------------------------------------------------
void RegProcessor::FindSubideals()
{
	//Report()->UpdateStatus("Start searching sub-ideals");

	int totalSub=0;
	int ObjRegSize, MinAppRegSize;
	long index;
	for(long l = 0; l<lObjsStorageSize; l++)
	{
		//находим маленькое REG множество
		MinAppRegSize = m_FinishStorage->getSize();
		for(long lObjsCounter = 0; lObjsCounter < lObjsStorageSize; lObjsCounter++)
		{
			ObjRegSize = ObjsStorage[lObjsCounter]->RulMarks.size();
			if(ObjRegSize > 0/*замени не empty*/ && ObjRegSize < MinAppRegSize)
			{
				MinAppRegSize = ObjRegSize;
				index = lObjsCounter;
			}
		}

		//проверяем большие
		//рассмотренные зануляем и не сравниваем больше
		int DiffRules;
		bool gotSubIdeal;
		//перебор снова всех объектов
		for(long Objs_i = 0; Objs_i < lObjsStorageSize; Objs_i++)
		{
			ObjRegSize = ObjsStorage[Objs_i]->RulMarks.size();
			if(ObjRegSize > 0 /*&& ObjRegSize < MinAppRegSize*/)
			{
				DiffRules = ObjRegSize - MinAppRegSize;
				gotSubIdeal = true;
				//перебор закономерностей из фиксированного класса, большого множества
				std::list<short>::iterator itr, min_itr = ObjsStorage[index]->RulMarks.begin();
				for(itr=ObjsStorage[Objs_i]->RulMarks.begin(); itr!=ObjsStorage[Objs_i]->RulMarks.end(); ++itr)
				{
					if(DiffRules>=0)
					{
						if( min_itr == ObjsStorage[index]->RulMarks.end() )
							break;

						short& reg_index = *itr;//номера закономерностей больших множеств
						short& min_reg_index = *min_itr;//сравнение с минимальным

						if( reg_index!=min_reg_index )
						{
							DiffRules--;
						}
						min_itr++;

					}
					else
					{
						gotSubIdeal = false;
						break;
					}

				}
				if(gotSubIdeal)
				{
					totalSub++;
                    /*
                    _status.str("");
					_status.clear();
					_status << MyForm->IdealizObjectsList->Cells[3][Objs_i+1].t_str();
					_status << " " << (index+1);
					MyForm->IdealizObjectsList->Cells[3][Objs_i+1] = _status.str().c_str();
					*/
				}

			}//условие, что множество еще не рассмотрено
		}//чек больших рег множеств
		ObjsStorage[index]->RulMarks.clear();
	}//для возврата и поиска нового минимума
	_status.str("");
	_status.clear();
	_status << "Found " << totalSub << " sub-ideals";
	//Report()->UpdateStatus(_status.str().c_str());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Загружает все идеальные объекты
long RegProcessor::LoadAllIdelObjs()
{
    using namespace std::placeholders;
    
	if ( this->m_SEQStorage == NULL )
	{
		return -1;
	}

	int nCodesCount = m_SEQStorage->getCodesCount();
	int lSeqLength = m_SEQStorage->getWidth();
	char* chObject = new char[(nCodesCount + 3) * lSeqLength + 2];

	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageSize; lObjsCounter++ )
    {
		ObjsStorage[lObjsCounter]->getObjAsStr(chObject, std::bind(&SEQStorage::Decode, m_SEQStorage, _1));
		//ReportNewObject(/*std::string(chObject), ObjsStorage[lObjsCounter]->getGamma()*/);
	}
	delete[] chObject;
	return(1);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Ищет объекты данного класса
int RegProcessor::StartClassSearch( int nClassNumber)
{
    using namespace std::placeholders;
    
	if ( ObjsStorage == NULL )
	{
		return __IDMSG_NORULESFORIDEALIZ_;
	}
	long t=0;
	char* chObject = new char[(m_SEQStorage->getCodesCount() + 3) * m_SEQStorage->getWidth() + 2];;
	CIdelObject* CurrObj = new CIdelObject;
	SEQSElem* CurrSeq = new SEQSElem();
	for ( int i=0; i < m_SEQStorage->getLength(); i++)
	{

			if ( m_SEQStorage->getClass(i) == nClassNumber)
			{
					m_SEQStorage->CreateElem(i, CurrSeq);
					CurrObj->Create(m_SEQStorage->getWidth(), m_SEQStorage->getCodesCount(), CurrSeq);
					CurrObj->getObjAsStr(chObject, std::bind(&SEQStorage::Decode, m_SEQStorage, _1));
					//Report()->NewObject(std::string(chObject), t);
					t++;
			}
	}
	delete CurrObj;
	delete[] chObject;
	delete CurrSeq;
	return -1;
}


//---------------------------------------------------------------------------
//Вычисление матрицы распознавания или предсказания
int RegProcessor::CountRecPredMatrix( int Rec_or_Pred, int nClassNumber)
// Rec_or_Pred: 0 - Recognition 1 - Prediction
{
	if ( ObjsStorage == NULL )
	{
		return __IDMSG_NORULESFORIDEALIZ_;
	}

        int codes_count;
		long seq_length;
        double v_val;

	codes_count = m_SEQStorage->getCodesCount();
		seq_length = m_SEQStorage->getWidth();

        m_classificator->CountMatrix( nClassNumber );

	for ( long ColCounter = 0; ColCounter < seq_length; ColCounter++ )
	{
            for ( int RowCounter = 0; RowCounter < codes_count; RowCounter++ )
            {
                if ( Rec_or_Pred == 0)
                       m_classificator->Rec_VMatrixVal(ColCounter, RowCounter, v_val );
                else if ( Rec_or_Pred == 1)
                        m_classificator->Pred_VMatrixVal(ColCounter, RowCounter, v_val );

                //MyForm->FillInWeightMatix(v_val, ColCounter, RowCounter );
            }
        }

	return -1;
}


//---------------------------------------------------------------------------
void RegProcessor::MarkRules(int Class)
{
	for(int i=0; i < m_FinishStorage->getSize(); i++)
	{
		//определяем применимость, подтверждаемость, опровержимость
		m_classificator->DefineRule(i, Class);
	}
}

//Удаление идеальных объектов и внесенние поправок в классы последовательностей
int RegProcessor::DeleteIdealObject( int nClassNumber)
{
	if ( ObjsStorage == NULL )
	{
		return __IDMSG_NORULESFORIDEALIZ_;
	}

		for ( int i=0; i < m_SEQStorage->getLength(); i++)
        {

				if ( m_SEQStorage->getClass(i) == nClassNumber)
                {
						m_SEQStorage->setClass(i, -1);
                }
				if ( m_SEQStorage->getClass(i) > nClassNumber)
                {
						m_SEQStorage->setClass(i, m_SEQStorage->getClass(i)-1);
                }

        }

        //if ( this->RulesAreClassified )
        {
            long nClass;
			for ( long i=0; i < this->m_FinishStorage->getSize(); i++)
            {
				nClass = m_FinishStorage->getRuleClass(i);
                if (  nClass == nClassNumber)
                {
						m_FinishStorage->setRuleClass(i,-1);
                }
                if ( nClass > nClassNumber)
                {
						m_FinishStorage->setRuleClass(i,nClass-1);
                }

            }
        }

        //if ( this->CtrlNegSeqsAreClassified)
		for ( long i=0; i < m_SEQStorage->NewStoreLength ; i++)
        {

                if ( m_SEQStorage->NewStorage[i].GetClassId() == nClassNumber)
                {
                        m_SEQStorage->NewStorage[i].SetClassId(-1);
                }
                if ( m_SEQStorage->NewStorage[i].GetClassId() > nClassNumber)
                {
                        m_SEQStorage->NewStorage[i].DecClassId();
                }

        }

       // if ( this->CtrlPosSeqsAreClassified)
        for ( long i=0; i < m_SEQStorage->CtrlPosStoreLength; i++)
        {

                if ( m_SEQStorage->CtrlPosStorage[i].GetClassId() == nClassNumber)
                {
                        m_SEQStorage->CtrlPosStorage[i].SetClassId(-1);
                }
                if ( m_SEQStorage->CtrlPosStorage[i].GetClassId() > nClassNumber)
                {
                        m_SEQStorage->CtrlPosStorage[i].DecClassId();
                }

        }

        //if ( this->NegSeqsAreLoaded )
        for ( long i=0; i < m_SEQStorage->LrnNegStoreLength; i++)
		{

                if ( m_SEQStorage->LrnNegStorage[i].GetClassId() == nClassNumber)
                {
                        m_SEQStorage->LrnNegStorage[i].SetClassId(-1);
                }
                if ( m_SEQStorage->LrnNegStorage[i].GetClassId() > nClassNumber)
                {
                        m_SEQStorage->LrnNegStorage[i].DecClassId();
                }

        }

        for (int i=nClassNumber; i< lObjsStorageSize; i++)
        {

                ObjsStorage[i] = ObjsStorage[i+1];
        }

		delete ObjsStorage[lObjsStorageSize - 1];
        ObjsStorage[lObjsStorageSize - 1] = NULL;
        lObjsStorageSize--;
        m_classificator->lObjsStorageCounter--;

	return -1;
}



