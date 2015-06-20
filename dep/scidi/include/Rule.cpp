//---------------------------------------------------------------------------
#include <algorithm>
#include <utility>

#include "Rule.h"
//---------------------------------------------------------------------------

//Правило неизвестной длины для наращивания посылки
Rule::Rule()
{
	Length = 0;
	//CPLevel = .0;
	prob_denominator = 0;
	prob_numerator = 0;
	Yvalue = 0;
	nTTPos = 0;
	TTSign = 1;
	chain = new PredicateArray[MAXDEFLEN];
	for(unsigned long r=0; r< MAXDEFLEN; r++)
	{
		chain[r] = 0;
	}
}

//Считывать из файла законы с уже известной длиной: IF(Len) S=V THEN S'=V'\n
Rule::Rule(unsigned long Length)
{
	this->Length = Length;
	//CPLevel = .0;
	prob_denominator = 0;
	prob_numerator = 0;
	Yvalue = 0;
	nTTPos = 0;
	TTSign = 1;
	chain = new PredicateArray[Length];
	for(unsigned long r=0; r< Length; r++)
	{
		chain[r] = 0;
	}
}
//---------------------------------------------------------------------------
Rule::~Rule()
{
	delete[] chain;
}
//---------------------------------------------------------------------------

//создает копию закономерности с наращиванием
Rule::Rule(Rule* ptr_RepRule, Predicate* TokenStruct)
{
	//заключение
	nTTPos = ptr_RepRule->getTTPos();
	Yvalue = ptr_RepRule->getTTValue();
	TTSign = ptr_RepRule->getTTSign();
	
	//посылка
	Length = ptr_RepRule->getRuleLength() + 1;
	chain = new PredicateArray[Length];
	// copy existed part
	for (UINT RuleIndex = 0; RuleIndex < Length - 1; RuleIndex++)
		chain[RuleIndex] = ptr_RepRule->chain[RuleIndex];
	// init the last item in chain
	chain[Length-1] = 0;
	
	//собственно наращивание
	AddSection( TokenStruct );//chain[ptr_RepRule->getRuleLength()] = TokenStruct;
	
	//CPLevel = .0;
	prob_denominator = 0;
	prob_numerator = 0;
}
//---------------------------------------------------------------------------


//Инициализирует новую посылку с заданной длинной
int Rule::init(long Length, SEQStorage* SEQStore)
{
	this->Length = Length;
	chain = new PredicateArray[Length];
	for(unsigned long r=0; r< Length; r++)
		chain[r] = 0;
	return(1);
}

void Rule::rulecpy(Rule* ptr_RepRule)
{
	nTTPos = ptr_RepRule->getTTPos();
	Yvalue = ptr_RepRule->getTTValue();
	TTSign = ptr_RepRule->getTTSign();

	for (UINT RuleIndex = 0; RuleIndex < ptr_RepRule->getRuleLength(); RuleIndex++)
		chain[RuleIndex] = ptr_RepRule->chain[RuleIndex];
		
}
//---------------------------------------------------------------------------
//создает копию закономерности
Rule::Rule(Rule* ptr_RepRule)
{
	prob_denominator = 0;
	prob_numerator = 0;
	//CPLevel = .0;
	
	this->init(ptr_RepRule->getRuleLength(), 0);
	this->rulecpy(ptr_RepRule);

}
//---------------------------------------------------------------------------

RuleSection element;

RuleSection& Rule::Chain(UINT position)
{
	element.Shift = 0;
	element.Sign = 0;
	element.Value = 0;
	if (position > getRuleLength())
	{
		return element;
	}
	if ( chain==0 )
	{
		return element;
	}
	return *chain[position];
}

////Наращивает закономерность
//int Rule::AddSection(RuleSection TokenStruct)//int Shift, int Sign, int SendToken)
//{
//	UINT ChainIndex;
//	//Length++;
//	if ( Length > 1 )
//	{
//		bool IsInsirted = false;
//		for ( ChainIndex = 0; ChainIndex < Length - 1; ChainIndex++ )
//		{
//			if ( Chain(ChainIndex).Shift > TokenStruct.Shift )
//			{
//				IsInsirted = true;
//				break;
//			}
//		}
//
//		if ( IsInsirted )
//		{
//			for ( UINT ShiftIndex = Length - 1; ShiftIndex >= ChainIndex + 1; ShiftIndex-- )
//			{
//				Chain(ShiftIndex) = Chain(ShiftIndex-1);
//			}
//		}
//		else
//		{
//			ChainIndex = Length - 1;
//		}
//	}
//	else
//	{
//		ChainIndex = 0;
//	}
//
//	Chain(ChainIndex) = TokenStruct;
//	return (0);
//}

//Наращивает закономерность
void Rule::AddSection(Predicate* TokenStruct)
{
			/*
			chain[r]->Shift = UINT_MAX;
		chain[r]->Sign = 0;
		chain[r]->Value = 0;
		*/
	UINT ChainIndex;
	if ( Length > 1 )
	{
		bool IsInserted = false;
		for ( ChainIndex = 0; ChainIndex < Length - 1; ChainIndex++ )
		{
			if ( chain[ChainIndex] == 0 || Chain(ChainIndex).Shift > TokenStruct->Shift )
			{
				IsInserted = true;
				break;
			}
		}

		if ( IsInserted )
		{
			for ( UINT ShiftIndex = Length - 1; ShiftIndex >= ChainIndex + 1; ShiftIndex-- )
			{
				if ( chain[ChainIndex]!=0 )
					chain[ShiftIndex] = &Chain(ShiftIndex-1);
			}
		}
		else
		{
			ChainIndex = Length - 1;
		}
	}
	else
	{
		ChainIndex = 0;
	}

	chain[ChainIndex] = TokenStruct;
}

//---------------------------------------------------------------------------
//Процедура вычисляет значимость закономерности, т.е. вычисляет значения
//условной вероятности и критерия Фишера

int Rule::Probability(SEQStorage *SeqsStore)
{
	//CPLevel = 0.;
	prob_denominator = 0;
	prob_numerator = 0;

	bool TopIsAplicable, BottomIsAplicable;
	RuleSection SendToken;
	int nTokenValue;
	unsigned int top = 0, bottom = 0;
	unsigned int spaces=0;
	//////
	///	Условная вероятность = N(P0&P1&...Pm)/N(P1&...Pm) для P0&P1&...Pm => P0
	/////
	//////
	///	Вероятность в числителе (Top) и знаменателе (Bottom)
	/////
	for (long FilterCounter = 0; FilterCounter < SeqsStore->getLength(); FilterCounter++)//getLength() - длина массива входных стимулов, т.е. число объектов
	{
		TopIsAplicable = true;
		BottomIsAplicable = true;
		spaces=0;
		//фиксируем объект, смотрим выполнимость посылки
		for ( UINT nChainIndex = 0; nChainIndex < getRuleLength(); nChainIndex++ )
		{
			if( SeqsStore->getElem_c(FilterCounter, Chain(nChainIndex).Shift, nTokenValue) )
			{
				
				SendToken.Shift = Chain(nChainIndex).Shift;
				SendToken.Value = Chain(nChainIndex).Value;
				SendToken.Sign = Chain(nChainIndex).Sign;

				if ( ( SendToken.Sign == 1 && nTokenValue != SendToken.Value )
					 || ( SendToken.Sign == -1 && nTokenValue == SendToken.Value ) )
				{
					 BottomIsAplicable = false;
					 break;
				}
			}
			else
			{
				BottomIsAplicable = false;
				break;
				/*spaces++;
				if(getRuleLength()==spaces)
					BottomIsAplicable = false;*/
			}
		}
		//выполнимость заключения
		if ( BottomIsAplicable )
		{
			if( SeqsStore->getElem_c(FilterCounter, getTTPos(), nTokenValue) )
			{
				bottom++;
				if ( ( getTTSign() > 0 && nTokenValue == getTTValue() )
					 || ( getTTSign() < 0 && nTokenValue != getTTValue() ) )
					 top++;
			}
		}
	}
	//правило нигде не выполняется
	//ничего не предсказывает
	//значит вероятность посылки неопределена
	if ( bottom == 0 || top == 0 )
	{
		return RULE_NO_DATA;
	}

	if (top > 0)
	{
		prob_numerator = top;
		prob_denominator = bottom;
		//dCP = (double)top / ((double)bottom);
		//(double)top/((double)bottom+ 1e-4);
		//((double)top + (double)1)/((double)bottom + (double)2);
		//(double)( (top + 1.)/(bottom + 2.) );
	}
	
	////если правило предсказывает лучше обычной встречаемости
	//if (getTTSign() > 0 /*&& dCP > SeqsStore->getFreq(nTTPos, Yvalue)*/) {
	//	CPLevel = (dCP*10000.)/10001.;
	//}
	//else if (getTTSign() < 0 /*&& dCP > ( 1 - SeqsStore->getFreq(nTTPos, Yvalue) ) */) {
	//	CPLevel = -1 * (dCP*10000.) / 10001.;
	//}


	return RULE_HAS_CP;
}

//////
///	критерии
/////
//check one new predicat
int Rule::Criteria(SEQStorage *SeqsStore, double ConfInterval, double ConfInterval2)
{
	unsigned long m_local[2][2];
	CountDeps(SeqsStore, m_local, std::vector<int>(0));
	//////
	///	Критерий
	/////
	double clyq1=.0, clyq2=.0;
	int iot=0;

	iot = FisherCriterion(m_local, ConfInterval, clyq1);
	if (iot == 3) iot = YuleCriterion(m_local, ConfInterval, ConfInterval2, clyq2);
	//switch(iot)
	//{
	//	case 0:
	//		Summary = RULE_NO_DATA_FISHER;
	//		break;
	//	case 1:
	//		Summary = RULE_NEG_CORRELATION;
	//		break;
	//	case 2:
	//		Summary = RULE_INDEPENDENT_EXPERTION;
	//		break;
	//}
	if ( iot < 3) //Negative or equal to zero correlation
	{
		//FisherValue = 1.;
		return DECLINE_GROW;
	}
	else  //Positive correlation
	{
		//FisherValue = clyq1;
		return ACCEPT_GROW;
	}

	return DECLINE_GROW;
}

#define PROPORTION 3
void Rule::ForceCorrelation(SEQStorage *Data, unsigned long (&m)[2][2], std::vector<int> true_conj)
{
	double pi=1., p0=.0, pi_1=1, n=Data->getLength();
	for(UINT l=0; l<getRuleLength(); l++ )
	{
		pi *= Data->getFreq( Chain(l).Shift, Chain(l).Value );
	}
	p0 = Data->getFreq( getTTPos(), getTTValue() );
	if( !true_conj.empty() )
		for(UINT t=0; t<true_conj.size(); t++ )
			pi_1 *= Data->getFreq( Chain(true_conj[t]).Shift, Chain(true_conj[t]).Value );

	m[1][1] += (unsigned long)ceil( pi*p0*n*PROPORTION );
	m[1][0] += (unsigned long)ceil( pi*(1-p0)*n*PROPORTION );
	m[0][1] += (unsigned long)ceil( p0*(1-pi)*n*PROPORTION );
	m[0][0] += (unsigned long)ceil( pi_1*n*(PROPORTION + pi*p0 - 2*(pi+p0)) );

}

void Rule::GetFisher(SEQStorage* Data, unsigned long (&m)[2][2], std::vector<int> true_conj, double& fisher, double& yule)
{
	unsigned long m_local[2][2];
	double clyq1 = 1.0, clyq2 = 0.0;

	CountDeps(Data, m, true_conj);
	//ForceCorrelation(Data, m);
	m_local[0][0] = m[0][0];
	m_local[0][1] = m[0][1];
	m_local[1][0] = m[1][0];
	m_local[1][1] = m[1][1];

	//if ( getRuleLength() > 1 )
	//	Minimize(Data, fisher, yule);
	//else
	{
		FisherCriterion(m_local, fisher, clyq1);
		//SetLambda( InverseNormalCDF(1.-0.5*fisher) );
		YuleCriterion(m_local, 0.01, yule, clyq2);
	}

	fisher = clyq1;
	yule = clyq2;
	return;
}

//---------------------------------------------------------------------------
//Процедура пытается минимизировать закономерность, т.е. выкинуть
//из посылки один из предикатов без ущерба для значений
//условной вероятности и критерия Фишера
//check other predicats
bool Rule::Minimize(SEQStorage *SeqsStore, double CInt_Value, double CInt2_Value)
{
	if ( getRuleLength() == 1 )
	{
		return false;
	}

	double MaxErr = 0.0;
	double MaxTokenFisher = .0;
	double MinTokenYule = 1.0;
	int DelIndex = -1;
	unsigned long m_local[2][2];

	for ( UINT conjunction = 1; conjunction < getRuleLength(); conjunction++ )
	{
		std::vector<int> mask(conjunction, 0);//k
		mask.resize(getRuleLength(), 1);            //n
		std::vector<int> subrule, true_conj;

		do
		{
			subrule.clear();
			true_conj.clear();

			for (unsigned int i=0; i < mask.size(); i++)
				if (!mask[i])
					true_conj.push_back(i);
				else
					subrule.push_back(i);


			CountDeps(SeqsStore, m_local, true_conj);

			int iot = 0;
			double clyq1 = 0.0, clyq2 = 0.0;
			iot = FisherCriterion(m_local, CInt_Value, clyq1);
			if(iot == 3) iot = YuleCriterion(m_local, CInt_Value, CInt2_Value, clyq2);
			if (iot != 3)
				return(true);
			if(MaxTokenFisher < clyq1)
				MaxTokenFisher = clyq1;
			if(MinTokenYule > clyq2)
				MinTokenYule = clyq2;

		} while (std::next_permutation(mask.begin(), mask.end()));
	}

//	this->FisherValue = MaxTokenFisher;
//	this->YuleValue = MinTokenYule;
	return(false);
}

//---------------------------------------------------------------------------
double Rule::Faktor(long ifak)
{
		if(ifak<=0) return 0.0;
		double ai;
		const double faktor[11] = {
		0.0,0.0,0.692,1.791,3.177,4.787,6.579,8.525,10.604,12.802,15.104 };
		if( ifak <= 10 ) return faktor[ifak];
		ai = double(ifak);
        ai = ai*log( ai ) + 0.5*(1.8374+log(ai)) - ai + 1./(12.*ai) -
		1./(360.*ai*ai);  // double log( double x ); - the natural logarithm
		return ai;
}
//---------------------------------------------------------------------------

//Процедура считает значение точного критерия Фишера (зависимость признаков)
//матрица m заполняется VityaevCount и в VityaevMinimize
// представляет собой стат. данные:             выполн     невыполн
//                                  посылка       a           b
//                                 цел. призн     c           d
// в случае зависимости - возвращает значение критерия Фишера

//ZDev  - доверительный интервал для Фишера
//clyq1 - возвращаемое значение критерия Фишера
int Rule::FisherCriterion(unsigned long (&m)[2][2], double ZDev, double& clyq1)
{
	long l1,l2,l3,l4;
	long m00,m01,m10,m11;
	double constant, variable;
	double lnGiper, Giper;
	long i1,k1;

	l1 = m[0][0]+m[0][1]+m[1][0]+m[1][1];
	l2 = m[1][0]+m[1][1];
	l3 = m[0][1]+m[1][1];
	l4 = m[1][1];

	if ( l1 == 0) return(0); // NO DATA

	//даже если будет показана зависимость, т.е. отсутствие равенства
	//проверяем: может неравенство в другую сторону
	if( (double)(l2*l3)/(double)l1 > l4)
	{
		l2 = l1-l2;
		l4 = l3-l4;
	} //Negative correlation

	constant = Faktor(l2)+Faktor(l1-l2)+Faktor(l3)+Faktor(l1-l3)-Faktor(l1);
	variable = -Faktor(m[0][0])-Faktor(m[0][1])-Faktor(m[1][0])-Faktor(m[1][1]);

	lnGiper = constant + variable;
	clyq1 = 0.0;
	clyq1 = exp(lnGiper);
	
	if (l2 <= l3)	k1 = l2;
	else	k1 = l3;

	m11 = l4;
	m10 = l3-l4;
	m01 = l2-l4;
	m00 = l1-l2-l3+l4;

	
	for(i1 = 0; i1 < k1-l4; i1++)
	{
			m00++;
			m11++;
			m01--;
			m10--;

			variable = -Faktor(m00)-Faktor(m01)-Faktor(m10)-Faktor(m11);
			lnGiper = constant + variable;
			Giper = exp(lnGiper);
			clyq1 += Giper;
	}
		
	if ( clyq1 > 1.0 ) clyq1 = 1.0;
	if ( clyq1 > ZDev ) return(2); // Independent expertion

	return(3); //Positive correlation
}


int Rule::YuleCriterion(unsigned long (&m)[2][2], double BelInt, double ZDev, double& clyq1)
{
	long m00,m01,m10,m11;

	const UINT freq = 4;
	if(m[0][0]>freq && m[0][1]>freq && m[1][0]>freq && m[1][1]>freq)//нет частоты <=4
	{
			m00 = m[0][0];
			m01 = m[0][1];
			m10 = m[1][0];
			m11 = m[1][1];

			double KritQ, Q_;
			
			if(m01==0 || m10==0 )
			{
				clyq1 = 1.;
				return(3);
			}
			else if(m11*m00==m01*m10)
			{
				clyq1 = .0;
				return(2);
			}
			else if(m11==0 || m00==0)
			{
				clyq1 = -1.;
				return(1);
			}
			else
			{
				Q_ = double(m00*m11 - m01*m10)/double(m00*m11 + m01*m10);
				double qvant = InverseNormalCDF(1.-0.5*BelInt);
				double tail = 0.5*fabs(1-Q_*Q_)*sqrt( 1./m00 + 1./m01 + 1./m10 + 1./m11 );
				KritQ = Q_ - tail*qvant;
			}
			clyq1 = KritQ;
			if(KritQ < ZDev)
				return(2);
	}
	else
		clyq1 = 1.;


	return(3); //Positive correlation
}

//---------------------------------------------------------------------------
RuleSection& Rule::operator[](UINT nSecIndex)
{
	return Chain(nSecIndex);
}
//---------------------------------------------------------------------------
int Rule::getStringInfo(char* destString, char* CPVal)
{
	this->getChainStr(destString);
	sprintf(CPVal, "%f", this->getCP() );
	return(1);
}

// ---------------------------------------------------------------------------
std::string Rule::PredStr(int PredShift)
{
	std::ostringstream out;

	RuleSection Predicat = Chain(PredShift);
	out << "p" << Predicat.Shift+1 << "=";
	if (Predicat.Sign < 0) out << " not ";
	out << Predicat.Value;//SeqsStore->myCodeTable.Decode(Predicat.Value);

	return out.str();
}

// ---------------------------------------------------------------------------
std::string Rule::ConclusionStr()
{
	std::ostringstream out;

	out << " => p" << getTTPos()+1 << "=";
	if (getTTSign() == -1) out << " not ";
	out << getTTValue();//SeqsStore->myCodeTable.Decode(getTTValue());

	return out.str();

}

//---------------------------------------------------------------------------
void Rule::CountDeps(SEQStorage *SeqsStore, unsigned long (&m)[2][2], std::vector<int> TrueToken)
//	статистическая значимость закономерности считается
//	с учетом предиката OnlyThisTokenIsTrue.
//	если передается -1, то оценивается все правило
{
	RuleSection SendToken;
	bool A_Aplicable;
	bool B_Aplicable;
	bool isAplicable;

	m[0][0] = 0;
	m[0][1] = 0;
	m[1][0] = 0;
	m[1][1] = 0;

	int nTok;
	int spaces;
	for (long FilterCounter = 0; FilterCounter < SeqsStore->getLength(); FilterCounter++)
	{
		A_Aplicable = true;		//заключение
		B_Aplicable = true;     //посылка
		isAplicable = true;		//предикат OnlyThisTokenIsTrue должен выполняться на объекте
								//иначе он не несет вклада в статистическую значимость
		spaces=0;
		for ( UINT nChainIndex = 0; nChainIndex < getRuleLength(); nChainIndex++ )
		{
			if( SeqsStore->getElem_c(FilterCounter, Chain(nChainIndex).Shift, nTok) )
			{
				SendToken.Shift = Chain(nChainIndex).Shift;
				SendToken.Value = Chain(nChainIndex).Value;
				SendToken.Sign = Chain(nChainIndex).Sign;
				//ищем плохой исход
				if ( ( SendToken.Sign == 1 && nTok != SendToken.Value )
					|| ( SendToken.Sign == -1 && nTok == SendToken.Value ) )
				{
						if(!TrueToken.empty() && std::find(TrueToken.begin(), TrueToken.end(), nChainIndex) == TrueToken.end())
								isAplicable = false;
						else
								B_Aplicable = false;
				}
			}
			else
			{
				isAplicable = false;	
				break;
			}

		}
		if(isAplicable)
		{

			if( SeqsStore->getElem_c(FilterCounter, this->nTTPos, nTok) )
			{
				if ( (TTSign == 1 && nTok != Yvalue)
					|| (TTSign == -1 && nTok == Yvalue) )
					A_Aplicable = false;
				

			   if ( (A_Aplicable)&&(B_Aplicable) )
					m[1][1]++;
			   else if ( (A_Aplicable)&&(!B_Aplicable) )
					m[0][1]++;
			   else if ( (!A_Aplicable)&&(B_Aplicable) )
					m[1][0]++;
			   else if ( (!A_Aplicable)&&(!B_Aplicable) )
					m[0][0]++;
			}
			//else we skip add freq in table
		}
	}//end for [all objects]
}//end method


double InverseNormalCDF(double alpha)
{
	double res = 0, p = 0, t = 0, q = 0;

	if (alpha < 0.5)
		p = alpha;
	else
		p = 1 - alpha;
	
	t = sqrt(-2 * log(p));
	q = t - ((0.010328 * t + 0.802853) * t + 2.515517) / (((0.001308 * t + 0.189269) * t + 1.432788) * t + 1);
	
	if (alpha > 0.5)
		res = q;
	else
		res = -q;

	return res;
}
