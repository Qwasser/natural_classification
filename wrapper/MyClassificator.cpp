#include <algorithm>
#include "MyClassificator.h"
#include "manager/Carcass.h"

int MyClassificator::id = 0;

MyClassificator::MyClassificator()
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

MyClassificator::MyClassificator(int IType)
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


MyClassificator::~MyClassificator()
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

bool MyClassificator::Create(SEQStorage* pSeqStorage,
                           RulesStorage* pRulesStorage,
                           CIdelObject** ObjsStorage)
{
    std::cout << "lla" << std::endl;
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

	this->pSeqStorage = pSeqStorage;
	lSeqStoreSize = pSeqStorage->getLength();
	this->lSeqLength = pSeqStorage->getWidth();

	this->pRulesStorage = pRulesStorage;
	lRuleStoreSize = pRulesStorage->getSize();
	this->nCodesCount = pSeqStorage->getCodesCount();

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
    id = 0;
	chObject = new char[MYMAXSTR];
	CurrIdelObject = NULL;

	this->ObjsStorage = ObjsStorage;

	return 0;
}


void MyClassificator::GenClasses()
{
    if(m_source == __IDEALSOURCE_ORIGINOBJS_) {
        WorkUpData();
    }
}
            
void MyClassificator::WorkUpData()
{
    int nClNumber;
    int SeqCount = pSeqStorage->getLength();

	for (int SeqCounter = 0; SeqCounter < SeqCount; SeqCounter++)
	{
		SEQSElem* CurrSeq = new SEQSElem(pSeqStorage->getWidth());
		pSeqStorage->CreateElem(SeqCounter, CurrSeq);
		
		nClNumber = NextIter(CurrSeq);
		pSeqStorage->setClass(SeqCounter, nClNumber);
		Carcass::lObjsStorageSize = lObjsStorageCounter;

		delete CurrSeq;
	}
}

//Процедура классификации, то есть таже идеализация, но
//найденный идеальный объект не сохраняется в множестве если он новый
long MyClassificator::OneIter(SEQSElem* NextSeq, long IterCounter)
{
    m_NextSeq = NextSeq;

    CurrIdelObject = new CIdelObject();
    CurrIdelObject->Create(lSeqLength, nCodesCount, m_NextSeq);

    switch ( IdealizType )
    {
        case __IDEALIZTYPE_OFFICIAL_:
        {
            MaxDelInsBuild();
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

void MyClassificator::getObjAsStr(SEQSElem* Pattern, char* chObject)
{
	CIdelObject TempObject;
	TempObject.Create(lSeqLength, nCodesCount, Pattern);
	TempObject.getObjAsStr(chObject);
}

void MyClassificator::CountMatrix(CIdelObject* Cl)
{
	CurrIdelObject = Cl;
	this->FillVMatrix();
}

double MyClassificator::Get_VMatrixVal(long pos, int cod)
{
	return VMatrix[pos][cod];
}

double MyClassificator::Count_GammaCryterion( )
{
    double fGamma = 0;

	for ( unsigned long pos = 0; pos < lSeqLength; pos++)
		for ( int cod = 0; cod < nCodesCount; cod++)
            fGamma += VMatrix[pos][cod];

	return fGamma;
}

// Процедура поиска идеального объекта
long MyClassificator::NextIter(SEQSElem* NextSeq)
{
	m_NextSeq = NextSeq;
	CurrIdelObject = new CIdelObject();
	CurrIdelObject->Create(m_NextSeq, pSeqStorage->getCodesCount());

    lIterCounter++;

    MaxDelInsBuild();

    CurrIdelObject->getObjAsStr(chObject);
    CurrIdelObject->setGamma(Count_GammaCryterion());
    ObjsStorage[lObjsStorageCounter] = CurrIdelObject;
    CurrIdelObject = NULL;

    long lClassNumber = lObjsStorageCounter;
    lObjsStorageCounter++;
	return lClassNumber;
}

bool MyClassificator::ObviousPred(SToken vij)
{
    if(vij.nPos>=lSeqLength || vij.nValue>=nCodesCount)
        return false;
    else
    {
        RuleSection rs;
        rs.Shift = vij.nPos;
        rs.Value = vij.nValue;
        rs.Sign = vij.Sign;

        if( std::find(FiltrTargets.begin(), FiltrTargets.end(), rs) == FiltrTargets.end() )
            return false;
        else
            return true;
    }

    return false;
}

void MyClassificator::MaxDelInsBuild()
{
	double Gamma_Straight;
	bool FinishFlag = true;

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
}

void MyClassificator::SearchForAction(CIdelObject* i_Object)
{
	SToken* CurrToken = new SToken;
	double dCurrGammaIncrease;
	I_negative.clear();
	I_positive.clear();

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
				}
			}
		}
	}

	delete CurrToken;
}

bool MyClassificator::ActionExtremum(CIdelObject* i_Object)
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
		FinishFlag = true; //значит еще одна итерация
		//m_Conflict = (*max_ex).second;

		for(GI j = i1.first; j != i1.second; j++)
		{
			i_Object->ExcludeT(&(*j).second);
		}

		Filtrovka();//W - только обновления посмотреть		
		CurrIdelObject->getObjAsStr(chObject);

		
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
		}

		for(GI j = i2.first; j != i2.second; j++)
		{
			i_Object->IncludeT(&(*j).second);
		}
		//FillVMatrix();//V - целиком считать матрицу
		Filtrovka();//W - только обновления посмотреть		

		// _DEBUG__B
		CurrIdelObject->getObjAsStr(chObject);
		
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
		}
		//FillVMatrix();//V - целиком считать матрицу
		Filtrovka();//W - только обновления посмотреть		

		// _DEBUG__B
		CurrIdelObject->getObjAsStr(chObject);
		
		//========== идеал получен. записываем максимум =
		i_Object->setGamma(i_Object->getGamma() + dMaxGamma_plus);
	}

	return FinishFlag;
}

// удалени признаков с нулевым приростом Г критерия
void MyClassificator::DelZeroFeature(CIdelObject* i_Object)
{
	SToken* CurrToken = new SToken;
	double dCurrGammaIncrease;

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
				}
			}
		}
	}

	delete CurrToken;
}


// Учтем уровень предсказания предикатов по вероятности у отобранных правил 
void MyClassificator::DiscountRule(RuleLink* pRule, RuleSection &prev)
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
int MyClassificator::FillVMatrix()
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

	return(-1);
}
//---------------------------------------------------------------------------
// Поиск закономерностей применимых к начальному набору
//заполняет массив p_AplicReg
long MyClassificator::Filtrovka()
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
	return FilterSize;
}

std::vector<std::string> MyClassificator::RulesToStrArr(Rule* Massiv, long Size)
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

std::vector<std::string> MyClassificator::RulesToStrArr(std::vector<RuleLink*>& Massiv, long Size)
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
double MyClassificator::PartialSum()
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

double MyClassificator::FeatureSum()
{
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
long MyClassificator::Filtr_Ins2(SToken* Stok)
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
long MyClassificator::Filtr_Ins3(SToken* Stok)
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
long MyClassificator::Filtr_Del4(SToken* Stok)
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
long MyClassificator::Filtr_Del1(SToken* Stok)
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
long MyClassificator::Filtr_Del2(SToken* Stok)
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
long MyClassificator::Filtr_Premis(SToken* Stok)
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
 
 double MyClassificator::ProbFunc(double x)
{
	return (-log(1-x));
}
//---------------------------------------------------------------------------
//Считает изменение критерия при добавлении заданного элемента
double MyClassificator::IncRulesTotalSum(SToken* TToken)
{
	double S1=0, S2=0, S3=0;

	//пункт 1
	Filtr_Premis(TToken);
	S1 = -2.*FeatureSum();

	//пункт 2
	Filtr_Ins2(TToken);
	S2 = PartialSum();

	//пункт 3
	Filtr_Ins3(TToken);
	S3 = -1.*PartialSum();
	return (S1+S2+S3);
}

//---------------------------------------------------------------------------
//Считает изменение критерия при удалении заданного элемента
double MyClassificator::ExcRulesTotalSum(SToken* TToken)
{
	double S1=0, S2=0, S4=0;

	//пункт 1
	Filtr_Premis(TToken);
	S1 = -2.*FeatureSum();

	Filtr_Del1(TToken);
	S2 = -1.*PartialSum();
	Filtr_Del4(TToken);
	S4 = PartialSum();
	return (S1+S2+S4);
}
//---------------------------------------------------------------------------
//Проверяет один ли признак у текущего идального объекта на данной позиции или нет
bool MyClassificator::IsAlone(SToken* TToken)
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
bool MyClassificator::IsAloneIdOb(SToken* TToken, int nClNumber)
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
double MyClassificator::IncludeTest(SToken* IncludeToken)
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
double MyClassificator::ExcludeTest(SToken* ExcludeToken)
{
	double W = 0.;
	CurrIdelObject->ExcludeT(ExcludeToken);
	W = /*- 2.*VMatrix[ExcludeToken->nPos][ExcludeToken->nValue]
		+ */ExcRulesTotalSum(ExcludeToken);
	CurrIdelObject->IncludeT(ExcludeToken);
	return W;
}

//---------------------------------------------------------------------------
std::vector<std::string> MyClassificator::IncludeTest(CIdelObject* ideal, SToken s1)
{
	//CountMatrix(ideal);
	std::vector<std::string> p2,p3;
	s1.Sign = 1;
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

		delete[] InitObj;
		delete[] CurrObj;

	}
	return p2;
}
