//---------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "CIdelObject.h"

//---------------------------------------------------------------------------

CIdelObject::CIdelObject()
{
	IdelObject = NULL;
	lLength = 0;
	nCodesCount = 0;
	dGammaValue = 0;

}
//---------------------------------------------------------------------------
CIdelObject::CIdelObject(const CIdelObject& replicant)
{
	this->lLength = replicant.lLength;
	this->nCodesCount = replicant.nCodesCount;
	this->dGammaValue = replicant.dGammaValue;

	IdelObject = new bool* [lLength];
	for (unsigned long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
	{
		IdelObject[lLengthCounter] = new bool[nCodesCount];
		for (unsigned int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++)
		{
			IdelObject[lLengthCounter][nCodesCounter] = replicant.IdelObject[lLengthCounter][nCodesCounter];
		}
	}
}
CIdelObject& CIdelObject::operator= (const CIdelObject& c)
{
	if(this != &c)
	{
		for (unsigned long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
		{
			delete[] IdelObject[lLengthCounter];
		}
		delete[] IdelObject;
		RulMarks.clear();

		this->lLength = c.lLength;
		this->nCodesCount = c.nCodesCount;
		this->dGammaValue = c.dGammaValue;

		IdelObject = new bool* [lLength];
		for (unsigned long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
		{
			IdelObject[lLengthCounter] = new bool[nCodesCount];
			for (unsigned int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++)
			{
				IdelObject[lLengthCounter][nCodesCounter] = c.IdelObject[lLengthCounter][nCodesCounter];
			}
		}
	}
	return *this;
}

CIdelObject::~CIdelObject()
{

	for (unsigned long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
	{
		delete[] IdelObject[lLengthCounter];
	}
	delete[] IdelObject;
	RulMarks.clear();

}

//---------------------------------------------------------------------------
//Процедура создает последовательность в формате идеального объекта
bool CIdelObject::Create(long lLength, int nCodesCount, SEQSElem* InitSeq)
{
	this->lLength = lLength;
	this->nCodesCount = nCodesCount;

	IdelObject = new bool* [lLength];
	for (long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
	{
		IdelObject[lLengthCounter] = new bool[nCodesCount];
		for (int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++)
		{
			IdelObject[lLengthCounter][nCodesCounter] = false;
		}
		int nInitTokenValue = (*InitSeq)[lLengthCounter];
		if ( nInitTokenValue < nCodesCount && nInitTokenValue >=0)
		{
			IdelObject[lLengthCounter][nInitTokenValue] = true;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Создает новый идаельный объект бес признаков
bool CIdelObject::Create(long lLength, int nCodesCount)
{
	this->lLength = lLength;
	this->nCodesCount = nCodesCount;
		SToken* Token = new SToken;


	IdelObject = new bool* [lLength];
	for ( Token->nPos = 0; Token->nPos < (unsigned long)lLength; Token->nPos++)
	{
		IdelObject[Token->nPos] = new bool[nCodesCount];
		for ( Token->nValue = 0; Token->nValue < nCodesCount; Token->nValue++)
		{
			IdelObject[Token->nPos][Token->nValue] = false;
		}
	}
	return true;
}

void CIdelObject::Create(long lLength, SEQSElem* InitSeq)
{

	this->lLength = lLength;
	//заполняем динамический массив
	int c1, c2=-1, countL=0;
	bool new_c;
	std::vector< std::vector<bool> > dynamic;
	for(int i=0; i<lLength; i++)
	{
		c1 = (*InitSeq)[i];
		new_c = true;
		for(int k=0; k<i; k++)
			if( c1 == (*InitSeq)[k] )
			{
				new_c = false;
				for(UINT m=0; m<dynamic.size(); m++)
					if( dynamic[m][k] )
					{
						c2 = m;
						break;
					}
				break;
			}

		if(new_c)
		{
			//CodeTable->Coding( c1 );
			std::vector<bool> row(lLength, false);
			dynamic.push_back(row);
			if(!dynamic.empty() && (*InitSeq)[i]==c1)
				dynamic[dynamic.size()-1][i] = true;
		}
		else
			if(!dynamic.empty() && (*InitSeq)[i]==c1)
				dynamic[c2][i] = true;
	}
		
	// dynamic [code] [atribute]		

	//создаем таблицу объекта
	this->nCodesCount = dynamic.size();  //=======
	IdelObject = new bool* [lLength];
	for (unsigned long lLengthCounter = 0; lLengthCounter < (unsigned long)lLength; lLengthCounter++)
	{
		IdelObject[lLengthCounter] = new bool[nCodesCount];
		for (unsigned int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++)
		{
			IdelObject[lLengthCounter][nCodesCounter] = dynamic[nCodesCounter][lLengthCounter];
		}
	}
}

void CIdelObject::Create(SEQSElem* InitSeq, unsigned int n_codes)
{
	lLength = InitSeq->GetSize();
	nCodesCount = n_codes;
	//=======
	IdelObject = new bool* [lLength];
	int nInitTokenValue;
	for (unsigned long lLengthCounter = 0; lLengthCounter < lLength; lLengthCounter++)
	{
		IdelObject[lLengthCounter] = new bool[nCodesCount];
		for (unsigned int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++)
		{
			IdelObject[lLengthCounter][nCodesCounter] = false;
		}
		nInitTokenValue = (*InitSeq)[lLengthCounter];
		if ( nInitTokenValue < nCodesCount && nInitTokenValue >=0)
		{
			IdelObject[lLengthCounter][nInitTokenValue] = true;
		}
	}
}

//---------------------------------------------------------------------------
bool CIdelObject::Create(long lLength, int nCodesCount, Rule* R)
{
	bool positivPredicats = true; //закономерность не содержит отрицаний


	this->lLength = lLength;
	this->nCodesCount = nCodesCount;
	SToken* Token = new SToken;


	IdelObject = new bool* [lLength];
	for ( Token->nPos = 0; Token->nPos < (unsigned long)lLength; Token->nPos++)
	{
		IdelObject[Token->nPos] = new bool[nCodesCount];
		for ( Token->nValue = 0; Token->nValue < nCodesCount; Token->nValue++)
		{
			IdelObject[Token->nPos][Token->nValue] = false;
		}
	}

	SToken CurrToken;
	SToken SendToken;
	double TTVal=0.;
	//посылка
	int nRuleLength = R->getRuleLength();
	for (int l=0; l < nRuleLength; l++)
	{
		SendToken.nPos = (*R)[l].Shift;
		SendToken.nValue = (*R)[l].Value;
		SendToken.Sign = (*R)[l].Sign;
		if ( SendToken.Sign < 0 )
		{
			positivPredicats = false;
			CurrToken.nPos = SendToken.nPos;
			for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++)
			{
					if (CurrToken.nValue != SendToken.nValue )
						this->IncludeT(&CurrToken);
			}
		}
		else if ( SendToken.Sign > 0 )
		{
			this->IncludeT(&SendToken);
		}
	}
	//заключение
	CurrToken.nPos = R->getTTPos();
	SendToken.nPos = CurrToken.nPos;
	TTVal = R->getCP();
	SendToken.nValue = R->getTTValue();
	if ( TTVal < 0 )
	{
			CurrToken.nPos = SendToken.nPos;
			for ( CurrToken.nValue = 0; CurrToken.nValue < nCodesCount; CurrToken.nValue++)
			{
					if (CurrToken.nValue != SendToken.nValue )
					{
							this->IncludeT(&CurrToken);
					}
			}
			positivPredicats = false;
	}
	else if ( TTVal > 0 )
	{
					this->IncludeT(&SendToken);
	}

	return positivPredicats;
}


//Проверяет на принадлежность признака идеальному объекту
bool CIdelObject::isBelong(SToken* Token)
{
        if ( (Token->nValue < 0) || (Token->nValue >= nCodesCount) )
                return false;
        else
	        return IdelObject[Token->nPos][Token->nValue];
}

//Проверяет на принадлежность признака идеальному объекту
bool CIdelObject::isBelong(RuleSection* Token)
{
        if ( (Token->Value < 0) || (Token->Value >= nCodesCount) )
                return false;
        else
	        return IdelObject[Token->Shift][Token->Value];
}

bool CIdelObject::isBelong(unsigned long pos, unsigned long value)
{
        if ( (value < 0) || (value >= nCodesCount) )
                return false;
        else
	        return IdelObject[pos][value];
}

//---------------------------------------------------------------------------
//Если в идеальном объекте присутствую на какой-то позиции все вопризнаки,
//то признаки удаляются, то есть [acgt] = []
void CIdelObject::Minimize()
{
	int nTokensCount;
	for ( unsigned long lPosCounter = 0; lPosCounter < lLength; lPosCounter++ )
	{
		nTokensCount = 0;
		for ( unsigned int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
		{
			if ( IdelObject[lPosCounter][nCodeCounter] )
			{
				nTokensCount++;
			}
		}
		if ( nTokensCount == nCodesCount )
		{
			for ( unsigned int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
			{
				IdelObject[lPosCounter][nCodeCounter] = false;
			}
		}
	}
}

//---------------------------------------------------------------------------
//Выдает идальный объект как строку
int CIdelObject::getObjAsStr(char* chObject)
{
	std::ostringstream out;
	CodeTable* Context = CodeTable::Instance();
	bool PosIsEmpty = true;
	
	for ( unsigned long lPosCounter = 0; lPosCounter < lLength; lPosCounter++ )
	{
		PosIsEmpty = true;
		for ( unsigned int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
		{

			if ( IdelObject[lPosCounter][nCodeCounter] )
			{
				out << Context->Decode(nCodeCounter) << ","; //codeTable here!
				PosIsEmpty = false;
			}
		}
		if ( PosIsEmpty )
		{
			out << " ";
		}
		out << ";";
	}
	std::string conv = out.str();
	if( conv.length() > MYMAXSTR )
		conv = conv.substr(0, MYMAXSTR-1);
	chObject[0] = '\0';
	strcpy(chObject, conv.c_str());
	return 0;
}

int CIdelObject::getObjAsStr(char* chObject, std::function<input_t (code_t)> f_decode)
{
	std::ostringstream out;
	bool PosIsEmpty = true;
	chObject[0] = '\0';
	
	for ( unsigned long lPosCounter = 0; lPosCounter < lLength; lPosCounter++ )
	{
		PosIsEmpty = true;
		for ( unsigned int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
		{

			if ( IdelObject[lPosCounter][nCodeCounter] )
			{
				out << f_decode(nCodeCounter); //codeTable here!
				PosIsEmpty = false;
			}
		}
		if ( PosIsEmpty )
		{
			out << " ";
		}
		out << " ";
	}
	std::string conv = out.str();
	if( conv.length() > MYMAXSTR )
		conv = conv.substr(0, MYMAXSTR-1);
	strcpy(chObject, conv.c_str());
	return 0;
}

bool CIdelObject::getObjAsVector(bool** b, int len, int code)
{

	if(len == lLength && code == nCodesCount)
	{
		b = IdelObject;
		/*for ( long lPosCounter = 0; lPosCounter < lLength; lPosCounter++ )
		{
			for ( int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
			{
				b[lPosCounter][nCodeCounter] = IdelObject[lPosCounter][nCodeCounter];
			}
		}*/
		return true;
	}
	return false;
}

bool CIdelObject::isIt(int i1, int i2)
{
	if(i1 < 0 || i2 < 0)
		return false;
	if(i1 < lLength && i2 < nCodesCount)
		return IdelObject[i1][i2];
	return false;
}
//---------------------------------------------------------------------------
 /*
bool CIdelObject::isEmpty()
{
	for ( long lPosCounter = 0; lPosCounter < lLength; lPosCounter++ )
	{
		for ( int nCodeCounter = 0; nCodeCounter < nCodesCount; nCodeCounter++ )
		{
			if ( IdelObject[lPosCounter][nCodeCounter] )
			{
				return (false);
			}
		}
	}
	return(true);
}  */
//---------------------------------------------------------------------------
//Удаление признака из идеального объекта
void CIdelObject::ExcludeT(SToken* Token)
{
	IdelObject[Token->nPos][Token->nValue] = false;
}

//---------------------------------------------------------------------------
//Включение признака в ид. объект
void CIdelObject::IncludeT(SToken* Token)
{
	IdelObject[Token->nPos][Token->nValue] = true;
}
//---------------------------------------------------------------------------
int CIdelObject::getValue(int Pos)
{
		SToken SendToken;
		SendToken.nPos = Pos;

		for( SendToken.nValue = 0; SendToken.nValue < nCodesCount; SendToken.nValue++)
		{
				if ( isBelong(&SendToken))
						return SendToken.nValue;
		}
        return -1;	
}
