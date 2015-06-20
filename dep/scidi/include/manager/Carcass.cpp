
#include "Carcass.h"
#include "../FileManager.h"

Carcass::Carcass(void)
{
    SeqCount = 0;
	CtrlPosSeqCount = 0;
	NegSeqCount = 0;
	NewSeqCount = 0;
	
	SeqArray = NULL;
	NegSeqArray = NULL;
	SeqArray_CrlNeg = NULL;
	SeqArray_CrP = NULL;
	
	m_SEQStorage = 0;
	m_FinishStorage = NULL;
	ObjsStorage = NULL;
	lObjsStorageSize = 0;
}

Carcass* Carcass::Instance()
{
	if(m_instance == 0){
		m_instance = new Carcass;
	}
	return m_instance;
}

Carcass::~Carcass(void)
{
    if(SeqArray!=NULL)
		delete[] SeqArray;
	if(SeqArray_CrlNeg!=NULL)
		delete[] SeqArray_CrlNeg;
	if(NegSeqArray!=NULL)
		delete[] NegSeqArray;
	if(SeqArray_CrP!=NULL)
		delete[] SeqArray_CrP;
	if(m_SEQStorage!=NULL)
		delete m_SEQStorage;
	if(m_FinishStorage!=NULL)
		delete m_FinishStorage;
	if(ObjsStorage!=NULL)
	{
		for ( long lStoreIndex = 0; lStoreIndex < lObjsStorageSize; lStoreIndex++ )
		{
			delete ObjsStorage[lStoreIndex];
		}
		delete[] ObjsStorage;
	}
}

Carcass* Carcass::m_instance = 0;
SEQStorage* Carcass::m_SEQStorage = 0;
RulesStorage* Carcass::m_FinishStorage = 0;
CIdelObject** Carcass::ObjsStorage = 0;
unsigned long Carcass::lObjsStorageSize = 0;


std::string Carcass::getSeq(int StoreType, long StoreIndex)
//функция возвращает СОДЕРЖАНИЕ последовательности из заданного хранилища
// Store type = ( 0 - positive, 1 - negative, 2 - ctrl )
{
	switch ( StoreType )
	{
			case 0:
			{
					if (StoreIndex > SeqCount ) return NULL;
					return SeqArray[StoreIndex].GetSeq();
			}
			case 1:
			{
					if (StoreIndex > NegSeqCount ) return NULL;
					return NegSeqArray[StoreIndex].GetSeq();
			}
			case 2:
			{
					if (StoreIndex > NewSeqCount ) return NULL;
					return SeqArray_CrlNeg[StoreIndex].GetSeq();
			}
			case 3:
			{
					if (StoreIndex > CtrlPosSeqCount ) return NULL;
					return SeqArray_CrP[StoreIndex].GetSeq();
			}

	}
}
//------------------------------------------------------------------------
int Carcass::getLength(int StoreType, long StoreIndex)
//функция возвращает СОДЕРЖАНИЕ последовательности из заданного хранилища
// Store type = ( 0 - positive, 1 - negative, 2 - ctrl )
{
switch ( StoreType )
{
		case 0:
		{
				if (StoreIndex > SeqCount ) return -1;
				return (int)SeqArray[StoreIndex].GetLength();
		}
		case 1:
		{
				if (StoreIndex > NegSeqCount ) return -1;
				return (int)NegSeqArray[StoreIndex].GetLength();
		}
		case 2:
		{
				if (StoreIndex > NewSeqCount ) return -1;
				return (int)SeqArray_CrlNeg[StoreIndex].GetLength();
		}
		case 3:
		{
				if (StoreIndex > CtrlPosSeqCount ) return -1;
				return (int)SeqArray_CrP[StoreIndex].GetLength();
		}
		default: return 0;

}
}
//---------------------------------------------------------------------------
std::string Carcass::getSeqName(int StoreType, long StoreIndex)
//функция возвращает ИМЯ последовательности из заданного хранилища
// Store type = ( 0 - positive, 1 - negative, 2 - ctrl )
{
	switch ( StoreType )
	{
			case 0:
			{
					if (StoreIndex > SeqCount ) return NULL;
					return SeqArray[StoreIndex].GetName();
			}
			case 1:
			{
					if (StoreIndex > NegSeqCount ) return NULL;
					return NegSeqArray[StoreIndex].GetName();
			}
			case 2:
			{
					if (StoreIndex > NewSeqCount ) return NULL;
					return SeqArray_CrlNeg[StoreIndex].GetName();
			}
			case 3:
			{
					if (StoreIndex > CtrlPosSeqCount ) return NULL;
					return SeqArray_CrP[StoreIndex].GetName();
			}
			default: return "";


	}

}
//---------------------------------------------------------------------------
/*
int Carcass::AddSeqs(const char* SourceFilePath, const char* regx, const char* tokenx)
//Добавляет последовательности в позитивную обучающую выборку
{
        FileManager ReadSeqFM;
        ReadSeqFM.Create(SourceFilePath, READSEQ_MODE);
		SeqCount = ReadSeqFM.GetObjCount(regx);
		if(SeqCount==0)
		{
			char strlog[256];
			strcpy(strlog, "В файле ");
			strcat(strlog, SourceFilePath);
			strcpy(strlog, " по данному регулярному выражению ничего не найдено.");
			MyForm->PrintLog(strlog);
			return RETURN_EMPTY;
		}
		if (SeqArray!=NULL)
			delete[] SeqArray;
		
		SeqArray = new Sequence[SeqCount];


		if (ReadSeqFM.ReadObjs(SeqArray, 0, regx, tokenx) == -1)
        {
				char strlog[256];
				strcpy(strlog, "Ошибка в чтении ");
				strcat(strlog, SourceFilePath);
				strcpy(strlog, ". Либо нет пустой строки в конце, либо проверьте регулярное выражение");
				MyForm->PrintLog(strlog);
                SeqCount = 0;
				return RETURN_ERROR;
		}

		SeqsIsLoading = true;
		ContinueProcess = false;

		return RETURN_SUCCESS;
}
*/
//---------------------------------------------------------------------------

bool Carcass::AddSeqs(const char* SourceFilePath)
//Добавляет последовательности в позитивную обучающую выборку
{
	
	/*
	FILE* f;
	UINT i, j, atrc=0, objc=0;
	char* data;
	char x;

	f = fopen(SourceFilePath, "r");
	fscanf(f, "%i\n", &atrc);
	fscanf(f, "%i\n", &objc);
	data = (char*) malloc( atrc * objc * sizeof(char));
	for (i = 0; i < objc; i++)
	{
		for(j = 0; j < atrc-1; j++)
		{
			fscanf(f, "%c", &x);
			data[i*atrc + j]  = x;
		}
		fscanf(f, "%c\n", &x);
		data[(i+1)*atrc - 1] = x; 
	}	
	fclose(f);
	SeqCount = objc;
	BUFFERS_SIZE = objc;
	*/

	if(m_SEQStorage!=NULL)
        delete m_SEQStorage;
    m_SEQStorage = FileManager::ReadCSVData(SourceFilePath);
	//if ( ObjsStorage == NULL )
	//{
	//	ObjsStorage = new CIdelObject*[objc];
	//}

	//SEQSElem* Seq = new SEQSElem(atrc);
	//for(unsigned m=0; m<SeqCount; m++)
	//{
	//	m_SEQStorage->CreateElem(m, Seq);
	//	ObjsStorage[m] = new CIdelObject();
	//	ObjsStorage[m]->Create(atrc, &m_SEQStorage->myCodeTable, Seq);
	//}
	//delete Seq;

	return RETURN_SUCCESS;
}

void Carcass::ClearObjects()
{
	if ( ObjsStorage != NULL && lObjsStorageSize > 0 )
	{
		for ( long lStoreIndex = 0; lStoreIndex < lObjsStorageSize; lStoreIndex++ )
		{
			delete ObjsStorage[lStoreIndex];
		}
		delete[] ObjsStorage;

		ObjsStorage = new CIdelObject*[ m_SEQStorage->getLength()/* + m_FinishStorage->getSize()*/];

		lObjsStorageSize = 0;
	}
	else
	{
		ObjsStorage = new CIdelObject*[ m_SEQStorage->getLength()/* + m_FinishStorage->getSize()*/];
	}

	m_SEQStorage->ClearClasses();
}
