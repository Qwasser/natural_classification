//---------------------------------------------------------------------------
/*
*
22:50 16.01.2012
Интересная вещь: алгоритм выверен по всем случаям отрицания и без, 
в посылке и заключении (благодаря симметричности теперь все достаточно прозрачно).
Но на бинарных данных с пропусками выдает разницу 
между быстрой оценкой и всеми правилами.
Когда смотрят все правила, то шаг делается так, 
что изменений в множестве применимых правил не наблюдается.
При быстрой же оценке, мы полагаемся на правильно извлеченные законы из данных.
Всегда работает палка о двух концах: 
либо неправильно найдены законы,
либо поиск неподвижной точки неверен.
*
*/

#include <algorithm>
#include "Classificator.h"
#include "manager/Carcass.h"

//---------------------------------------------------------------------------

int Classificator::id = 0;

Classificator::Classificator()
{
	VMatrix = NULL;
	pSeqStorage = NULL;
	lSeqStoreSize = 0;
	pRulesStorage = NULL;
	lRuleStoreSize = 0;
	ObjsStorage = NULL;
	lObjsStorageCounter = 0;
	m_NextSeq = NULL;
	FilterSize = 0;
	FilterSize2 = 0;
	IdealizType = -1;
	lIterCounter = 0;
	id=0;
	InitObj=NULL;
	CurrObj=NULL;
	CurrIdelObject = NULL;
}

Classificator::Classificator(int IType)
{
	//TODO: Add your source code here
	VMatrix = NULL;
	pSeqStorage = NULL;
	lSeqStoreSize = 0;
	pRulesStorage = NULL;
	lRuleStoreSize = 0;
	ObjsStorage = NULL;
	lObjsStorageCounter = 0;
	m_NextSeq = NULL;
	FilterSize = 0;
	FilterSize2 = 0;
	
	IdealizType = IType;

	lIterCounter = 0;
	id=0;
	InitObj=NULL;
	CurrObj=NULL;
	CurrIdelObject = NULL;
}


Classificator::~Classificator()
{
	if(VMatrix!=NULL)
	{
		for ( unsigned long ColCounter = 0; ColCounter < lSeqLength; ColCounter++ )
		{
			delete[] VMatrix[ColCounter];
		}
		delete[] VMatrix;
	}
	if(InitObj!=NULL)
		delete[] InitObj;
	if(CurrObj!=NULL)
		delete[] CurrObj;

	
	if(CurrIdelObject!=NULL)
		delete CurrIdelObject;
	I_negative.clear();
	I_positive.clear();
	p_AplicReg.clear();
	p_SubsetRules.clear();

}


bool Classificator::Create(		SEQStorage* pSeqStorage,
								RulesStorage* pRulesStorage,
								CIdelObject** ObjsStorage)
{
		//а. исходные объекты.           SEQStorage* pSeqStorage
        //б. найденные закономерности.   RulesStorage* pRulesStorage
        //Тип идеализации               IdealizType

	if ( pSeqStorage == NULL )
	{
		return false;
	}
	else if ( pRulesStorage == NULL )
	{
		return false;
	}
	else if ( (IdealizType < 0) && (IdealizType > 3) )
	{
		return false;
	}

	//Sequences
	this->pSeqStorage = pSeqStorage;
	lSeqStoreSize = pSeqStorage->getLength();
	this->lSeqLength = pSeqStorage->getWidth();

	//Rules
	this->pRulesStorage = pRulesStorage;
	lRuleStoreSize = pRulesStorage->getSize();
	//lRuleStoreSize = DelSubRules(pRulesStorage->Storage, lRuleStoreSize);//we work with SPR
////DEBUG_B
//	char rule1[256];
//	FILE *data1 = fopen("DelSubRules.txt","w");
//	for(int i=0; i<lRuleStoreSize; i++)
//	{
//		pRulesStorage->Storage[i]->ChainStr(rule1, rule1);
//		fprintf(data1,"%s\n", rule1);
//	}
//	fclose(data1);
////DEBUG_E
	this->nCodesCount = pSeqStorage->getCodesCount();
	//For ideals	
	VMatrix = new double* [lSeqLength];
	for ( unsigned long ColCounter = 0; ColCounter < lSeqLength; ColCounter++ )
	{
		VMatrix[ColCounter] = new double[nCodesCount];
		for ( int RowCounter = 0; RowCounter < nCodesCount; RowCounter++ )
		{
			VMatrix[ColCounter][RowCounter] = 0;
		}
	}

	p_AplicReg.reserve(lRuleStoreSize);
	p_SubsetRules.reserve(lRuleStoreSize);

	FilterSize = 0;
	FilterSize2 = 0;
	id=0;
//	__DEBUG_IO_COUNTER = 0;
	chObject = new char[MYMAXSTR];
	CurrIdelObject = NULL;

	this->ObjsStorage = ObjsStorage;
	

	return 0;
}

void Classificator::Restore(SEQStorage* pSeqStorage, RulesStorage* pRulesStorage, CIdelObject** ObjsStorage, long SCounter)
{
	Create(pSeqStorage, pRulesStorage, ObjsStorage);
	lObjsStorageCounter = SCounter;

}

//bool Classificator::isBelong(CIdelObject* obj, SToken* Token)
//{
//	//may be Token point to gap
//	if(GetCodeTable()->getCodeByToken("0") == Token->nValue)
//		return false;
//	else
//		obj->isBelong(Token);
//}

void Classificator::GenClasses()
{
    if(m_source == __IDEALSOURCE_REGULARITIES_)
        WorkUpRules();
    else if(m_source == __IDEALSOURCE_ORIGINOBJS_)
        WorkUpData();
}
            
void Classificator::WorkUpData()
{
    int nClNumber;
    int SeqCount = pSeqStorage->getLength();
	#ifdef TEST_LOG
		FILE* f = fopen("classes.txt", "w");
		char original[MYMAXSTR];
		FILE* f2 = fopen("transformation.txt", "w");
	#endif
	for (int SeqCounter = 0; SeqCounter < SeqCount; SeqCounter++)
	{
		//Report->UpdateStatus("remaining objects: " + std::to_string(SeqCount - SeqCounter));
		//if ( !ContinueProcess ) break;
		SEQSElem* CurrSeq = new SEQSElem(pSeqStorage->getWidth());
		pSeqStorage->CreateElem(SeqCounter, CurrSeq);
		
		nClNumber = NextIter(CurrSeq);
		pSeqStorage->setClass(SeqCounter, nClNumber);
		Carcass::lObjsStorageSize = lObjsStorageCounter;

		#ifdef TEST_LOG
			fprintf(f, "%i\t%i\n", 
				SeqCounter+1,
				nClNumber+1);
			strcpy(original, pSeqStorage->printSeq(CurrSeq).c_str());
			fprintf(f2, "%i\t%i\n%s\n%s\n\n",
				SeqCounter + 1,
				nClNumber + 1,
				original,
				chObject);
		#endif
		delete CurrSeq;
	}
	#ifdef TEST_LOG
		fclose( f );
	#endif
    if(pSeqStorage->LrnNegStoreLength > 0)
    {
		SeqCount = pSeqStorage->LrnNegStoreLength;
		for (int SeqCounter = 0; SeqCounter < SeqCount; SeqCounter++)
		{
			//Report->UpdateStatus("remaining neg objs: " + std::to_string(SeqCount - SeqCounter));
			//if ( !ContinueProcess ) break;
			SEQSElem* CurrSeq = new SEQSElem(pSeqStorage->getWidth());
			pSeqStorage->CreateElem(SeqCounter, CurrSeq);

			nClNumber = OneIter(CurrSeq, SeqCounter + 1);
/*          if ( nClNumber == -1)
					{
							for ( int nClassNumber = 0; nClassNumber < lObjsStorageSize; nClassNumber++)
							{
							double FVal, MaxFVal;
							CurrSeq = &pSeqStorage->LrnNegStorage[SeqCounter];
								FVal = myClassificator->CountF( CurrSeq, SeqCounter, nClassNumber );
									if (nClassNumber == 0)
									{
											MaxFVal=FVal;
											nClNumber = 0;
									} else if ( MaxFVal < FVal)
									{
											MaxFVal=FVal;
											nClNumber = nClassNumber;
									}
							}

					}
*/
            pSeqStorage->LrnNegStorage[SeqCounter].SetClassId(nClNumber);
			//lObjsStorageSize = lObjsStorageCounter;
			delete CurrSeq;
		}
	}
}

void Classificator::WorkUpRules()
{
    int nClNumber;
	int RuleCount = pRulesStorage->getSize();

	for (int RuleCounter = 0; RuleCounter < RuleCount; RuleCounter++)
	{
		//Report->UpdateStatus("remaining regs: " + std::to_string(RuleCount - RuleCounter));
		//if ( !ContinueProcess ) break;
		Rule* CurrRule = pRulesStorage->ConvertFromLinkToRule(&(*pRulesStorage)[RuleCounter]);
		nClNumber = NextIter(CurrRule);
		pRulesStorage->setRuleClass( RuleCounter, nClNumber);
		//lObjsStorageSize = lObjsStorageCounter;
		delete CurrRule;
	}
}

//------------------------------------------------------------------------
//Один этап процедуры классификации
//найденный идеальный объект не сохраняется нигде
void Classificator::OneIter1(CIdelObject* Pattern)
{
	CurrIdelObject = Pattern;
	FillVMatrix();
	CurrIdelObject->setGamma(Count_GammaCryterion());
}

bool Classificator::OneIter2(CIdelObject* Pattern)
{
	CurrIdelObject = Pattern;
	bool FinishFlag = true;

	SearchForAction(CurrIdelObject);
	FinishFlag = ActionExtremum(CurrIdelObject);
	
	if(FinishFlag)
	{
		Pattern = CurrIdelObject;
		return true;
	}
	
	DelZeroFeature(CurrIdelObject);
	Pattern = CurrIdelObject;

	return false;

}

bool Classificator::One1Iter(SEQSElem* Pattern)
{

	CurrIdelObject = new CIdelObject();
	CurrIdelObject->Create(lSeqLength, nCodesCount, Pattern);
	bool FinishFlag = true;

	FillVMatrix();
	SearchForAction(CurrIdelObject);
	FinishFlag = ActionExtremum(CurrIdelObject);
	if(FinishFlag)
		return true;
	
	DelZeroFeature(CurrIdelObject);
	delete CurrIdelObject;
	return false;

}

void Classificator::getObjAsStr(SEQSElem* Pattern, char* chObject)
{
	CIdelObject TempObject;
	TempObject.Create(lSeqLength, nCodesCount, Pattern);
	TempObject.getObjAsStr(chObject);
}
//------------------------------------------------------------------------
//Процедура классификации, то есть таже идеализация, но
//найденный идеальный объект не сохраняется в множестве если он новый
long Classificator::OneIter(SEQSElem* NextSeq, long IterCounter)
{
	m_NextSeq = NextSeq;

//	ObjsStorage[lObjsStorageCounter].Create(lSeqLength, nCodesCount, NextSeq);
//	CurrIdelObject = &ObjsStorage[lObjsStorageCounter];

	CurrIdelObject = new CIdelObject();
	CurrIdelObject->Create(lSeqLength, nCodesCount, m_NextSeq);

//        IdealizType = 2;

	switch ( IdealizType )
	{
		case __IDEALIZTYPE_DELETEFIRST_:
		{
			DelFInsABuild();
		     	CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_INSERTFIRST_:
		{

			InsFDelABuild();
  			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_DELETEINSERT_:
		{
			DelInsBuild();
	 //		CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_OFFICIAL_:
		{
			MaxDelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
	}

	bool bObjIsUnique;
	bool bToCompare;
	bool bForCompare;
	SToken CompareToken;
	bObjIsUnique = true;
	long lClassNumber = 0;
	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageCounter; lObjsCounter++ )
	{
		bObjIsUnique = false;
		for ( CompareToken.nPos = 0; CompareToken.nPos < lSeqLength; CompareToken.nPos++ )
		{
			for ( CompareToken.nValue = 0; CompareToken.nValue < nCodesCount; CompareToken.nValue++ )
			{
				bForCompare = ObjsStorage[lObjsCounter]->isBelong(&CompareToken);
				bToCompare = CurrIdelObject->isBelong(&CompareToken);
				if ( bForCompare != bToCompare )
				{
					bObjIsUnique = true;
					break;
				}
			}
			if ( bObjIsUnique )
			{
				break;
			}
		}
		if ( !bObjIsUnique )
		{
			lClassNumber = lObjsCounter;
			break;
		}
	}

	delete CurrIdelObject;
	if ( bObjIsUnique )
	{
		return -1;
	}
	else
	{
		return(lClassNumber);
	}

}
//------------------------------------------------------------------------
//Процедура классификации по закону, то есть та же идеализация, но
//найденный идеальный объект не сохраняется в множестве если он новый
long Classificator::OneIter(Rule* NextReg, long IterCounter)
{
	CurrIdelObject = new CIdelObject;
	CurrIdelObject->Create(lSeqLength, nCodesCount, NextReg);
	switch ( IdealizType )
	{
		case __IDEALIZTYPE_DELETEFIRST_:
		{
			DelFInsABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_INSERTFIRST_:
		{

			InsFDelABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_DELETEINSERT_:
		{
			DelInsBuild();
	 //		CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_OFFICIAL_:
		{
			MaxDelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
	}

	bool bObjIsUnique;
	bool bToCompare;
	bool bForCompare;
	SToken CompareToken;
	bObjIsUnique = true;
	long lClassNumber = 0;
	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageCounter; lObjsCounter++ )
	{
		bObjIsUnique = false;
		for ( CompareToken.nPos = 0; CompareToken.nPos < lSeqLength; CompareToken.nPos++ )
		{
			for ( CompareToken.nValue = 0; CompareToken.nValue < nCodesCount; CompareToken.nValue++ )
			{
				bForCompare = ObjsStorage[lObjsCounter]->isBelong(&CompareToken);
				bToCompare = CurrIdelObject->isBelong(&CompareToken);
				if ( bForCompare != bToCompare )
				{
					bObjIsUnique = true;
					break;
				}
			}
			if ( bObjIsUnique )
			{
				break;
			}
		}
		if ( !bObjIsUnique )
		{
			lClassNumber = lObjsCounter;
			break;
		}
	}

	delete CurrIdelObject;
	if ( bObjIsUnique )
	{
		return -1;
	}
	else
	{
		return(lClassNumber);
	}

}

// Поиск закономерностей применимых к начальному набору и опредление их выполнимости
int Classificator::DefineRule( int nRuleNumber, int nClNumber)
{
	SToken CurrToken;
	SToken SendToken;
	bool isApplicable;
	bool isCheckedOut;
	long nRuleLength;
	
	isApplicable = true;
	isCheckedOut = false;
	
	nRuleLength = (*pRulesStorage)[nRuleNumber].getRuleLength();
	for (long l=0; l < nRuleLength; l++)
	{
			SendToken.nPos = (*pRulesStorage)[nRuleNumber][l].Shift;
			SendToken.nValue = (*pRulesStorage)[nRuleNumber][l].Value;
			SendToken.Sign = (*pRulesStorage)[nRuleNumber][l].Sign;

			if ( (SendToken.Sign < 0) )
			{
					if ( ObjsStorage[nClNumber]->isBelong(&SendToken))
					{
							isApplicable = false;
							break;
					}
					isApplicable = false;

					if ( !IsAloneIdOb(&SendToken, nClNumber))
					{
							isApplicable = true;
					}

			}
			else if ( (SendToken.Sign >= 0) && (!ObjsStorage[nClNumber]->isBelong(&SendToken)) )
			{
					isApplicable = false;
					break;
			}
	}

	if ( isApplicable)   //закономерность применима
	{
		CurrToken.nPos = (*pRulesStorage)[nRuleNumber].getTTPos();
		CurrToken.nValue = (*pRulesStorage)[nRuleNumber].getTTValue();
		CurrToken.Sign = (*pRulesStorage)[nRuleNumber].getTTSign();

		if ((CurrToken.Sign == 1) && (ObjsStorage[nClNumber]->isBelong(&CurrToken)) ||
			(CurrToken.Sign == -1) && (!ObjsStorage[nClNumber]->isBelong(&CurrToken)))
		{
				isCheckedOut = true; // если законмерность подтверждается
				ObjsStorage[nClNumber]->RulMarks.push_back(short(nRuleNumber));
				return(1);
		}
		else
		{
				//ObjsStorage[nClNumber]->RulMarks.push_back(nRuleNumber);
				return(2);  //закономерность опровергается
		}
	}
	return(0);//не применима
}
//Составляет матрицу V (матрица предсказаний) для заданного идеального объекта
void Classificator::CountMatrix(int nClNumber )
{
	CurrIdelObject = new CIdelObject;
	CurrIdelObject = ObjsStorage[nClNumber];
   //     	SToken CurrToken;
   //             CurrToken.nPos = 9;
   //             for ( CurrToken.nValue = 0; CurrToken.nValue < 4; CurrToken.nValue++)
   //             CurrIdelObject->ExcludeT(&CurrToken);

		this->FillVMatrix();
}
void Classificator::CountMatrix(CIdelObject* Cl)
{
	CurrIdelObject = Cl;
	this->FillVMatrix();
}
//Выдает поэлементно матрицу V
void Classificator::Pred_VMatrixVal(long pos, int cod, double& val )
{
//        double val;
		val = VMatrix[pos][cod];
//        return(val);

}
double Classificator::Get_VMatrixVal(long pos, int cod)
{
	return VMatrix[pos][cod];
}
//Считает матрицу распознавания, т.е. для элемента который предсказывается
//с отрицательным показателем считаются значения всех закономерностей
//в которых он участвует в посылке
void Classificator::Rec_VMatrixVal(long pos, int cod, double& val  )
{
//        double* val;
//        val = &VMatrix[pos][cod];
//        val = VMatrix[pos][cod];
	SToken CurrToken;

        CurrToken.nPos = pos;
        CurrToken.nValue = cod;

        if ( CurrIdelObject->isBelong(&CurrToken) )
        {
                val = fabs(ExcludeTest(&CurrToken));
  //
 //               val = ExcludeTest(&CurrToken);
        }
        else
        {
                val = IncludeTest(&CurrToken);
                //////////////////////
/*                CurrIdelObject->IncludeT(&CurrToken);
                FillVMatrix();
                val = fabs(ExcludeTest(&CurrToken));
                CurrIdelObject->ExcludeT(&CurrToken);
*/                //////////////////////
		}

//        if ( *val <= 0 )
 /*       if ( val <= 0 )
		{

           bool isApplicable;
           int nRuleLength;
           SToken SendToken;
           SToken CurrToken;
           double val2=0.;
           double MaxVal,dTTZDevVal;
           int CheckVal;

           CurrToken.nPos =  pos;
		   CurrToken.nValue =  cod;
           CurrToken.Sign = 1;
		   if ( !CurrIdelObject->isBelong(&CurrToken))
           {
                CurrToken.Sign = -1;
           }

		   for (long RulesCounter = 0 ; RulesCounter < this->FilterSize; RulesCounter++)
           {
                isApplicable = false;
                nRuleLength = this->RulesFilter[RulesCounter].getRuleLength();
                for (int l=0; l < nRuleLength; l++)
                {
        	       SendToken.nPos = this->RulesFilter[RulesCounter][l].Shift;
	               SendToken.nValue = this->RulesFilter[RulesCounter][l].Value;
		       SendToken.Sign = this->RulesFilter[RulesCounter][l].Sign;

                        if ( (SendToken.nPos == pos) && (SendToken.nValue == cod) && (SendToken.Sign > 0) && (CurrToken.Sign > 0))
                        {
                                isApplicable = true;
                                break;
                        }
                        else if( (SendToken.nPos == pos) && (SendToken.nValue != cod) && (SendToken.Sign < 0) && (CurrToken.Sign < 0))
                        {
                                isApplicable = true;
                                break;
                        }
        	}
                if ( isApplicable)
                {




				CurrToken.nPos = this->RulesFilter[RulesCounter].getTTPos();
                MaxVal = 0.;
	        for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
	        {
			dTTZDevVal = this->RulesFilter[RulesCounter].getTTZDevVal(CurrToken.nValue);
                        if ( (fabs(dTTZDevVal) > fabs(MaxVal)) )
                        {
                                MaxVal = dTTZDevVal;
                                CheckVal = CurrToken.nValue;
                        }
                }

                CurrToken.nValue = CheckVal;

                if ( (MaxVal > (double)0) && (CurrIdelObject->isBelong(&CurrToken)) )
                {
                        val2 = val2 - (log(1 - MaxVal)); // если законмерность выполняется
                }
                else if ( (MaxVal < (double)0) && (!CurrIdelObject->isBelong(&CurrToken))  )
                {
                        val2 = val2 - (log(1- fabs(MaxVal)));  // если законмерность выполняется
                }
                else if ( (MaxVal > (double)0) && (!CurrIdelObject->isBelong(&CurrToken)) )
                {
                        val2 = val2 + (log(1 - MaxVal));   // если законмерность не выполняется
                }
                else if ( (MaxVal < (double)0) && (CurrIdelObject->isBelong(&CurrToken)))
                {
                        val2 = val2 + (log(1 - fabs(MaxVal)));   // если законмерность не выполняется
                }
                }
           }

           val = val2;
         //  return(&val2);
        }

//        return(val);
*/
}
// Вычисление оценки 1 для заданного класса
// Прямой счет вклада всех последовательностей
double Classificator::Count_GammaCryterion( )
{
	double fGamma=0.;

	for ( unsigned long pos = 0; pos < lSeqLength; pos++)
		for ( int cod = 0; cod < nCodesCount; cod++)
			fGamma+= VMatrix[pos][cod];
	return fGamma;
}
//---------------------------------------------------------------------------
//Закомментированная часть содержит обращение к старым массивам Rule*
//Надо переписать, если нужен функционал данного метода
double Classificator::CountF(SEQSElem* NextSeq, int nSeqNumb, int nClNumber)
{
//	m_NextSeq = NextSeq;
//
//
//	CurrIdelObject = new CIdelObject;
//	CurrIdelObject->Create(lSeqLength, nCodesCount, m_NextSeq);
//
////debugmode_B
////	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
////debugmode_E
//        // Поиск закономерностей применимых к начальному набору
//		FilterSize = 0;
//    	FilterSize2 = 0;
//	SToken CurrToken;
//	SToken SendToken;
//	bool isApplicable;
//        bool isCheckedOut;
//        int CheckVal;
//	long nRuleLength;
//        double MaxVal;
//
//        Filtrovka();
//
//     //////////////////
///*     FilterSize = lRuleStoreSize;
//     for (long lRulesCount=0; lRulesCount < FilterSize;lRulesCount++)
//	 {
//		RulesFilter[lRulesCount] =  (*pRulesStorage)[lRulesCount];
//     }
//*/     ///////////////////
//
//
//  	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < FilterSize; lRulesStoreCounter++)
//	{
//               isApplicable = true;
//               isCheckedOut = false;
//               CheckVal = -1;
//			   MaxVal = 0.0;
//
//               nRuleLength = RulesFilter[lRulesStoreCounter].getRuleLength();
//               for (long l=0; l < nRuleLength; l++)
//               {
//        	       SendToken.nPos = RulesFilter[lRulesStoreCounter][l].Shift;
//	               SendToken.nValue = RulesFilter[lRulesStoreCounter][l].Value;
//		       SendToken.Sign = RulesFilter[lRulesStoreCounter][l].Sign;
//
//
//        	       	if ( (SendToken.Sign < 0) )
//	        	{
//								if ( ObjsStorage[nClNumber]->isBelong(&SendToken))
//                                {
//			                isApplicable = false;
//		        	        break;
//                                }
//                                isApplicable = false;
//
//                                if ( !IsAloneIdOb(&SendToken, nClNumber))
//                                {
//                                        isApplicable = true;
//                                }
//
//        		}
//	        	else if ( (SendToken.Sign >= 0) && (!ObjsStorage[nClNumber]->isBelong(&SendToken)) )
//		        {
//			        isApplicable = false;
//        			break;
//	        	}
//			}
//
//                if ( isApplicable)
//				{
//					CurrToken.nPos = RulesFilter[lRulesStoreCounter].getTTPos();
//					MaxVal = RulesFilter[lRulesStoreCounter].getCP();
//					CheckVal = RulesFilter[lRulesStoreCounter].getTTValue();
//			/*    for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
//				{
//						DevVal = RulesFilter[lRulesStoreCounter].getTTZDevVal(CurrToken.nValue);
//						if ( fabs(DevVal) > fabs(MaxVal))
//						{
//								MaxVal = DevVal;
//								CheckVal = CurrToken.nValue;
//						} else if ( (fabs(DevVal) == fabs(MaxVal)) && (DevVal > MaxVal))
//						{
//								MaxVal = DevVal;
//								CheckVal = CurrToken.nValue;
//						}
//
//				}
//			  */
//			  CurrToken.nValue = CheckVal;
//                if ( (MaxVal > (double)0) && (ObjsStorage[nClNumber]->isBelong(&CurrToken)) )
//                {
//                        isCheckedOut = true; // если законмерность выполняется
//                }
//                else if ( (MaxVal < (double)0) && (!ObjsStorage[nClNumber]->isBelong(&CurrToken)) )
//                {
//                        isCheckedOut = true; // если законмерность выполняется
//                }
//                }
//
//		if ( (isApplicable) && (isCheckedOut))
//		{
//                        RulesFilter2[FilterSize2] = RulesFilter[lRulesStoreCounter];
//			FilterSize2++;
//		}
//
//	}
//
//
//
//	double* VmatrixCell;
//
//	for ( CurrToken.nPos = 0; CurrToken.nPos < lSeqLength; CurrToken.nPos++ )
//	{
//		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
//		{
//			VMatrix[CurrToken.nPos][CurrToken.nValue] = 0;
//		}
//	}
//
///*
//    // Заполнение матрицы V
//    for (long RulesCounter = 0 ; RulesCounter < this->FilterSize2; RulesCounter++)
//    {
//            CurrToken.nPos = this->RulesFilter2[RulesCounter].getTTPos();
//        for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
//        {
//	        VmatrixCell = &VMatrix[CurrToken.nPos][CurrToken.nValue];
//		dTTZDevVal = this->RulesFilter2[RulesCounter].getTTZDevVal(CurrToken.nValue);
//                    //  ^
//                    //  взяли признак являющийся целевым объектом
//                    if ( (dTTZDevVal > (double)0) && (CurrIdelObject->isBelong(&CurrToken)) )
//                    {
//                            *VmatrixCell = *VmatrixCell - (log(1 - dTTZDevVal)); // если законмерность выполняется
//                    }
//                    else if ( (dTTZDevVal < (double)0) && (!CurrIdelObject->isBelong(&CurrToken))  )
//                    {
//                            *VmatrixCell = *VmatrixCell - (log(1- fabs(dTTZDevVal)));  // если законмерность выполняется
//                    }
//                    else if ( (dTTZDevVal > (double)0) && (!CurrIdelObject->isBelong(&CurrToken)) )
//                    {
//                            *VmatrixCell = *VmatrixCell + (log(1 - dTTZDevVal));   // если законмерность не выполняется
//                    }
//                    else if ( (dTTZDevVal < (double)0) && (CurrIdelObject->isBelong(&CurrToken)))
//					{
//                            *VmatrixCell = *VmatrixCell + (log(1 - fabs(dTTZDevVal)));   // если законмерность не выполняется
//                    }
//            }
//    }
//
//*/
//
//    for (long RulesCounter = 0 ; RulesCounter < this->FilterSize2; RulesCounter++)
//	{
//			CurrToken.nPos = this->RulesFilter2[RulesCounter].getTTPos();
//			MaxVal = this->RulesFilter2[RulesCounter].getCP();
//			CheckVal = this->RulesFilter2[RulesCounter].getTTValue();
//
//		  /*	CurrToken.nPos = this->RulesFilter2[RulesCounter].getTTPos();
//			MaxVal = 0.0;
//		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
//		{
//		dTTZDevVal = this->RulesFilter2[RulesCounter].getTTZDevVal(CurrToken.nValue);
//					if ( (fabs(dTTZDevVal) > fabs(MaxVal)) )
//					{
//							MaxVal = dTTZDevVal;
//							CheckVal = CurrToken.nValue;
//					} else if ( (fabs(dTTZDevVal) == fabs(MaxVal)) && (dTTZDevVal > MaxVal))
//					{
//							MaxVal = dTTZDevVal;
//							CheckVal = CurrToken.nValue;
//					}
//			}
//			*/
//            CurrToken.nValue = CheckVal;
//            VmatrixCell = &VMatrix[CurrToken.nPos][CurrToken.nValue];
////                dTTZDevVal = this->RulesFilter2[RulesCounter].getTTZDevVal(CurrToken.nValue);
//            //  ^
//            //  взяли признак являющийся целевым объектом
//            if ( (MaxVal > (double)0) && (CurrIdelObject->isBelong(&CurrToken)) )
//            {
//                    *VmatrixCell = *VmatrixCell - (log(1 - MaxVal)); // если законмерность выполняется
//            }
//            else if ( (MaxVal < (double)0) && (!CurrIdelObject->isBelong(&CurrToken))  )
//            {
//                    *VmatrixCell = *VmatrixCell - (log(1- fabs(MaxVal)));  // если законмерность выполняется
//            }
//            else if ( (MaxVal > (double)0) && (!CurrIdelObject->isBelong(&CurrToken)) )
//            {
//                    *VmatrixCell = *VmatrixCell + (log(1 - MaxVal));   // если законмерность не выполняется
//            }
//            else if ( (MaxVal < (double)0) && (CurrIdelObject->isBelong(&CurrToken)))
//            {
//                    *VmatrixCell = *VmatrixCell + (log(1 - fabs(MaxVal)));   // если законмерность не выполняется
//            }
//    }
//
//    double FValue = 0;
//
//	for ( CurrToken.nPos = 0; CurrToken.nPos < lSeqLength; CurrToken.nPos++ )
//	{
//		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
//		{
//			FValue = FValue + VMatrix[CurrToken.nPos][CurrToken.nValue];
//		}
//	}
//
//	return(FValue);
return .0;

}
//Процедура поиска идеального объекта
long Classificator::NextIter(SEQSElem* NextSeq, int NextRuleNumber)
{
		/*
		*	Шаг I
		*/
		if (NextSeq != NULL )
		{
			m_NextSeq = NextSeq;
			CurrIdelObject = new CIdelObject;
			CurrIdelObject->Create(lSeqLength, nCodesCount, m_NextSeq);
			lIterCounter++;
		}
		else if (NextRuleNumber != -1 )
		{
			CurrIdelObject = new CIdelObject;
			CurrIdelObject->Create(lSeqLength, nCodesCount/*, &(*pRulesStorage)[NextRuleNumber]*/);
		}

		// _DEBUG__B
//				CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		// _DEBUG__E
	switch ( IdealizType )
	{
		case __IDEALIZTYPE_DELETEFIRST_:
		{
			DelFInsABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_INSERTFIRST_:
		{

			InsFDelABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_DELETEINSERT_:
		{
			DelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_OFFICIAL_:
		{
			MaxDelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
	}

//	bool bObjIsUnique;
	bool bToCompare;
	bool bForCompare;
	SToken CompareToken;
		bObjIsUnique = true;
	long lClassNumber = 0;
//-------------------
	SToken CurrToken;

	double FValue = 0;

	for ( CurrToken.nPos = 0; CurrToken.nPos < lSeqLength; CurrToken.nPos++ )
	{
		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
		{
			FValue = FValue + VMatrix[CurrToken.nPos][CurrToken.nValue];
		}
	}
//-------------------


	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageCounter; lObjsCounter++ )
	{
		bObjIsUnique = false;
		for ( CompareToken.nPos = 0; CompareToken.nPos < lSeqLength; CompareToken.nPos++ )
		{
			for ( CompareToken.nValue = 0; CompareToken.nValue < nCodesCount; CompareToken.nValue++ )
			{
				bForCompare = ObjsStorage[(unsigned)lObjsCounter]->isBelong(&CompareToken);
				bToCompare = CurrIdelObject->isBelong(&CompareToken);
				if ( bForCompare != bToCompare )
				{
					bObjIsUnique = true;
					break;
				}
			}
			if ( bObjIsUnique )
			{
				break;
			}
		}
		if ( !bObjIsUnique )
		{
			lClassNumber = lObjsCounter;
			break;
		}
	}
	if ( bObjIsUnique )
	{
		CurrIdelObject->getObjAsStr(chObject);
//                MainForm->IdealObjPopupMenu->AutoPopup = true;
//		MainForm->OnObjsStorageAdd(chObject, CurrIdelObject->getGamma(), lObjsStorageCounter);
//		if (NextSeq != NULL ) MainForm->SetClassNumber(lIterCounter, lObjsStorageCounter + 1);
//                else MainForm->SetRuleClassNumber(NextRuleNumber + 1, lObjsStorageCounter + 1);
		ObjsStorage[lObjsStorageCounter] = CurrIdelObject;
		CurrIdelObject = NULL;
		lClassNumber = lObjsStorageCounter;
		lObjsStorageCounter++;
	}
	else
	{
		if (CurrIdelObject != NULL)
		{
//			if (NextSeq != NULL ) MainForm->SetClassNumber(lIterCounter, lClassNumber + 1);
//                        else MainForm->SetRuleClassNumber(NextRuleNumber + 1, lClassNumber + 1);
			delete CurrIdelObject;
		}
		CurrIdelObject = NULL;
	}

//	return lObjsStorageCounter;
	return lClassNumber;
}

//Процедура поиска идеального объекта
long Classificator::NextIter(SEQSElem* NextSeq)
{
	/*
	*	Шаг I
	*/
	m_NextSeq = NextSeq;
	CurrIdelObject = new CIdelObject();
	CurrIdelObject->Create(m_NextSeq, pSeqStorage->getCodesCount());
	lIterCounter++;

	// _DEBUG__B
	CurrIdelObject->getObjAsStr(chObject);
	m_progress << "===-------   -------===\n\tПрограмма наблюдает объект \n" << chObject << "\n";
	ReportLog(m_progress.str());
	m_progress.str("");
	// _DEBUG__E
	
	switch ( IdealizType )
	{
		case __IDEALIZTYPE_DELETEFIRST_:
		{
			DelFInsABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_INSERTFIRST_:
		{

			InsFDelABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_DELETEINSERT_:
		{
			DelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_OFFICIAL_:
		{
			MaxDelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
	}

//	bool bObjIsUnique;
	bool bToCompare;
	bool bForCompare;
	SToken CompareToken;
		bObjIsUnique = true;
	long lClassNumber = 0;
//-------------------
	SToken CurrToken;

	double FValue = 0;

	for ( CurrToken.nPos = 0; CurrToken.nPos < lSeqLength; CurrToken.nPos++ )
	{
		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
		{
			FValue = FValue + VMatrix[CurrToken.nPos][CurrToken.nValue];
		}
	}
//-------------------


	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageCounter; lObjsCounter++ )
	{
		bObjIsUnique = false;
		for ( CompareToken.nPos = 0; CompareToken.nPos < lSeqLength; CompareToken.nPos++ )
		{
			for ( CompareToken.nValue = 0; CompareToken.nValue < nCodesCount; CompareToken.nValue++ )
			{
				bForCompare = ObjsStorage[lObjsCounter]->isBelong(&CompareToken);
				bToCompare = CurrIdelObject->isBelong(&CompareToken);
				if ( bForCompare != bToCompare )
				{
					bObjIsUnique = true;
					break;
				}
			}
			if ( bObjIsUnique )
			{
				break;
			}
		}
		if ( !bObjIsUnique )
		{
			lClassNumber = lObjsCounter;
			break;
		}
	}
	if ( bObjIsUnique )
	{
		CurrIdelObject->getObjAsStr(chObject);
		CurrIdelObject->setGamma(FValue);
		ObjsStorage[lObjsStorageCounter] = CurrIdelObject;
		CurrIdelObject = NULL;
		lClassNumber = lObjsStorageCounter;
		lObjsStorageCounter++;
	}
	else
	{
		if (CurrIdelObject != NULL)
		{
//			if (NextSeq != NULL ) MainForm->SetClassNumber(lIterCounter, lClassNumber + 1);
//                        else MainForm->SetRuleClassNumber(NextRuleNumber + 1, lClassNumber + 1);
			delete CurrIdelObject;
		}
		CurrIdelObject = NULL;
	}

//	return lObjsStorageCounter;
	return lClassNumber;
}
//Процедура поиска идеального объекта
long Classificator::NextIter(Rule* NextRule)
{
		/*
		*	Шаг I
		*/

			CurrIdelObject = new CIdelObject;
			//закономерность не содержит отрицаний
			if ( !CurrIdelObject->Create(lSeqLength, nCodesCount, NextRule) )
			{
				bObjIsUnique = false;
				return -1;
			}

		// _DEBUG__B
//				CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		// _DEBUG__E
	switch ( IdealizType )
	{
		case __IDEALIZTYPE_DELETEFIRST_:
		{
			DelFInsABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_INSERTFIRST_:
		{

			InsFDelABuild();
			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_DELETEINSERT_:
		{
			DelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
		case __IDEALIZTYPE_OFFICIAL_:
		{
			MaxDelInsBuild();
  //			CurrIdelObject->Minimize();
			break;
		}
	}

//	bool bObjIsUnique;
	bool bToCompare;
	bool bForCompare;
	SToken CompareToken;
		bObjIsUnique = true;
	long lClassNumber = 0;
//-------------------
	SToken CurrToken;

	double FValue = 0;

	for ( CurrToken.nPos = 0; CurrToken.nPos < lSeqLength; CurrToken.nPos++ )
	{
		for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++ )
		{
			FValue = FValue + VMatrix[CurrToken.nPos][CurrToken.nValue];
		}
	}
//-------------------


	for ( long lObjsCounter = 0; lObjsCounter < lObjsStorageCounter; lObjsCounter++ )
	{
		bObjIsUnique = false;
		for ( CompareToken.nPos = 0; CompareToken.nPos < lSeqLength; CompareToken.nPos++ )
		{
			for ( CompareToken.nValue = 0; CompareToken.nValue < nCodesCount; CompareToken.nValue++ )
			{
				bForCompare = ObjsStorage[lObjsCounter]->isBelong(&CompareToken);
				bToCompare = CurrIdelObject->isBelong(&CompareToken);
				if ( bForCompare != bToCompare )
				{
					bObjIsUnique = true;
					break;
				}
			}
			if ( bObjIsUnique )
			{
				break;
			}
		}
		if ( !bObjIsUnique )
		{
			lClassNumber = lObjsCounter;
			break;
		}
	}
	if ( bObjIsUnique )
	{
		CurrIdelObject->getObjAsStr(chObject);
//                MainForm->IdealObjPopupMenu->AutoPopup = true;
//		MainForm->OnObjsStorageAdd(chObject, CurrIdelObject->getGamma(), lObjsStorageCounter);
//		if (NextSeq != NULL ) MainForm->SetClassNumber(lIterCounter, lObjsStorageCounter + 1);
//                else MainForm->SetRuleClassNumber(NextRuleNumber + 1, lObjsStorageCounter + 1);
		ObjsStorage[lObjsStorageCounter] = CurrIdelObject;
		CurrIdelObject = NULL;
		lClassNumber = lObjsStorageCounter;
		lObjsStorageCounter++;
	}
	else
	{
		if (CurrIdelObject != NULL)
		{
//			if (NextSeq != NULL ) MainForm->SetClassNumber(lIterCounter, lClassNumber + 1);
//                        else MainForm->SetRuleClassNumber(NextRuleNumber + 1, lClassNumber + 1);
			delete CurrIdelObject;
		}
		CurrIdelObject = NULL;
	}

//	return lObjsStorageCounter;
	return lClassNumber;
}
//Алгоритм идеализации, в котором объекты сначала удаляются, а потом добавляются
void Classificator::DelFInsABuild()
{


//debugmode_B
//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//	MainForm->OnObjsChange(chObject);
//debugmode_E

	SToken* CurrToken = new SToken;
	SToken* Candidate = new SToken;

	double dMaxGammaIncrease;
	double dCurrGammaIncrease;

	bool FinishFlag = true;


	while ( FinishFlag )
	{
                FillVMatrix();
		dMaxGammaIncrease = 0.0;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( CurrIdelObject->isBelong(CurrToken) )
				{
					dCurrGammaIncrease = ExcludeTest(CurrToken);

					if ( dCurrGammaIncrease > dMaxGammaIncrease )
					{
						dMaxGammaIncrease = dCurrGammaIncrease;
						Candidate->nPos = CurrToken->nPos;
						Candidate->nValue = CurrToken->nValue;
					}
				}
			}
		}

		if ( dMaxGammaIncrease > 0. )
		{
			CurrIdelObject->ExcludeT(Candidate);
		// _debugmode__B
		//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		//	MainForm->OnObjsChange(chObject);
		// _debugmode__E
		}
		else
		{
			dMaxGammaIncrease = 0.0;
			FillVMatrix();
			for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
			{
				for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
				{
					if ( !CurrIdelObject->isBelong(CurrToken) )
					{
						dCurrGammaIncrease = IncludeTest(CurrToken);

						if ( dCurrGammaIncrease > dMaxGammaIncrease )
						{
							dMaxGammaIncrease = dCurrGammaIncrease;
							Candidate->nPos = CurrToken->nPos;
							Candidate->nValue = CurrToken->nValue;
						}
					}
				}
			}
			if ( dMaxGammaIncrease > 0. )
			{
				CurrIdelObject->IncludeT(Candidate);
			// _DEBUG__B
			//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
			//	MainForm->OnObjsChange(chObject);
			// _DEBUG__E
			}
			else
			{
				FinishFlag = false;
			}
		}
	}

	delete CurrToken;
	delete Candidate;


}

//Алгоритм идеализации, в котором объекты сначала добавляются, а потом удаляются
void Classificator::InsFDelABuild()
{

//debugmode_B
//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//	MainForm->OnObjsChange(chObject);
//debugmode_E

	SToken* CurrToken = new SToken;
	SToken* Candidate = new SToken;

	double dMaxGammaIncrease;
	double dCurrGammaIncrease;

	bool FinishFlag = true;

	while ( FinishFlag )
	{
                FillVMatrix();
		dMaxGammaIncrease = 0;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( !CurrIdelObject->isBelong(CurrToken) )
				{
					dCurrGammaIncrease = IncludeTest(CurrToken);

					if ( dCurrGammaIncrease > dMaxGammaIncrease )
					{
						dMaxGammaIncrease = dCurrGammaIncrease;
						Candidate->nPos = CurrToken->nPos;
						Candidate->nValue = CurrToken->nValue;
					}
				}
			}
		}

		if ( dMaxGammaIncrease > 0. )
		{
			CurrIdelObject->IncludeT(Candidate);
		// _debugmode__B
		//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		//	MainForm->OnObjsChange(chObject);
		// _debugmode__E
		}
		else
		{
                        FillVMatrix();
			dMaxGammaIncrease = 0;
			for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
			{
				for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
				{
					if ( CurrIdelObject->isBelong(CurrToken) )
					{
						dCurrGammaIncrease = ExcludeTest(CurrToken);

						if ( dCurrGammaIncrease > dMaxGammaIncrease )
						{
							dMaxGammaIncrease = dCurrGammaIncrease;
							Candidate->nPos = CurrToken->nPos;
							Candidate->nValue = CurrToken->nValue;
						}
					}
				}
			}
			if ( dMaxGammaIncrease > 0.)
			{
				CurrIdelObject->ExcludeT(Candidate);
			// _DEBUG__B
			//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
			//	MainForm->OnObjsChange(chObject);
			// _DEBUG__E
			}
			else
			{
				FinishFlag = false;
			}
		}
	}

	delete CurrToken;
	delete Candidate;
}

//---------------------------------------------------------------------------
//Алгоритм идеализации, в котором нет приоритетов для порядка действий
//удаляется или добавляется тот признак, который дает максимальный прирост
//взамосогласованности
void Classificator::DelInsBuild()
{
	File.open("rules.txt");

//debugmode_B
//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//	MainForm->OnObjsChange(chObject);
//debugmode_E

	SToken* CurrToken = new SToken;
	SToken* Candidate = new SToken;

	double dMaxGammaIncrease;
	double dCurrGammaIncrease;
		double Gamma,Gamma1;

	bool FinishFlag = true;

//debugmode_B
//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//debugmode_E



		FillVMatrix();
		Gamma = Count_GammaCryterion();
	while ( FinishFlag )
	{
		FinishFlag = false;
		dMaxGammaIncrease = 0.;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( CurrIdelObject->isBelong(CurrToken)  /*&& fabs(VMatrix[CurrToken->nPos][CurrToken->nValue])>=0.99*/ )
				{
					dCurrGammaIncrease = ExcludeTest(CurrToken);
//////////////////
/*    			                CurrIdelObject->ExcludeT(CurrToken);
                                        FillVMatrix();
                                        double grr;
					grr = IncludeTest(CurrToken);
			                CurrIdelObject->IncludeT(CurrToken);
                                        FillVMatrix();
                                        if ( fabs(dCurrGammaIncrease + grr) > 0.000001 )
                                        {
                                                grr = 2;
                                        }
*/                             ///////////////////

					if ( dCurrGammaIncrease > dMaxGammaIncrease )
					{
						dMaxGammaIncrease = dCurrGammaIncrease;
						Candidate->nPos = CurrToken->nPos;
						Candidate->nValue = CurrToken->nValue;
					}
				}
			}
		}

		if ( dMaxGammaIncrease > 0. )
		{
			CurrIdelObject->ExcludeT(Candidate);
			FinishFlag = true;
		// _debugmode__B
//			CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		//	MainForm->OnObjsChange(chObject);
		// _debugmode__E
			FillVMatrix();
		////////////////////

						Gamma1 = Count_GammaCryterion();
						double fdif = Gamma1 - Gamma;
						if ( fabs(fdif - dMaxGammaIncrease) > 0.0000001)
						{
						 //       int vangogh =1;
								double rrr = IncludeTest(Candidate);
								CurrIdelObject->IncludeT(Candidate);
								FillVMatrix();
								rrr = ExcludeTest(Candidate);
								CurrIdelObject->ExcludeT(Candidate);
								FillVMatrix();

						}
						if ( Gamma > Gamma1)
						{
								Gamma = Gamma1;
						}
						else
								Gamma = Gamma1;

		////////////////////
		}
		dMaxGammaIncrease = 0.;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( !CurrIdelObject->isBelong(CurrToken) /*&& fabs(VMatrix[CurrToken->nPos][CurrToken->nValue])>=0.99*/ )
				{
					dCurrGammaIncrease = IncludeTest(CurrToken);
							 //////////////////
	/*			                CurrIdelObject->IncludeT(CurrToken);
										FillVMatrix();
										double grr;
					grr = ExcludeTest(CurrToken);
							CurrIdelObject->ExcludeT(CurrToken);
										FillVMatrix();
										if ( fabs(dCurrGammaIncrease + grr) > 0.000001 )
										{
												grr = 2;
										}
	 */                        ///////////////////

					if ( dCurrGammaIncrease > dMaxGammaIncrease )
					{
						dMaxGammaIncrease = dCurrGammaIncrease;
						Candidate->nPos = CurrToken->nPos;
						Candidate->nValue = CurrToken->nValue;
					}
				}
			}
		}
		if ( dMaxGammaIncrease > 0. )
		{
			CurrIdelObject->IncludeT(Candidate);
			FinishFlag = true;
		// _DEBUG__B
//			CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//			MainForm->OnObjsChange(chObject);
		// _DEBUG__E
			FillVMatrix();
		////////////////////
						Gamma1 = Count_GammaCryterion();
						double fdif = Gamma1 - Gamma;
						if ( fabs(fdif - dMaxGammaIncrease) > 0.0000001)
						{
						//        int vangogh =1;
								double rrr = ExcludeTest(Candidate);
								CurrIdelObject->ExcludeT(Candidate);
								FillVMatrix();
								rrr = IncludeTest(Candidate);
								CurrIdelObject->IncludeT(Candidate);
								FillVMatrix();
						}

						if ( Gamma > Gamma1)
						{
								Gamma = Gamma1;
						}
						else
								Gamma = Gamma1;

        /////////////////////////////
		}
	}
/////////////////////////////////////
		// _DEBUG__B
//			CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
		// _DEBUG__E

		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
				{
					for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
                    {
						if ( CurrIdelObject->isBelong(CurrToken) )
						{
					dCurrGammaIncrease = ExcludeTest(CurrToken);

					if ( fabs(dCurrGammaIncrease) == double(0) )
					{
						CurrIdelObject->ExcludeT(CurrToken);
						FillVMatrix();
						// _DEBUG__B
//						CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
						// _DEBUG__E
					}
						}
					}
				}

	CurrIdelObject->setGamma(Count_GammaCryterion());
/////////////////////////////////////
	delete CurrToken;
	delete Candidate;
	File.close();
}

bool Classificator::ObviousPred(SToken vij)
{
	if(vij.nPos>=lSeqLength || vij.nValue>=nCodesCount)
		return false;
	else
	{
		RuleSection rs;
		rs.Shift = vij.nPos;
		rs.Value = vij.nValue;
		rs.Sign = vij.Sign;
		//if( /*nCodesCount==2 && */rs.Sign==-1 )	//случай когда работает закон исключенного третьего
		//{
		//	//нужно проверить противоположное значение
		//	rs.Sign = 1;
		//	if( rs.Value==0 ) rs.Value = 1;
		//	else rs.Value = 0;
		//}
		if( std::find(FiltrTargets.begin(), FiltrTargets.end(), rs) == FiltrTargets.end() )
			return false;
		else
			return true;
	}
	
	return false;
}

void Classificator::MaxDelInsBuild()
{
	double Gamma_Straight;
	bool FinishFlag = true;

	//InitObj = new char[MYMAXSTR];
	//CurrObj = new char[MYMAXSTR];
	//CurrIdelObject->getObjAsStr(InitObj);
	//CurrIdelObject->getObjAsStr(CurrObj);
	/*
	*	Шаг II
	*/
	FillVMatrix();
	Gamma_Straight = Count_GammaCryterion();
	CurrIdelObject->setGamma(Gamma_Straight);

	while ( FinishFlag )
	{

		SearchForAction(CurrIdelObject);
		FinishFlag = ActionExtremum(CurrIdelObject);
		
	}
	// удалени признаков с нулевым приростом Г критерия
	DelZeroFeature(CurrIdelObject);
	//delete[] CurrObj;
	//delete[] InitObj;
	//InitObj=NULL;
	//CurrObj=NULL;
}


void Classificator::SearchForAction(CIdelObject* i_Object)
{
	SToken* CurrToken = new SToken;
	double dCurrGammaIncrease;
	I_negative.clear();
	I_positive.clear();

	m_progress << "Имеем: " << CurrIdelObject->getGamma() <<"\n------------------\n";
	//перебираем признаки и смотрим прирост Г при удалении
	CurrToken->Sign = -1;
	for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
	{
		for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
		{
			if ( i_Object->isBelong(CurrToken) && ObviousPred(*CurrToken) )
			{
				dCurrGammaIncrease = ExcludeTest(CurrToken);

				if ( dCurrGammaIncrease > 0. )
				{
					SToken Candidate;
					Candidate.nPos = CurrToken->nPos;
					Candidate.nValue = CurrToken->nValue;

					I_negative.insert(std::make_pair(dCurrGammaIncrease, Candidate));
					m_progress << ">--\t может удалим " << GetCodeTable()->Decode( Candidate.nValue ) << " на позиции "<< Candidate.nPos<< "\n";
				}
			}
		}
	}
	//перебираем признаки и смотрим прирост Г при вставке
	CurrToken->Sign = 1;
	for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
	{
		for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
		{
			if ( !i_Object->isBelong(CurrToken) && ObviousPred(*CurrToken) )
			{
				dCurrGammaIncrease = IncludeTest(CurrToken);
				if ( dCurrGammaIncrease > 0. )
				{
					SToken Candidate;
					Candidate.nPos = CurrToken->nPos;
					Candidate.nValue = CurrToken->nValue;
					I_positive.insert(std::make_pair(dCurrGammaIncrease, Candidate));
					m_progress << ">--\t может вставим " << GetCodeTable()->Decode( Candidate.nValue ) << " на позицию "<< Candidate.nPos<< "\n";
				}
			}
		}
	}
	ReportLog(m_progress.str());
	m_progress.str("");

	delete CurrToken;
}

bool Classificator::ActionExtremum(CIdelObject* i_Object)
{
	double dMaxGamma_plus=0, dMaxGamma_minus=0;
	bool FinishFlag=false;

	rGI max_ex = I_negative.rbegin();
	rGI max_in = I_positive.rbegin();
	//ищем максимум для удаления с проверкой, что есть явное предсказание

	if( !I_negative.empty() )
		dMaxGamma_minus = (*max_ex).first;
	if( !I_positive.empty() )
		dMaxGamma_plus = (*max_in).first;
	std::pair<GI, GI> i1 = I_negative.equal_range( dMaxGamma_minus );
	std::pair<GI, GI> i2 = I_positive.equal_range( dMaxGamma_plus );
	//выбираем: вставка или удаление
	//
	//	Наибольшее предвосхищение достигается при удалении признака(ов)
	//
	if ( dMaxGamma_plus < dMaxGamma_minus )
	{
		//SToken Candidate_ = (*max_ex).second;
		//if(m_Conflict.nPos!=lSeqLength && m_Conflict.nPos==Candidate_.nPos && m_Conflict.nValue==Candidate_.nValue)
		//	FinishFlag = false;//найдено противоречие
		//else
		FinishFlag = true; //значит еще одна итерация
		//m_Conflict = (*max_ex).second;

		for(GI j = i1.first; j != i1.second; j++)
		{
			i_Object->ExcludeT(&(*j).second);
			m_progress	//<< "\tКритерий окажется равным [ " << Count_GammaCryterion() 
					<< " когда в объекте удалим атом на " << (*j).second.nPos 
					<< " месте со значением " << (*j).second.nValue;
		}
		//FillVMatrix();//V - целиком считать матрицу
		Filtrovka();//W - только обновления посмотреть		
		// _DEBUG__B
		CurrIdelObject->getObjAsStr(chObject);
		m_progress << "\n" << chObject << "\n";
		// _DEBUG__E
		
		ReportLog(m_progress.str());
		m_progress.str("");
		
		//========== идеал получен. записываем максимум =
		i_Object->setGamma(i_Object->getGamma() + dMaxGamma_minus);
	}
	//
	//	Наибольшее предвосхищение достигается одинаково при удалении и вставке признаков
	//
	else if ( dMaxGamma_plus > 0. && dMaxGamma_plus == dMaxGamma_minus )
	{
		FinishFlag = true; //значит еще одна итерация
		
		for(GI j = i1.first; j != i1.second; j++)
		{
			i_Object->ExcludeT(&(*j).second);
			m_progress	//<< "\tКритерий окажется равным [ " << Count_GammaCryterion() 
					<< " когда в объекте удалим атом на " << (*j).second.nPos 
					<< " месте со значением " << (*j).second.nValue;
		}

		for(GI j = i2.first; j != i2.second; j++)
		{
			i_Object->IncludeT(&(*j).second);
			m_progress	//<< "\tКритерий окажется равным [ " << Count_GammaCryterion() 
					<< " в объект вставим атом на " << (*j).second.nPos 
					<< " месте со значением " << (*j).second.nValue;
		}
		//FillVMatrix();//V - целиком считать матрицу
		Filtrovka();//W - только обновления посмотреть		

		// _DEBUG__B
		CurrIdelObject->getObjAsStr(chObject);
		m_progress << "\n" << chObject << "\n";
		// _DEBUG__E
		ReportLog(m_progress.str());
		m_progress.str("");
		
		//========== идеал получен. записываем максимум =
		i_Object->setGamma(i_Object->getGamma() + dMaxGamma_plus);
	}
	//
	//	Наибольшее предвосхищение достигается при вставке признака(ов)
	//
	else if ( dMaxGamma_plus > 0. )
	{
		//SToken Candidate_ = (*max_in).second;
		//if(m_Conflict.nPos!=lSeqLength && m_Conflict.nPos==Candidate_.nPos && m_Conflict.nValue==Candidate_.nValue)
		//	FinishFlag = false;//найдено противоречие
		//else
		FinishFlag = true; //значит еще одна итерация
		//m_Conflict = (*max_in).second;
		
		for(GI j = i2.first; j != i2.second; j++)
		{
			i_Object->IncludeT(&(*j).second);
			m_progress	//<< "\tКритерий окажется равным [ " << Count_GammaCryterion() 
					<< " в объект вставим атом на " << (*j).second.nPos 
					<< " месте со значением " << (*j).second.nValue;
		}
		//FillVMatrix();//V - целиком считать матрицу
		Filtrovka();//W - только обновления посмотреть		

		// _DEBUG__B
		CurrIdelObject->getObjAsStr(chObject);
		m_progress << "\n" << chObject << "\n";
		// _DEBUG__E
		ReportLog(m_progress.str());
		m_progress.str("");
		
		//========== идеал получен. записываем максимум =
		i_Object->setGamma(i_Object->getGamma() + dMaxGamma_plus);
	}

	return FinishFlag;
}

// удалени признаков с нулевым приростом Г критерия
void Classificator::DelZeroFeature(CIdelObject* i_Object)
{
	SToken* CurrToken = new SToken;
	double dCurrGammaIncrease;
//	CurrObj = new char[(nCodesCount + 3) * lSeqLength + 2];

	for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
	{
		for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
		{
			if ( i_Object->isBelong(CurrToken) )
			{
				dCurrGammaIncrease = ExcludeTest(CurrToken);
				if ( fabs(dCurrGammaIncrease) == double(0) )
				{
					i_Object->ExcludeT(CurrToken);
					m_progress	<< "$$$ есть признак без предсказаний: удалим атом на\n " << CurrToken->nPos
					<< " месте со значением " << CurrToken->nValue;
					ReportLog(m_progress.str());
					m_progress.str("");
					Filtrovka();
//					i_Object->getObjAsStr(CurrObj, GetCodeTable());
//					if( strcmp(AboutFrm->CheckObj1->Caption.t_str(), InitObj)==0 )
//						AboutFrm->ListStep1->Items->Add(CurrObj);
				}
			}
		}
	}

//	delete[] CurrObj;
	delete CurrToken;
}

void Classificator::OfficialBuild()
{
	SToken* CurrToken = new SToken;
	SToken LastPriznak;
//	SToken* Candidate2 = new SToken;

	double dMaxGamma_plus=0, dMaxGamma_minus=0;
	double dCurrGammaIncrease;
	double Gamma_Straight;
	bool FinishFlag = true;

//debugmode_B
//	CurrIdelObject->getObjAsStr(chObject, GetCodeTable());
//debugmode_E

	InitObj = new char[(nCodesCount + 3) * lSeqLength + 2];
	CurrObj = new char[(nCodesCount + 3) * lSeqLength + 2];
	CurrIdelObject->getObjAsStr(InitObj);
	//
	CurrIdelObject->getObjAsStr(CurrObj);
/*	if(strcmp(AboutFrm->CheckObj1->Caption.t_str(), InitObj)==0)
	{
		AboutFrm->ListStep1->Items->Add(CurrObj);
	}
	if(strcmp(AboutFrm->CheckObj2->Caption.t_str(), InitObj)==0)
	{
		AboutFrm->ListStep2->Items->Add(CurrObj);
	}
	*/
	/*
	*	Шаг II
	*/
	FillVMatrix();
	Gamma_Straight = Count_GammaCryterion();
	LastPriznak.nPos = lSeqLength;

	char* digit, *name;
	sprintf(digit, "%d", id);
	char ext[]=".bmp";
	name = new char[strlen(ext)+strlen(digit)+1];/* +1 - for \0 */
	strcpy(name, digit);
	strcat(name, ext);
//	CurrIdelObject->Print(name,GetCodeTable());

	while ( FinishFlag )
	{
		I_negative.clear();
		I_positive.clear();
		dMaxGamma_plus = 0;
		dMaxGamma_minus = 0;

		FinishFlag = false;
		//перебираем признаки и смотрим прирост Г при удалении
CurrToken->Sign = -1;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( CurrIdelObject->isBelong(CurrToken) && ObviousPred(*CurrToken) )
				{
					dCurrGammaIncrease = ExcludeTest(CurrToken);

					if ( dCurrGammaIncrease > 0. )
					{
						SToken Candidate;
						Candidate.nPos = CurrToken->nPos;
						Candidate.nValue = CurrToken->nValue;

						I_negative.insert(std::make_pair(dCurrGammaIncrease, Candidate));
					}
				}
			}
		}
 /*	FILE *data = fopen("i-negative.txt","w");
		fprintf(data,"negative size: %i\n", I_negative.size());
		GI p;
		for ( p=I_negative.begin(); p!=I_negative.end(); p++)
		{
				fprintf(data,"%f ", (*p).first);
		}
  */
		//перебираем признаки и смотрим прирост Г при вставке
CurrToken->Sign = 1;
		for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
		{
			for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
			{
				if ( !CurrIdelObject->isBelong(CurrToken) && ObviousPred(*CurrToken) )
				{
					dCurrGammaIncrease = IncludeTest(CurrToken);

					if ( dCurrGammaIncrease > 0. )
					{
						SToken Candidate;
						Candidate.nPos = CurrToken->nPos;
						Candidate.nValue = CurrToken->nValue;
						I_positive.insert(std::make_pair(dCurrGammaIncrease, Candidate));
					}
				}
			}
		}
		//
/*		fprintf(data,"positive size: %i\n", I_positive.size());
		for ( p=I_positive.begin(); p!=I_positive.end(); p++)
		{
				fprintf(data,"%f ", (*p).first);
		}
		fclose(data);
 */
		/*
		*	Шаг IV
		*/
		GI max_ex = I_negative.end();
		GI max_in = I_positive.end();
		//ищем максимум для удаления с проверкой, что есть явное предсказание
//		while( !I_negative.empty() && !ObviousPred((*(--max_ex)).second) )
//		{
//			I_negative.erase(max_ex);
//			max_ex = I_negative.end();
//		}
		if( !I_negative.empty() )
			dMaxGamma_minus = (*(--max_ex)).first;

		//ищем максимум для вставки с проверкой, что есть явное предсказание
//		while( !I_positive.empty() && !ObviousPred((*(--max_in)).second) )
//		{
//			I_positive.erase(max_in);
//			max_in = I_positive.end();
//		}
		if( !I_positive.empty() )
			dMaxGamma_plus = (*(--max_in)).first;

		//выбираем: вставка или удаление
		if ( dMaxGamma_plus < dMaxGamma_minus )
		{
			SToken Candidate_ = (*max_ex).second;
			if(LastPriznak.nPos!=lSeqLength && LastPriznak.nPos==Candidate_.nPos && LastPriznak.nValue==Candidate_.nValue)
				FinishFlag = false;//найдено противоречие
			else
				FinishFlag = true; //значит еще одна итерация

			LastPriznak = (*max_ex).second;
			CurrIdelObject->ExcludeT(&(*max_ex).second);


			FillVMatrix();
				CurrIdelObject->getObjAsStr(CurrObj);
/*
				if(strcmp(AboutFrm->CheckObj1->Caption.t_str(), InitObj)==0)
				{
					AboutFrm->ListStep1->Items->Add(CurrObj);
				}
				if(strcmp(AboutFrm->CheckObj2->Caption.t_str(), InitObj)==0)
				{
					AboutFrm->ListStep2->Items->Add(CurrObj);
				}
*/
			//t_CallbackOutput->Execute((void*)MM);// make pause on this step
	//		CurrIdelObject->UpdatePic(name,GetCodeTable());
		}
		else if ( dMaxGamma_plus > 0. )
		{
			SToken Candidate_ = (*max_in).second;
			if(LastPriznak.nPos!=lSeqLength && LastPriznak.nPos==Candidate_.nPos && LastPriznak.nValue==Candidate_.nValue)
				FinishFlag = false;//найдено противоречие
			else
				FinishFlag = true; //значит еще одна итерация


			LastPriznak = (*max_in).second;
			CurrIdelObject->IncludeT(&(*max_in).second);

			FillVMatrix();
			Gamma_Straight = Count_GammaCryterion();
				CurrIdelObject->getObjAsStr(CurrObj);
	/*			if(strcmp(AboutFrm->CheckObj1->Caption.t_str(), InitObj)==0)
				{
					AboutFrm->ListStep1->Items->Add(CurrObj);
				}
				if(strcmp(AboutFrm->CheckObj2->Caption.t_str(), InitObj)==0)
				{
					AboutFrm->ListStep2->Items->Add(CurrObj);
				}
*/
			//t_CallbackOutput->Execute((void*)MM);// make pause on this step
		
	//	CurrIdelObject->UpdatePic(name,GetCodeTable());
}
	}
//==========
// удалени признаков с нулевым приростом Г критерия
	for ( CurrToken->nPos = 0; CurrToken->nPos < lSeqLength; CurrToken->nPos++ )
	{
		for ( CurrToken->nValue = 0; CurrToken->nValue < nCodesCount; CurrToken->nValue++ )
		{
			if ( CurrIdelObject->isBelong(CurrToken) )
			{
				dCurrGammaIncrease = ExcludeTest(CurrToken);
				if ( fabs(dCurrGammaIncrease) == double(0) )
				{
					CurrIdelObject->ExcludeT(CurrToken);
					FillVMatrix();
		/*			CurrIdelObject->getObjAsStr(CurrObj, GetCodeTable());
					if(strcmp(AboutFrm->CheckObj1->Caption.t_str(), InitObj)==0)
					{
						AboutFrm->ListStep1->Items->Add(CurrObj);
					}
					if(strcmp(AboutFrm->CheckObj2->Caption.t_str(), InitObj)==0)
					{
						AboutFrm->ListStep2->Items->Add(CurrObj);
					}
					*/
		//			CurrIdelObject->UpdatePic(name,GetCodeTable());
				}
			}
		}
	}
	//========== идеал получен. записываем максимум =
	CurrIdelObject->setGamma(Count_GammaCryterion());

	id++;
	delete[] name;
	delete[] InitObj;
	delete[] CurrObj;
	InitObj=NULL;
	CurrObj=NULL;
	delete CurrToken;
}
void Classificator::SetObviousPred()
{
	FiltrTargets.clear();
	RuleSection prev, follow;
//FILE *seq = fopen("alltargets.txt","w");
	prev.Shift = 0;
	prev.Sign = 0;
	prev.Value = 0;
	for(int i=0; i<(int)FilterSize; i++)
	{
		follow.Shift = p_AplicReg[i]->getTTPos();
		follow.Sign = p_AplicReg[i]->getTTSign();
		follow.Value = p_AplicReg[i]->getTTValue();
		if(follow!=prev)
		{
//fprintf(seq,"%i %i %i\n", follow.Shift ,follow.Sign, follow.Value);
			FiltrTargets.push_back(follow);
		}
		prev = follow;
	}
	
	
	m_progress << "\tЯвно предсказываются " << FiltrTargets.size() << " различных атомов";
	ReportLog(m_progress.str());
	m_progress.str("");
	
//fclose(seq);
}

// Учтем уровень предсказания предикатов по вероятности у отобранных правил 
void Classificator::DiscountRule(RuleLink* pRule, RuleSection &prev)
{
	RuleSection CurrToken;
	double* VmatrixCell;
	bool isBelong;

	CurrToken.Shift = pRule->getTTPos();
	CurrToken.Value = pRule->getTTValue();//CurrToken.nValue = CheckVal;
	CurrToken.Sign = pRule->getTTSign();
	double MaxVal = pRule->getCP();
	isBelong = CurrIdelObject->isBelong(&CurrToken);
	VmatrixCell = &VMatrix[CurrToken.Shift][CurrToken.Value];
	if ((CurrToken.Sign == 1) && isBelong)
	{
			*VmatrixCell = *VmatrixCell + ProbFunc(MaxVal); // если законмерность подтверждается
	}
	else if ((CurrToken.Sign != 1) && !isBelong)
	{
			*VmatrixCell = *VmatrixCell + ProbFunc(fabs(MaxVal));  // если законмерность подтверждается
	}
	else if ((CurrToken.Sign == 1) && !isBelong)
	{
			*VmatrixCell = *VmatrixCell - ProbFunc(MaxVal);   // если законмерность опровергается
	}
	else if ((CurrToken.Sign != 1) && isBelong)
	{
			*VmatrixCell = *VmatrixCell - ProbFunc(fabs(MaxVal));   // если законмерность опровергается
	}

	//и явное предсказание подготавливаем
	if(CurrToken!=prev)
	{
		FiltrTargets.push_back(CurrToken);
	}
	prev = CurrToken;
}
//---------------------------------------------------------------------------
//Заполнение матрицы V
int Classificator::FillVMatrix()
{
 	RuleSection CurrToken;
	
	//init
	for ( CurrToken.Shift = 0; CurrToken.Shift < lSeqLength; CurrToken.Shift++ )
	{
		for ( CurrToken.Value = 0; CurrToken.Value < nCodesCount; CurrToken.Value++ )
		{
			VMatrix[CurrToken.Shift][CurrToken.Value] = 0;
		}
	}

	FiltrTargets.clear();
	RuleSection prev;
	prev.Shift = 0;
	prev.Sign = 0;
	prev.Value = 0;

	//отбираем правила, содержащие признаки, т.е.
	//совпадает предикат и его значение из посылки с элементом ид объекта
	FilterSize = 0;
	SToken SendToken;
	bool isApplicable;
	long nRuleLength;
	ruleID itRulesStorage = pRulesStorage->begin();
/////////////////////
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < lRuleStoreSize; lRulesStoreCounter++)
	{
		isApplicable = true;
		long l =0;
		RuleLink* pRule = &(*itRulesStorage);
		nRuleLength = pRule->getRuleLength();
		while (isApplicable && (l < nRuleLength))
		{
			SendToken.nPos = (*pRule)[l].Shift;
			SendToken.nValue = (*pRule)[l].Value;
			SendToken.Sign = (*pRule)[l].Sign;
			l++;

			//утверждаем, что какого-то определенного значения нет
			if ( SendToken.Sign < 0 )
			{
				//если мы отрицаем существующее значение,
				//то это конечно не_применимо_
				if ( CurrIdelObject->isBelong(&SendToken))
				{
						isApplicable = false;
						break;
				}
				isApplicable = false;
				//иначе если в объекте есть некоторое отличное значение x',
				//а отрицаем мы x, то такой случай _применим_
				if ( !IsAlone(&SendToken))
				{
					isApplicable = true;
				}

    		}
			else if ( (SendToken.Sign >= 0) && (!CurrIdelObject->isBelong(&SendToken)) )
	        {
		        isApplicable = false;
			}
		}

		if ( isApplicable )
		{
			//RulesFilter[FilterSize] = (*itRulesStorage);
			if((unsigned int)FilterSize < p_AplicReg.size())
				p_AplicReg[FilterSize] = pRule;
			else
				p_AplicReg.push_back( pRule );

			//эти правила предсказывают остальные признаки
			//составляем по целевым признакам(их значениям и вероятностям) матрицу V
			DiscountRule( pRule, prev );
			FilterSize++;
		}
		itRulesStorage++;

	}
//	p_AplicReg.resize(FilterSize);
	m_progress << "\tЗаполняем массив p_AplicReg " << FilterSize << " закономерностями";
	m_progress << "\n\tЯвно предсказываются " << FiltrTargets.size() << " различных атомов";
	ReportLog(m_progress.str());
	m_progress.str("");


	return(-1);
}
//---------------------------------------------------------------------------
// Поиск закономерностей применимых к начальному набору
//заполняет массив p_AplicReg
long Classificator::Filtrovka()
{
	FilterSize = 0;
	SToken SendToken;
	bool isApplicable;
	long nRuleLength;
	ruleID itRulesStorage = pRulesStorage->begin();
/////////////////////
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < lRuleStoreSize; lRulesStoreCounter++)
	{
		isApplicable = true;
		long l =0;
		RuleLink* pRule = &(*itRulesStorage);
		nRuleLength = pRule->getRuleLength();
		while (isApplicable && (l < nRuleLength))
		{
			SendToken.nPos = (*pRule)[l].Shift;
			SendToken.nValue = (*pRule)[l].Value;
			SendToken.Sign = (*pRule)[l].Sign;
			l++;

			//утверждаем, что какого-то определенного значения нет
			if ( SendToken.Sign < 0 )
			{
				//если мы отрицаем существующее значение,
				//то это конечно не_применимо_
				if ( CurrIdelObject->isBelong(&SendToken))
				{
						isApplicable = false;
						break;
				}
				isApplicable = false;
				//иначе если в объекте есть некоторое отличное значение x',
				//а отрицаем мы x, то такой случай _применим_
				if ( !IsAlone(&SendToken))
				{
					isApplicable = true;
				}

    		}
			else if ( (SendToken.Sign >= 0) && (!CurrIdelObject->isBelong(&SendToken)) )
	        {
		        isApplicable = false;
			}
		}

		if ( isApplicable )
		{
			//RulesFilter[FilterSize] = (*itRulesStorage);
			if((unsigned int)FilterSize < p_AplicReg.size())
				p_AplicReg[FilterSize] = pRule;
			else
				p_AplicReg.push_back( pRule );

			FilterSize++;
		}
		itRulesStorage++;

	}
	p_AplicReg.resize(FilterSize);
	m_progress << "\tЗаполняем массив p_AplicReg " << FilterSize << " закономерностями";
	ReportLog(m_progress.str());
	m_progress.str("");

	//берем только СВЗ
	//FilterSize = DelSubRules(p_AplicReg, FilterSize);
	
	//m_progress << "\tИз них СВЗ только " << FilterSize;
	//ReportLog(m_progress.str());
	//m_progress.str("");
	
	
//	//DEBUG_BEGIN
//FILE *seq = fopen("rule.txt","w");
//fprintf(seq,"------Filtrovka-------\n");
//char Chain[MYMAXSTR];
//	for(int s=0; s<FilterSize; s++)
//	{
//		
//		p_AplicReg[s]->getChainStr(Chain);
//		fprintf(seq,"%s\n", Chain);
//	}
//fclose(seq);
////DEBUG_END

	return FilterSize;
}
// ========
void Classificator::PrintRules(Rule* Massiv, long Size, std::streambuf *File)
{
	std::cout.rdbuf(File);
	std::cout << "-------------\n";
	for(int s=0; s<Size; s++)
	{
		char Chain[MYMAXSTR];
		Massiv[s].getChainStr(Chain);
		std::cout << Chain << std::endl;
	}
	File = std::cout.rdbuf();

}

std::vector<std::string> Classificator::RulesToStrArr(Rule* Massiv, long Size)
{
	std::vector<std::string> rArr(Size);
	for(int s=0; s<Size; s++)
	{
		char Chain[MYMAXSTR];
		Massiv[s].getChainStr(Chain);
		rArr[s] = Chain;
	}
	return rArr;
}

std::vector<std::string> Classificator::RulesToStrArr(std::vector<RuleLink*>& Massiv, long Size)
{
	std::vector<std::string> rArr(Size);
	for(int s=0; s<Size; s++)
	{
		char Chain[MYMAXSTR];
		Massiv[s]->getChainStr(Chain);
		rArr[s] = Chain;
	}
	return rArr;
}
//---------------------------------------------------------------------------
//
//Изменение критерия гамма после выбора
//определенного множества закономерностей
//
double Classificator::PartialSum()
{
	double dTotalSum = 0, MaxVal;
	SToken CurrToken;
	bool isBelong, isPositive;
	for (long RulesCounter = 0 ; RulesCounter < this->FilterSize2; RulesCounter++)
	{
			CurrToken.nPos = p_SubsetRules[RulesCounter]->getTTPos();
			CurrToken.nValue = p_SubsetRules[RulesCounter]->getTTValue();
			CurrToken.Sign = p_SubsetRules[RulesCounter]->getTTSign();

			isBelong = CurrIdelObject->isBelong(&CurrToken);
			isPositive = CurrToken.Sign == 1;
			MaxVal = p_SubsetRules[RulesCounter]->getCP();

			if ( isPositive && isBelong )
			{
					dTotalSum = dTotalSum + ProbFunc(MaxVal); // если законмерность подверждается
			}
			else if ( !isPositive && !isBelong  )
			{
					dTotalSum = dTotalSum + ProbFunc(fabs(MaxVal));  // если законмерность подверждается
			}
			else if ( isPositive && !isBelong )
			{
					dTotalSum = dTotalSum - ProbFunc(MaxVal);   // если законмерность опровергается
			}
			else if ( !isPositive && isBelong )
			{
					dTotalSum = dTotalSum - ProbFunc(fabs(MaxVal));   // если законмерность опровергается
			}
	}
	return dTotalSum;
}

double Classificator::FeatureSum()
{
	//double dTTZDevVal, dTotalSum=0;
	//for (long RulesCounter = 0 ; RulesCounter < this->FilterSize2; RulesCounter++)
	//{
	//	dTTZDevVal = p_SubsetRules[RulesCounter]->getCP();
	//	if ( dTTZDevVal > 0. )
	//	{
	//			dTotalSum = dTotalSum + ProbFunc(dTTZDevVal);
	//	}
	//	else //if (!Alone)
	//	{
	//			dTotalSum = dTotalSum - ProbFunc(fabs(dTTZDevVal));
	//	}
	//}
	//return dTotalSum;
		double dTotalSum = 0, MaxVal;
	SToken CurrToken;
	bool isBelong, isPositive;
	for (long RulesCounter = 0 ; RulesCounter < this->FilterSize2; RulesCounter++)
	{
			CurrToken.nPos = p_SubsetRules[RulesCounter]->getTTPos();
			CurrToken.nValue = p_SubsetRules[RulesCounter]->getTTValue();

			isBelong = !CurrIdelObject->isBelong(&CurrToken);
			MaxVal = p_SubsetRules[RulesCounter]->getCP();
			isPositive = p_SubsetRules[RulesCounter]->getTTSign() == 1;

			if ( isPositive && isBelong )
			{
					dTotalSum = dTotalSum + ProbFunc(MaxVal); // если законмерность подверждается
			}
			else if ( !isPositive && !isBelong  )
			{
					dTotalSum = dTotalSum + ProbFunc(fabs(MaxVal));  // если законмерность подверждается
			}
			else if ( isPositive && !isBelong )
			{
					dTotalSum = dTotalSum - ProbFunc(MaxVal);   // если законмерность опровергается
			}
			else if ( !isPositive && isBelong )
			{
					dTotalSum = dTotalSum - ProbFunc(fabs(MaxVal));   // если законмерность опровергается
			}
	}
	return dTotalSum;
}

// Отбор закономерностей содержащих в посылке Stok
// только точный вариант
// применимых к начальному набору
// т.к. вставка, то применимость надо пересчитывать
long Classificator::Filtr_Ins2(SToken* Stok)
{
	FilterSize2 = 0;
	SToken SendToken;   //предикат
	long nRuleLength;
	bool isApplicable;
	bool isNew;
	
	//проверим, что вставленный является единственным и узнаем
	//в объекте не было значений в текущей позиции
	bool EmptyColumn = IsAlone(Stok);
	ruleID itRulesStorage = pRulesStorage->begin();
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < lRuleStoreSize; lRulesStoreCounter++)
	{
	   nRuleLength = (*itRulesStorage).getRuleLength();
	   isNew = false;
	   isApplicable = true;
	   long l = 0;
	   RuleLink* pRule = &(*itRulesStorage);
	   while ( isApplicable && (l < nRuleLength) )
	   {
			   SendToken.nPos = (*pRule)[l].Shift;
				SendToken.nValue = (*pRule)[l].Value;
				SendToken.Sign = (*pRule)[l].Sign;//отрицание предиката, признак не отрицают
			   l++;

			   //Ri = x
				if ( SendToken.Sign > 0 )
				{
					if ((SendToken.nPos == Stok->nPos ) && (SendToken.nValue == Stok->nValue))
					{
							isNew = true;
					}
					if(!CurrIdelObject->isBelong(&SendToken))
					{
							isApplicable = false;
					}
				}
				//Ri = not x'
			   else if ( SendToken.Sign < 0 )
			   {
						if  (CurrIdelObject->isBelong(&SendToken))
						{
								isApplicable = false;
								break;
						}
						
						if ( EmptyColumn )
						{
							isApplicable = true;
							if ((SendToken.nPos == Stok->nPos ) && (SendToken.nValue != Stok->nValue))
							{
								isNew = true;
							}
						}
						else
							isApplicable = false;

			   }
	   }

	   if ( (isNew) && (isApplicable))
	   {
			if((unsigned int)FilterSize2 < p_SubsetRules.size())
				p_SubsetRules[FilterSize2] = pRule;
			else
				p_SubsetRules.push_back( pRule );
			
			FilterSize2++;
		}
	   itRulesStorage++;
	}

	//берем только СВЗ
	//FilterSize2 = DelSubRules(p_SubsetRules, FilterSize2);

	return FilterSize2;
}
// минусовать по законам, в которых
// not x  и  x'
long Classificator::Filtr_Ins3(SToken* Stok)
{
	FilterSize2 = 0;
	SToken SendToken;
	long nRuleLength;
	bool isOld;  //предикат уже был в законе, для нового набора уже не актуально

	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < FilterSize; lRulesStoreCounter++)
	{
		   nRuleLength = p_AplicReg[lRulesStoreCounter]->getRuleLength();
		   isOld = false;
		   for (long l=0; l < nRuleLength; l++)
		   {
			   SendToken.nPos = (*p_AplicReg[lRulesStoreCounter])[l].Shift;
			   SendToken.nValue = (*p_AplicReg[lRulesStoreCounter])[l].Value;
			   SendToken.Sign = (*p_AplicReg[lRulesStoreCounter])[l].Sign;

				// not x
				if ( (SendToken.Sign < 0 ) && (SendToken.nPos == Stok->nPos ) && (SendToken.nValue == Stok->nValue) )
				{
						isOld = true;
				}
				// x'
				else if ( (SendToken.Sign > 0 ) && (SendToken.nPos == Stok->nPos ) && (SendToken.nValue != Stok->nValue) /*&& IsAlone(Stok)*/ )
				{
						isOld = true;
				}
		   }
		   if ( isOld )
		   {
				//p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				if((unsigned int)FilterSize2 < p_SubsetRules.size())
					p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				else
					p_SubsetRules.push_back( p_AplicReg[lRulesStoreCounter] );

				FilterSize2++;
		   }
	}
	return FilterSize2;
}
//---------------------------------------------------------------------------
// Отбор закономерностей содержащих в заключении НЕ Stok
// применимых к начальному набору
//-- нужен пересчет применимых
long Classificator::Filtr_Del4(SToken* Stok)
{
	FilterSize2 = 0;
	SToken SendToken;
	long nRuleLength;
	bool isApplicable;
	bool isNew;
	ruleID itRulesStorage = pRulesStorage->begin();
	
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < lRuleStoreSize; lRulesStoreCounter++)
	{
			   nRuleLength = (*itRulesStorage).getRuleLength();
			   isNew = false;
			   isApplicable = true;
			   long l = 0;
			   RuleLink* pRule = &(*itRulesStorage);
			   while ( isApplicable && (l < nRuleLength) )
			   {
					SendToken.nPos = (*pRule)[l].Shift;
					SendToken.nValue = (*pRule)[l].Value;
					SendToken.Sign = (*pRule)[l].Sign;
					l++;
					
				   if  (SendToken.Sign > 0 ) 
				   {
					   if (!CurrIdelObject->isBelong(&SendToken)) 
							isApplicable = false;
						// x'
					   if( (SendToken.nPos == Stok->nPos ) && (SendToken.nValue != Stok->nValue) /*&& IsAlone(Stok)*/ )
							isNew = true;
					
				   }
				   else if ( SendToken.Sign < 0 )
				   {
					   // not x
					   if( (SendToken.nPos == Stok->nPos ) && (SendToken.nValue == Stok->nValue) )
						   isNew = true;
					
					   if  (CurrIdelObject->isBelong(&SendToken))
							{
									isApplicable = false;
									break;
							}
							isApplicable = false;
							if ( !IsAlone(&SendToken)) isApplicable = true;

				   }
			   }

			   if ( (isNew) && (isApplicable))
			   {
					if((unsigned int)FilterSize2 < p_SubsetRules.size())
						p_SubsetRules[FilterSize2] = pRule;
					else
						p_SubsetRules.push_back( pRule );

					FilterSize2++;
			   }
			   itRulesStorage++;
		}

	//берем только СВЗ
	//FilterSize2 = DelSubRules(p_SubsetRules, FilterSize2);
	
	return FilterSize2;
}
//---------------------------------------------------------------------------
// Отбор закономерностей содержащих в посылке Stok
// применимых к начальному набору
long Classificator::Filtr_Del1(SToken* Stok)
{
	FilterSize2 = 0;
	SToken SendToken;
	long nRuleLength;
	bool isOld;    // признак, который удаляется
	
	
	// p_AplicReg - уже содержит СВЗ
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < FilterSize; lRulesStoreCounter++)
	{
		   nRuleLength = p_AplicReg[lRulesStoreCounter]->getRuleLength();
		   isOld = false;
		   for (long l=0; l < nRuleLength; l++)
		   {
			   SendToken.nPos = (*p_AplicReg[lRulesStoreCounter])[l].Shift;
			   SendToken.nValue = (*p_AplicReg[lRulesStoreCounter])[l].Value;
			   SendToken.Sign = (*p_AplicReg[lRulesStoreCounter])[l].Sign;

				if ( (SendToken.Sign > 0 ) && (SendToken.nPos == Stok->nPos ) && (SendToken.nValue == Stok->nValue) )
				{
						isOld = true;
				}
				//проверим, удаляем ли мы последнее значение (stok) для признака
				//если удалили, то любой другой на его месте будет одинок
				if ( IsAlone(&SendToken) && (SendToken.Sign < 0 ) && (SendToken.nPos == Stok->nPos ) && (SendToken.nValue != Stok->nValue) )
				{
						isOld = true;
				}

		   }
		   if ( isOld )
		   {
				//p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				if((unsigned int)FilterSize2 < p_SubsetRules.size())
					p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				else
					p_SubsetRules.push_back( p_AplicReg[lRulesStoreCounter] );

				FilterSize2++;
		   }
	}
	return FilterSize2;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Отбор закономерностей предсказывающих Stok
// и применимых к начальному набору
// (stok) в заключении
long Classificator::Filtr_Del2(SToken* Stok)
{
	FilterSize2 = 0;
	int CheckVal, CheckPos;

	// p_AplicReg - уже содержит СВЗ
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < FilterSize; lRulesStoreCounter++)
	{
			CheckPos = p_AplicReg[lRulesStoreCounter]->getTTPos();
			CheckVal = p_AplicReg[lRulesStoreCounter]->getTTValue();
			if( (CheckPos == Stok->nPos ) && (CheckVal == Stok->nValue) )
			{
				if((unsigned int)FilterSize2 < p_SubsetRules.size())
					p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				else
					p_SubsetRules.push_back( p_AplicReg[lRulesStoreCounter] );

				FilterSize2++;
			}
	}
	return FilterSize2;
}

//---------------------------------------------------------------------------
// Отбор закономерностей предсказывающих Stok
// и применимых к начальному набору
// (stok) в заключении
long Classificator::Filtr_Premis(SToken* Stok)
{
	FilterSize2 = 0;
	int CheckVal, CheckPos;

	// p_AplicReg - уже содержит СВЗ
	for ( long lRulesStoreCounter = 0; lRulesStoreCounter < FilterSize; lRulesStoreCounter++)
	{
			CheckPos = p_AplicReg[lRulesStoreCounter]->getTTPos();
			CheckVal = p_AplicReg[lRulesStoreCounter]->getTTValue();
			if( (CheckPos == Stok->nPos ) && (CheckVal == Stok->nValue) )
			{
				if((unsigned int)FilterSize2 < p_SubsetRules.size())
					p_SubsetRules[FilterSize2] = p_AplicReg[lRulesStoreCounter];
				else
					p_SubsetRules.push_back( p_AplicReg[lRulesStoreCounter] );

				FilterSize2++;
			}
	}
	return FilterSize2;
}


 void Classificator::DrawRules()
 {

//	 AboutFrm->
 }
 
 double Classificator::ProbFunc(double x)
{
	return (-log(1-x));
}
//---------------------------------------------------------------------------
//Считает изменение критерия при добавлении заданного элемента
double Classificator::IncRulesTotalSum(SToken* TToken )
{
	double S1=0, S2=0, S3=0;
	//пункт 1
	Filtr_Premis(TToken);

//		//DEBUG_BEGIN
//FILE *seq = fopen("w-in.txt","w");
//fprintf(seq,"-------IncRulesTotalSum------\n");
//fprintf(seq,"------Filtr_Ins2-------\n");
//char Chain[MYMAXSTR];
//	for(int s=0; s<FilterSize2; s++)
//	{
//		
//		p_SubsetRules[s]->getChainStr(Chain);
//		fprintf(seq,"%s\n",Chain);
//	}
////DEBUG_END
	S1 = -2.*FeatureSum();
	//пункт 2
	Filtr_Ins2(TToken);
//		//DEBUG_BEGIN
	//for(int s=0; s<FilterSize2; s++)
	//{
	//	
	//	p_SubsetRules[s]->getChainStr(Chain);
	//	fprintf(seq,"%s\n",Chain);
	//}

////DEBUG_END

	S2 = PartialSum();
	//пункт 3
	Filtr_Ins3(TToken);
//		//DEBUG_BEGIN
//fprintf(seq,"------Filtr_Ins3-------\n");
//	for(int s=0; s<FilterSize2; s++)
//	{
//		
//		p_SubsetRules[s]->getChainStr(Chain);
//		fprintf(seq,"%s\n", Chain);
//	}
//		
//fclose(seq);
////DEBUG_END

	S3 = -1.*PartialSum();

	return (S1+S2+S3);
}
//---------------------------------------------------------------------------
//Считает изменение критерия при удалении заданного элемента
double Classificator::ExcRulesTotalSum(SToken* TToken)
{
	double S1=0, S2=0, S4=0;
	//пункт 1
	Filtr_Premis(TToken);
//		//DEBUG_BEGIN
//FILE *seq = fopen("w-ex.txt","w");
//fprintf(seq,"------ExcRulesTotalSum-------\n");
//fprintf(seq,"------Filtr_Premis-------\n");
//char Chain[MYMAXSTR];
//		char* Context = new char[strlen(Chain)];
//	for(int s=0; s<FilterSize2; s++)
//	{
//		
//		p_SubsetRules[s]->ChainStr(Chain, Context, true);
//		fprintf(seq,"%s\n", Chain);
//	}
//
////DEBUG_END

	S1 = -2.*FeatureSum();
	// == 1 ==
	Filtr_Del1(TToken);
//		//DEBUG_BEGIN
//fprintf(seq,"------Filtr_Del1-------\n");
//	for(int s=0; s<FilterSize2; s++)
//	{
//		
//		p_SubsetRules[s]->ChainStr(Chain, Context, true);
//		fprintf(seq,"%s\n", Chain);
//	}
//
////DEBUG_END

	S2 = -1.*PartialSum();
	// == 4 ==
	Filtr_Del4(TToken);
//		//DEBUG_BEGIN
//fprintf(seq,"------Filtr_Del4-------\n");
//	for(int s=0; s<FilterSize2; s++)
//	{
//		
//		p_SubsetRules[s]->ChainStr(Chain, Context, true);
//		fprintf(seq,"%s\n", Chain);
//	}
//			delete[] Context;
//
//fclose(seq);
////DEBUG_END

	S4 = PartialSum();

	return (S1+S2+S4);
}
//---------------------------------------------------------------------------
//Проверяет один ли признак у текущего идального объекта на данной позиции или нет
bool Classificator::IsAlone(SToken* TToken)
{
		SToken SendToken;

		SendToken.nPos = TToken->nPos;
		for( SendToken.nValue = 0; SendToken.nValue < nCodesCount; SendToken.nValue++)
		{
				if ( (CurrIdelObject->isBelong(&SendToken)) && (SendToken.nValue != TToken->nValue))
				{
						return false;
                }
        }
        return true;

}
//---------------------------------------------------------------------------
//Проверяет один ли признак у идального объекта из хранилища на данной позиции или нет
bool Classificator::IsAloneIdOb(SToken* TToken, int nClNumber)
{
		SToken SendToken;

        SendToken.nPos = TToken->nPos;
        for( SendToken.nValue = 0; SendToken.nValue < nCodesCount; SendToken.nValue++)
        {
                if ( (ObjsStorage[nClNumber]->isBelong(&SendToken)) && (SendToken.nValue != TToken->nValue))
                {
                        return false;
                }
        }
        return true;
}

/*
*	Шаг III
*/

//---------------------------------------------------------------------------
//Выдает собственно полное изменение критерия при добавлении объекта
double Classificator::IncludeTest(SToken* IncludeToken)
{
	double W = 0.;
	CurrIdelObject->IncludeT(IncludeToken);
	W = /*- 2.*VMatrix[IncludeToken->nPos][IncludeToken->nValue]
		+ */IncRulesTotalSum(IncludeToken);
	CurrIdelObject->ExcludeT(IncludeToken);
	return W;
}
//---------------------------------------------------------------------------
//Выдает собственно полное изменение критерия при удалении объекта
double Classificator::ExcludeTest(SToken* ExcludeToken)
{
	double W = 0.;
	CurrIdelObject->ExcludeT(ExcludeToken);
	W = /*- 2.*VMatrix[ExcludeToken->nPos][ExcludeToken->nValue]
		+ */ExcRulesTotalSum(ExcludeToken);
	CurrIdelObject->IncludeT(ExcludeToken);
	return W;
}
//---------------------------------------------------------------------------
void Classificator::Insider(CIdelObject* ideal, SToken& ct, bool &in, bool &ex)
{
	double dMaxGamma_plus=0, dMaxGamma_minus=0;
	double Gamma_Straight;

	CountMatrix(ideal);//FillVMatrix();
	Gamma_Straight = Count_GammaCryterion();

//	AboutFrm->InitO->Caption = FloatToStrF(Count_GammaCryterion(), ffGeneral, 8, 4);
	in = false;
	ex = false;

	SearchForAction(ideal);
    //just find extremum without actions
	rGI max_ex = I_negative.rbegin();
	rGI max_in = I_positive.rbegin();
	if( !I_negative.empty() )
		dMaxGamma_minus = (*max_ex).first;
	if( !I_positive.empty() )
		dMaxGamma_plus = (*max_in).first;

	//выбираем: вставка или удаление
	if ( dMaxGamma_plus <  dMaxGamma_minus)
	{
		ex = true;
		ct = (*max_ex).second;
//		AboutFrm->CurrO->Caption = FloatToStrF(dMaxGamma_minus, ffGeneral, 8, 4);
	}
	else if ( dMaxGamma_plus > 0. )
	{
		in = true;
		ct = (*max_in).second;
//		AboutFrm->CurrO->Caption = FloatToStrF(dMaxGamma_plus, ffGeneral, 8, 4);
	}
	else
	{
;//		AboutFrm->CurrO->Caption = "0";
	}


}
//---------------------------------------------------------------------------
std::vector<std::string> Classificator::IncludeTest(CIdelObject* ideal, SToken s1)
{
	//CountMatrix(ideal);
	std::vector<std::string> p2,p3;
	s1.Sign = 1;
 //	ObviousPred(s1);
	if(!ideal->isBelong(&s1))
	{
		InitObj = new char[MYMAXSTR];
		CurrObj = new char[MYMAXSTR];
		std::ostringstream strs;
		ideal->getObjAsStr(InitObj);
		double m_Gamma = Count_GammaCryterion();

		ideal->IncludeT(&s1);
		double S1=0, S2=0, S3=0;
		Filtr_Premis(&s1);
		S1 = -2.*FeatureSum();
		p2 = RulesToStrArr(p_SubsetRules, FilterSize2);
		strs << S1;
		p2.push_back(strs.str());
		strs.str("");		
		
		//пункт 2
		Filtr_Ins2(&s1);
		S2 = PartialSum();
		p3 = RulesToStrArr(p_SubsetRules, FilterSize2);
		p2.push_back(" ");

		p2.reserve(p2.size()+p3.size());
		p2.insert(p2.end(), p3.begin(), p3.end());
		strs << S2;
		p2.push_back(strs.str());
		strs.str("");
		
		//пункт 3
		Filtr_Ins3(&s1);
		S3 = -1.*PartialSum();
		p3 = RulesToStrArr(p_SubsetRules, FilterSize2);
		p2.push_back(" ");
		
		p2.reserve(p2.size()+p3.size());
		p2.insert(p2.end(), p3.begin(), p3.end());
		strs << S3;
		p2.push_back(strs.str());
		strs.str("");
		
		FillVMatrix();
		double m_Increase = Count_GammaCryterion() - m_Gamma;
		strs << (S1+S2+S3) << " -w | dv:" << m_Increase;
		p2.push_back(strs.str());
		
		ideal->ExcludeT(&s1);
		FillVMatrix();
		ideal->getObjAsStr(CurrObj);
		//if( strcmp(CurrObj, InitObj)==0 )
		//{
		//	AboutFrm->useRules1->Cells[0][0] = "dW: " + FloatToStrF(double(S2+S3+S1) - m_Increase, ffGeneral, 8, 4)
		//	+ " dV " + FloatToStrF(atof(AboutFrm->InitO->Caption.t_str()) - m_Gamma, ffGeneral, 8, 4);

		//}
		//else
		//	AboutFrm->useRules1->Cells[0][0] = "Error!";
		delete[] InitObj;
		delete[] CurrObj;

	}
	return p2;
}
//---------------------------------------------------------------------------
std::vector<std::string> Classificator::ExcludeTest(CIdelObject* ideal, SToken s2)
{
	//CountMatrix(ideal);
	std::vector<std::string> p1,p2;
	s2.Sign = -1;
//	ObviousPred(s2);
	if(ideal->isBelong(&s2))
	{
	
		InitObj = new char[MYMAXSTR];
		CurrObj = new char[MYMAXSTR];
		std::ostringstream strs;
		ideal->getObjAsStr(InitObj);
		double m_Gamma = Count_GammaCryterion();
		ideal->ExcludeT(&s2);

		double S1=0, S2=0, S4=0;
		Filtr_Premis(&s2);
		S1 = -2.*FeatureSum();
		p1 = RulesToStrArr(p_SubsetRules, FilterSize2);
		strs << S1;
		p1.push_back(strs.str());
		strs.str("");
		
		// == 1 ==
		Filtr_Del1(&s2);
		S2 = -1.*PartialSum();
		p1.push_back(" ");
		p2 = RulesToStrArr(p_SubsetRules, FilterSize2);
		p1.reserve(p1.size()+p2.size());
		p1.insert(p1.end(), p2.begin(), p2.end());
		strs << S2;
		p1.push_back(strs.str());
		strs.str("");
		
		// == 4 ==
		Filtr_Del4(&s2);
		S4 = PartialSum();
		p1.push_back(" ");
		p2 = RulesToStrArr(p_SubsetRules, FilterSize2);
		p1.reserve(p1.size()+p2.size());
		p1.insert(p1.end(), p2.begin(), p2.end());

		strs << S4;
		p1.push_back(strs.str());
		strs.str("");
		
		FillVMatrix();
		double m_Increase = Count_GammaCryterion() - m_Gamma;
		strs << (S1+S2+S4) << " -w | dv:" << m_Increase;
		p1.push_back(strs.str());
		strs.str("");
		
		ideal->IncludeT(&s2);
		FillVMatrix();
        		ideal->getObjAsStr(CurrObj);
		//if( strcmp(CurrObj, InitObj)==0 )
		//{
		//	AboutFrm->useRules1->Cells[0][0] = "dW: " + FloatToStrF(double(S1+S2+S4) - m_Increase, ffGeneral, 8, 4)
		//	+ " dV " + FloatToStrF(atof(AboutFrm->InitO->Caption.t_str()) - m_Gamma, ffGeneral, 8, 4);

		//}
		//else
		//	AboutFrm->useRules1->Cells[0][0] = "Error!";
		delete[] InitObj;
		delete[] CurrObj;
	}
	return p1;
}

std::vector<std::string> Classificator::GetWRules(CIdelObject* ideal, SToken s1)
{
	if(!ideal->isBelong(&s1))
		return IncludeTest(ideal, s1);
	else 
		return ExcludeTest(ideal, s1);
	
}

std::vector<std::string> Classificator::GetAplicableRules()
{
	return RulesToStrArr(p_AplicReg, FilterSize);
}
