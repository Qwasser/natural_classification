#include "SEQStorage.h"

#include <stdio.h>
#include <sstream>
#include <utility>
#include <algorithm>

SEQStorage::SEQStorage() : nWidth(0), nLength(0), nCodesCount(0), m_classes(NULL),
	NewStoreLength(0UL),
	LrnNegStoreLength(0UL),
	CtrlPosStoreLength(0UL) {}

SEQStorage::SEQStorage(Sequence* Array, long ArraySize) : SEQStorage()
{
	nLength = ArraySize;
	Storage.reserve(nLength);

	for (long i = 0; i < ArraySize; i++)
	{
		if(Array[i].GetLength() > nWidth)
			nWidth = Array[i].GetLength();
		
		SEQSElem elem(Array[i].GetLength());
		Parse(&Array[i], elem);
		Storage.push_back(elem);
	}

	nCodesCount = getCodesCount();
	CountFrequencies(0);
}

//! deprecated
SEQStorage::SEQStorage(char* arr, unsigned long nWidth_, unsigned long nLength_) : SEQStorage()
{
	m_classes = new int[nLength];
	for(unsigned i=0; i<nLength; i++)
		m_classes[i] = 0;
}

SEQStorage::~SEQStorage()
{	
	Storage.clear();
	LrnNegStorage.clear();
	CtrlPosStorage.clear();
	NewStorage.clear();

	ExecutObj.clear();
	TTFrequncies.clear();
}

void SEQStorage::ClearClasses()
{
	if(m_classes != 0) delete[] m_classes;
	m_classes = new int[nLength];
	for(unsigned i=0; i<nLength; i++)
		m_classes[i] = 0;
}
//-----------------------------------------------------------------------
std::string SEQStorage::printSeq(p_object obj)
{
	std::ostringstream output;
	for(unsigned int len = 0; len < obj->GetSize(); len++)
		output << Decode((*obj)[len]) << " ";
	std::string s_obj = output.str();
	if (s_obj.length() > MYMAXSTR) s_obj = s_obj.substr(0, MYMAXSTR - 1);
	return s_obj;
}

input_t SEQStorage::printSeq(c_objects::size_type i, char * c_string)
{
	std::ostringstream output;
	for(unsigned int len = 0; len < Storage[i].GetSize(); len++)
		output << Decode(Storage[i][len]) << " ";
	std::string s_obj = output.str();
	
	c_string[0] = '\0';
	if( s_obj.length() > MYMAXSTR ) s_obj = s_obj.substr(0, MYMAXSTR-1);
	strcpy(c_string, s_obj.c_str());
	return c_string;
}

input_t SEQStorage::printSeq_c(c_objects::size_type i, char * output)
{
	int length = 0;
	output[0] = '\0';

	for(unsigned int len = 0; len < Storage[i].GetSize(); len++)
	{
		const char* atr = Decode(Storage[i][len]);
		length += strlen(atr) + 1; // with space
		if(length > MYMAXSTR) return output;
		strcat(output, atr);
		strcat(output, " ");
	}
	return output;
}
//Заполняет таблицу частот встречаемости
void SEQStorage::CountFrequencies(unsigned long size)
{
	
	// Формируем структуру для хранения частот встречаемости каждого признака
	// в своем столбце
	nCodesCount = getCodesCount();

	if( TTFrequncies.empty() )
		for ( long lPosCounter = 0; lPosCounter < nWidth; lPosCounter++ )
		{
			std::vector<double> column(nCodesCount,.0);
			TTFrequncies.push_back(column);
		}

	if( size <= 0 || size >= nLength)
		size = nLength;

	long* lTokensCount = new long[nCodesCount];
	int nCurrToken;

	for ( long lPosCounter = 0; lPosCounter < nWidth; lPosCounter++ )
	{
		for ( int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++ )
		{
			lTokensCount[nCodesCounter] = 0;
		}

		for ( long lSesCounter = 0; lSesCounter < size; lSesCounter++ )
		{
			getElem_c(lSesCounter, lPosCounter, nCurrToken);
			if(nCurrToken>=0 && nCurrToken<nCodesCount)
				lTokensCount[nCurrToken]++;
		}
		for ( int nCodesCounter = 0; nCodesCounter < nCodesCount; nCodesCounter++ )
		{
			TTFrequncies[lPosCounter][nCodesCounter] = (double)(((double)lTokensCount[nCodesCounter])/((double)size));
		}
	}
	delete[] lTokensCount;

}

//---------------------------------------------------------------------------
void SEQStorage::CountExecutable()
{
	int nTokenValue;

	ExecutObj.reserve(2*nWidth*nCodesCount);
	for ( int nIndex = 0; nIndex < nWidth; nIndex++ )//Shift
	{
		for(int nValue = 0; nValue < nCodesCount; nValue++)//Value
		{
			std::vector<bool> OneP(nLength,true);	//Positive
			for (long ObjCounter = 0; ObjCounter < nLength; ObjCounter++)
			{
// 					SendToken.nPos = nIndex;
//					SendToken.nValue = nValue;
//					SendToken.Sign = 1;
					getElem(ObjCounter, nIndex, nTokenValue);
					if( nTokenValue != nValue )
						OneP[ObjCounter]=false;
					else
						OneP[ObjCounter]=true;

			}
			std::vector<bool> OneP2(nLength,true);	//Negative
			for (long ObjCounter = 0; ObjCounter < nLength; ObjCounter++)
			{
//					SendToken.nPos = nIndex;
//					SendToken.nValue = nValue;
//					SendToken.Sign = -1;
					getElem(ObjCounter, nIndex, nTokenValue);
					if( nTokenValue == nValue )
						OneP2[ObjCounter]=false;
					else
						OneP2[ObjCounter]=true;

			}
			ExecutObj.push_back(OneP);
			ExecutObj.push_back(OneP2);
		}
	}
 /*
 	FILE *data = fopen("executobj.txt","w");
	if(!ExecutObj.empty())
	{
		fprintf(data,"size: %i, width: %i, nCodesCount: %i\n", ExecutObj.size(), nWidth, nCodesCount);
		for ( int nIndex = 0; nIndex < nWidth; nIndex++ )//Shift
		{
			for(int nValue = 0; nValue < nCodesCount; nValue++)//Value
			{
				fprintf(data,"[%i](x%i = a%i)\t\t",int(2*nCodesCount*nIndex + nValue*2),nIndex,nValue);
				for (long ObjCounter = 0; ObjCounter < nLength; ObjCounter++)
				{
					if(ExecutObj[2*nCodesCount*nIndex + nValue*2][ObjCounter])
						fprintf(data,"1");
					else
						fprintf(data,"0");
				}
				fprintf(data,"\n");
				fprintf(data,"[%i](x%i not= a%i)\t",int(2*nCodesCount*nIndex + nValue*2 + 1),nIndex,nValue);
				for (long ObjCounter = 0; ObjCounter < nLength; ObjCounter++)
				{
					if(ExecutObj[2*nCodesCount*nIndex + nValue*2 + 1][ObjCounter])
						fprintf(data,"1");
					else
						fprintf(data,"0");
				}
				fprintf(data,"\n");
			}

		}
	}
	fclose(data);
   */
}
//--------
//Нахождение по предикату "массив выполнимости"
//вектор булевых значений выполнимости предиката на данных
//которые хранятся в данном классе
std::vector<bool> SEQStorage::getOneExecute(int Shift, int Sign, int SendToken)
{
	int i = 2*nCodesCount*Shift + SendToken*2 + int(0.5*(1 - Sign));
	return ExecutObj[i];
}

bool SEQStorage::getElem(int oj, int xi, int &el)
{
	el = (int)Storage[oj][xi];
	if( hasLacks() == VERSION_WITH_GAPS )
	{
		bool mean_space;
		if( Decode(el) == Decode(getCodesCount()) )
			mean_space = false;
		else
			mean_space = true;
		return mean_space;
	}
	else
		return true;
}

bool SEQStorage::getElem_c(int oj, int xi, int &el)
{
	el = (int)Storage[oj][xi];
//	if( getCodesCount() > 0 )
//		el = getCode( el );
	if( hasLacks() == VERSION_WITH_GAPS )
	{
		bool mean_space;
		if( el == getCodesCount() )
			mean_space = false;
		else
			mean_space = true;
		return mean_space;
	}
	else
		return true;
}

double SEQStorage::getFreq(long n, int a)
{
	if(!TTFrequncies.empty() && 
		TTFrequncies.size() > (unsigned long)n && 
		!TTFrequncies[n].empty() &&
		TTFrequncies[n].size() > (unsigned int)a)
	
		return TTFrequncies[n][a];
	else return 0.0;
}


void SEQStorage::CreateElem(unsigned from, SEQSElem *dest)
{
	for(int i=0; i<nWidth; i++)
		dest->AddValue(Storage[from][i]);
}

void SEQStorage::setClass(int id, int nCL)
{
	m_classes[id] = nCL;
}

int SEQStorage::getClass(int id)
{
	if( m_classes == 0 ) return -1;
	else return m_classes[id];
}

void SEQStorage::Conclude()
{
	CountFrequencies(0);
	CountExecutable();
}
