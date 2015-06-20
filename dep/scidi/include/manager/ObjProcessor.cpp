
#include "ObjProcessor.h"
#include "../Thread.h"

ObjProcessor::ObjProcessor(void)
{
}

ObjProcessor::ObjProcessor(Classificator* pC)
{
    myClassificator = pC;
}

ObjProcessor::~ObjProcessor(void)
{
}

void ObjProcessor::start(ProcessorSettings* set)
{
	//! TODO
	/*
	if ( myClassificator == NULL )
	return __IDMSG_NOOBJSFORCLASSIFICATION_;

	if ( myClassificator->lObjsStorageCounter == 0 )
	return __IDMSG_NOOBJSFORCLASSIFICATION_;

	if ( SeqArray_CrlNeg == NULL )
	return __IDMSG_NOSEQFORCLASSIFICATION_;

	if ( nRecClass != -1 && RecSourcePos != -1 && RecSourceNeg != -1)
	{
	ErrCountPosSource = RecSourcePos;
	ErrCountNegSource = RecSourceNeg;
	}

	this->nRecClass = nRecClass;
	if(nRecClass == -1)
	m_SEQStorage->Add(SeqArray_CrlNeg, NewSeqCount, "ControlNegative");
	*/
	std::shared_ptr<Runnable> r(new ObjRunnable(set->myClassificator));
	std::shared_ptr<Thread> thread1(new Thread(r));
	thread1->start();
}

void ObjProcessor::terminate()
{
	_thread->terminate();
}

bool ObjProcessor::is_completed()
{
	return _thread->is_completed();
}

bool ObjProcessor::in_progress()
{
	return _thread->in_progress();
}

//-------------------------------------------------------------
int ObjProcessor::CreateClassificator()
{
	if ( (m_SEQStorage == NULL) || (m_SEQStorage->getLength() == 0) )
		return __IDMSG_NOSEQFORIDEALIZ_;

	if ( (m_FinishStorage == NULL) || (m_FinishStorage->getSize() == 0) )
		return __IDMSG_NORULESFORIDEALIZ_;

	if ( myClassificator != NULL )
	{
		delete myClassificator;
	}
	myClassificator = new Classificator(__IDEALIZTYPE_OFFICIAL_);

	myClassificator->Create(m_SEQStorage, m_FinishStorage, ObjsStorage);
	return __IDMSG_TASKCOMPLETE_;
}

//---------------------------------------------------------------------------
//Подготовка к началу классификации контр. выборки
int ObjProcessor::StartNewIdeal()
{
	CreateClassificator();
	
	if ( myClassificator == NULL )
	{
		return __IDMSG_NOOBJSFORCLASSIFICATION_;
	}

	myClassificator->Restore(m_SEQStorage, m_FinishStorage, ObjsStorage, lObjsStorageSize);
	/*if ( myClassificator->lObjsStorageCounter == 0 )
	{
		return __IDMSG_NOOBJSFORCLASSIFICATION_;
	}


 /*       if ( SeqArray_CrlNeg == NULL )
		{
				return __IDMSG_NOSEQFORCLASSIFICATION_;
		}


		if ( !CtrlNegSeqsAreClassified )
				m_SEQStorage->Add(SeqArray_CrlNeg, NewSeqCount);   // may be should write "2" - default

		if ( CtrlPosSeqsAreLoaded && !CtrlPosSeqsAreClassified )
				m_SEQStorage->Add( SeqArray_CrP, CtrlPosSeqCount, 1);

 */		//SHIFT_POSITION
//	if(NParametrs.nRulesSearchType == AT_FIXED_POSITIONS)
		m_SEQStorage->Add(SeqArray_CrP, CtrlPosSeqCount, "SeqArray_CrP");
//	else
//		m_SEQStorage->Add(SeqArray_CrP, CtrlPosSeqCount, NParametrs.nScanFrameWidth, Predicat);

	return __IDMSG_THREADSTARTERROR_;
}

//---------------------------------------------------------------------------
// Классификация контрольной выборки
//---------------------------------------------------------------------------
void ObjProcessor::NewIdealThread()
{
	SEQSElem* CurrSeq = NULL;
	int Count, nClNumber;
	double MaxFVal,FVal=-1.;
 
	Count = NewSeqCount;
	
	for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
	{
		if (!GetStatus("processing")) break;
		CurrSeq = &m_SEQStorage->NewStorage[SeqCounter];
		nClNumber = myClassificator->OneIter( CurrSeq , SeqCounter + 1);
		if ( nClNumber == -1)
        {
            for ( int nClassNumber = 0; nClassNumber < lObjsStorageSize; nClassNumber++)
            {
                CurrSeq = &m_SEQStorage->NewStorage[SeqCounter];
 		        FVal = myClassificator->CountF( CurrSeq, SeqCounter, nClassNumber );
                if(nClassNumber == 0 || MaxFVal < FVal)
                {
                    MaxFVal=FVal;
                    nClNumber = nClassNumber;
                }
            }
		}
		m_SEQStorage->NewStorage[SeqCounter].SetClassId(nClNumber);
		//MyForm->SetNewClassNumber(SeqCounter + 1, nClNumber + 1, false);

	}
	
	// == positive storage classification here ==
	// == prediction mode commented
	if(CtrlPosSeqCount > 0)
    {
		Count = m_SEQStorage->CtrlPosStoreLength;
		//FILE* f1 = fopen("control.log","w");
		for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
		{
			if (!GetStatus("processing")) break;
			CurrSeq = &m_SEQStorage->CtrlPosStorage[SeqCounter];
			//fprintf(f1, "%s - ", m_SEQStorage->printSeq(CurrSeq));
			//int temp = (*CurrSeq)[0];
			//(*CurrSeq)[0] = m_SEQStorage->getCodesCount();
			nClNumber = myClassificator->OneIter( CurrSeq , SeqCounter + 1);
			if ( nClNumber == -1)
			{
				MaxFVal=FVal;
				nClNumber = 0;
				for ( int nClassNumber = 0; nClassNumber < lObjsStorageSize; nClassNumber++)
				{
					FVal = myClassificator->CountF( CurrSeq, SeqCounter, nClassNumber );
					if ( MaxFVal < FVal)
					{
						MaxFVal=FVal;
						nClNumber = nClassNumber;
					}
				}
				m_SEQStorage->CtrlPosStorage[SeqCounter].SetClassId(nClNumber);
				//MyForm->SetCtrlPosClassNumber(SeqCounter + 1, std::string(IntToStr(nClNumber+1).t_str()), true);
			}
			else
			{
				m_SEQStorage->CtrlPosStorage[SeqCounter].SetClassId(nClNumber);
				//MyForm->SetCtrlPosClassNumber(SeqCounter + 1, std::string(IntToStr(nClNumber+1).t_str()), false);
			}
			//(*CurrSeq)[0] = temp;
			//fprintf(f1, "%i\n", nClNumber);
		}
		//fclose(f1);
	}
	EndNewIdeal();
}

//---------------------------------------------------------------------------
void ObjProcessor::EndNewIdeal()
{
}
//---------------------------------------------------------------------------
//Процедура распознавания контр. выборки
//также считает скор по заднному класс при поиске ошибок
/*
void ObjProcessor::RecognitionThread()
{
	SEQSElem* CurrSeq = NULL;
    int Count;
    double FVal;
    Count = NewSeqCount;
    int LSCount=0;

    if (nRecClass == -1)//подсчитывает очки
    {
		for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
		{
			for ( int nClassNumber = 0; nClassNumber < lObjsStorageSize; nClassNumber++)
			{
				CurrSeq = &m_SEQStorage->NewStorage[SeqCounter];
				FVal = myClassificator->CountF( CurrSeq, SeqCounter, nClassNumber );
				MyForm->SetFValue(SeqCounter, FVal, nClassNumber);

			}
		}
	LSCount = -2;
	}
	else
	{  //оценивает правильность распознания контроля
	   if ( ErrCountNegSource == __ERRCOUNTNEG_CONTROL_ )
	   {
			Count = NewSeqCount;
			for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
			{
						if ( m_SEQStorage->NewStorage[SeqCounter].GetClassId() == nRecClass)
						{
						CurrSeq = &m_SEQStorage->NewStorage[SeqCounter];
							FVal = myClassificator->CountF( CurrSeq, SeqCounter, nRecClass );
							//	m_SEQStorage->NewStorage[SeqCounter].FVal = FVal;
								MyForm->SetFValue3(SeqCounter, FVal,3);

						}
			}
	   }
		else
		{
			Count = NegSeqCount;
			for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
			{
						if ( m_SEQStorage->LrnNegStorage[SeqCounter].GetClassId() == nRecClass)
						{
							CurrSeq = &m_SEQStorage->LrnNegStorage[SeqCounter];
							FVal = myClassificator->CountF( CurrSeq, SeqCounter, nRecClass );
							//m_SEQStorage->LrnNegStorage[SeqCounter].FVal = FVal;
							MyForm->SetFValue3(SeqCounter, FVal,1);
						}
			}
		}

	   if ( ErrCountPosSource == __ERRCOUNTPOS_CONTROL_ )
//           if ( ErrRecForm->RadioGroup1->ItemIndex == 1 )
	   {
			Count = CtrlPosSeqCount;
			for (int SeqCounter = 0; SeqCounter < Count; SeqCounter++)
			{
						if ( m_SEQStorage->CtrlPosStorage[SeqCounter].GetClassId() == nRecClass)
						{
							CurrSeq = &m_SEQStorage->CtrlPosStorage[SeqCounter];
							FVal = myClassificator->CountF( CurrSeq, SeqCounter, nRecClass );
							//m_SEQStorage->CtrlPosStorage[SeqCounter].FVal = FVal;
							MyForm->SetFValue3(SeqCounter, FVal,2);
							LSCount++;
						}
			}
	   }
	   else
		for (int i=0; i < m_SEQStorage->getLength() ; i++)
		{
			if ( m_SEQStorage->getClass(i) == nRecClass)
			{
					m_SEQStorage->CreateElem(i, CurrSeq);
					FVal = myClassificator->CountF( CurrSeq, i, nRecClass );
					//m_SEQStorage->Storage[i].FVal = FVal;
					MyForm->SetFValue3(i, FVal,0);
					LSCount++;
			}
		}

   }

	EndRecognition(LSCount);
}
*/
//---------------------------------------------------------------------------
void ObjProcessor::EndRecognition(int LSCount)
{
    if(LSCount == -2)
    {
        m_SEQStorage->RemoveNewStorage(false);
        ControlRecognized = true;
        return;
    }
    RecErrCountDraw(LSCount);
    ControlRecognized = false;
}
//---------------------------------------------------------------------------
/*int ObjProcessor::NextRecognition( )
{

        TGridRect myRect;
        myRect = IdealizObjectsList->Selection;

        long count=0;
        for ( long i=1; i < SequencesList->RowCount; i++)
        {
                if ( (SequencesList->Cells[4][i] == IntToStr(nRecClass + 1)) &&
                     (SequencesList->Cells[6][i] == "-NotRecognized") )
                count++;
		}
        MyForm->IdealizObjectsList->Cells[4][nRecClass + 1] = IntToStr(count);

		count=0;
        for ( long i=1; i < NewSeqsList->RowCount; i++)
        {
                if ( (NewSeqsList->Cells[4][i] == IntToStr(nRecClass + 1)) &&
                     (NewSeqsList->Cells[6][i] == "+Recognized") )
                count++;
        }
        MyForm->IdealizObjectsList->Cells[5][nRecClass + 1] = IntToStr(count);

        MyForm->IdealizObjectsList->Cells[3][nRecClass + 1] = RecLevelEdit->Text;

        if ( MyForm->btnNextClassErrCount->Caption == "Total")
        {
                MyForm->btnNextClassErrCount->Caption = "Next";
                MyForm->btnNextClassErrCount->Enabled = false;

                MyForm->UnblockAllButtons();

                MyForm->IdealizObjectsList->Enabled = true;
                MyForm->Chart1->Visible = false;
                MyForm->btnHideShowChart->Enabled = false;
                MyForm->FirstLErrEd->Enabled = false;
                MyForm->RecLevelEdit->Enabled = false;
                long FECount=0;
                long SECount=0;
                long FTotal=0;
                for ( int i= myRect.Top; i <= myRect.Bottom; i++)
                {
                        for ( long j=1; j < SequencesList->RowCount; j++)
                            if ( StrToInt(SequencesList->Cells[4][j]) == i)
                                FTotal++;
                        FECount += StrToInt(IdealizObjectsList->Cells[4][i]);
                        SECount += StrToInt(IdealizObjectsList->Cells[5][i]);
                }
                double fErr;
                fErr = (double)FECount/(double)FTotal;
                FirstLErrEd->Text = FloatToStrF(fErr, 2 , 8, 8);
                AnsiString s = "Recognized " + IntToStr(FTotal - FECount) + " out of " + IntToStr(FTotal);
                FstErrInW->Caption = s;

                fErr = (double)SECount/(double)NewSeqCount;
                SecondLErr->Caption = FloatToStrF(fErr, 2 , 8, 8);
				s = "Recognized " + IntToStr(SECount) + " out of " + IntToStr(NewSeqCount);
                SecErrInW->Caption = s;
                SaveRecErrBtn->Enabled = true;
				return;
        }

        StartRecognition(nRecClass + 1);


        if ( nRecClass == myRect.Bottom - 1)
        {
                btnNextClassErrCount->Caption = "Total";
        }


        Chart1->Visible = true;
        btnHideShowChart->Enabled = true;
        FirstLErrEd->Enabled = false;
        RecLevelEdit->Enabled = false;

	return -1;
} */
//-------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Процедура распознавания длинной последовательности и вывод графика
int ObjProcessor::StartRecDrawing(long nClassNumber )
{
        if ( myClassificator == NULL )
	{
		return __IDMSG_NOOBJSFORCLASSIFICATION_;
	}

	if ( myClassificator->lObjsStorageCounter == 0 )
	{
		return __IDMSG_NOOBJSFORCLASSIFICATION_;
	}

        if ( SeqArray_CrlNeg == NULL )
		{
                return __IDMSG_NOSEQFORCLASSIFICATION_;
        }

        if (nClassNumber != CheckClNumber)
        {
                CheckClNumber = nClassNumber;
//                delete[] DrawArray;

                m_SEQStorage->Add(SeqArray_CrlNeg, NewSeqCount, "DrawArray");

                DrawArray = new double [m_SEQStorage->NewStoreLength];
                for ( long i=0; i<m_SEQStorage->NewStoreLength; i++)
                {
                        DrawArray[i] = 0.0;
                }


                long nnSeqCount;
	        SEQSElem* CurrSeq = NULL;

                nnSeqCount = m_SEQStorage->NewStoreLength;
/*
Сделать проверку nClassNumber
*/
                for ( int PosCounter = 0; PosCounter < nnSeqCount ; PosCounter++)
                {

                        CurrSeq = &m_SEQStorage->NewStorage[PosCounter];
                        DrawArray[PosCounter] =
                        myClassificator->CountF( CurrSeq, PosCounter, nClassNumber);
                }

//                MyForm->RecChartDraw( m_SEQStorage->NewStoreLength );
                m_SEQStorage->RemoveNewStorage();

/////////////////////////////////////

        }else
        {
                ;//MyForm->RecChartDraw( m_SEQStorage->NewStoreLength );
        }




	return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Рисует гистограмму при поиске ошибок
void ObjProcessor::RecErrCountDraw( int LnStCount )
{
 /*
		double fval;
        long y;
        MyForm->Series1->Clear();
        MyForm->Chart1->LeftAxis->AutomaticMaximum = false;
        MyForm->Chart1->LeftAxis->Maximum = 1.1;
        MyForm->Chart1->LeftAxis->AutomaticMinimum = false;
        MyForm->Chart1->LeftAxis->Minimum = 0.0;
        MyForm->Chart1->LeftAxis->Title->Caption = "Percentage of sampling";
        MyForm->Chart1->BottomAxis->Title->Caption = "Score";

        double st = 10.;//const st = 10;
        double Ost;
        double fMaxVal = 0.;
        bool bFirst = true;


    if ( ErrCountNegSource == __ERRCOUNTNEG_CONTROL_ )
    {

        for (int i=0; i< NewSeqCount; i++)
        {
            if ( m_SEQStorage->NewStorage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->NewStorage[i].FVal;

                if ( bFirst )
                {
                        bFirst = false;
                        fMaxVal = fval;
                }
                else if ( fMaxVal < fval )
                        fMaxVal = fval;

                Ost = fval;//Ost = st*floor(fval/(double)st) + (double)st/(double)2;

                y = MyForm->Series1->XValues->Locate(Ost);

                if ( y == -1)
                        MyForm->Series1->AddXY(Ost,(double)1/(double)(NewSeqCount),"",clTeeColor);
                else
                        MyForm->Series1->YValues->Value[y] += (double)1/(double)(NewSeqCount);
            }
        }
    }
    else
	{

        for (int i=0; i< NegSeqCount; i++)
        {
            if ( m_SEQStorage->LrnNegStorage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->LrnNegStorage[i].FVal;

                if ( bFirst )
                {
                        bFirst = false;
                        fMaxVal = fval;
                }
                else if ( fMaxVal < fval )
                        fMaxVal = fval;

                Ost = fval;//Ost = st*floor(fval/(double)st) + (double)st/(double)2;

                y = MyForm->Series1->XValues->Locate(Ost);

                if ( y == -1)
                        MyForm->Series1->AddXY(Ost,(double)1/(double)(NegSeqCount),"",clTeeColor);
                else
                        MyForm->Series1->YValues->Value[y] += (double)1/(double)(NegSeqCount);
            }
        }
    }

        MyForm->Series2->Clear();

        double fMinVal = 0.;
        bFirst = true;
        if ( ErrCountPosSource == __ERRCOUNTPOS_CONTROL_ )
        for (int i=0; i< CtrlPosSeqCount; i++)
        {
            if (m_SEQStorage->CtrlPosStorage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->CtrlPosStorage[i].FVal;

                if ( bFirst )
                {
                        bFirst = false;
                        fMinVal = fval;
                }
                else if ( fMaxVal > fval )
						fMinVal = fval;

                // i change there!!!!!!!!
                Ost = fval;//st*floor(fval/st) + (double)st/2;//Ost = st*floor(fval/st) + (double)st/2;

                y = MyForm->Series2->XValues->Locate(Ost);

                if ( y == -1)
                        MyForm->Series2->AddXY(Ost,(double)1/(LnStCount),"",clTeeColor);
                else
                        MyForm->Series2->YValues->Value[y] += (double)1/(LnStCount);
            }
        }
        else
        for (int i=0; i< SeqCount; i++)
        {
            if (m_SEQStorage->Storage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->Storage[i].FVal;

                if ( bFirst )
                {
                        bFirst = false;
                        fMinVal = fval;
                }
                else if ( fMaxVal > fval )
                        fMinVal = fval;

                Ost = fval;//Ost = st*floor(fval/st) + (double)st/2;

                y = MyForm->Series2->XValues->Locate(Ost);

				if ( y == -1)
                        MyForm->Series2->AddXY(Ost,(double)1/(LnStCount),"",clTeeColor);
                else
                        MyForm->Series2->YValues->Value[y] += (double)1/(LnStCount);
            }
        }

        double RecLevel;

        RecLevel = (fMinVal + fMaxVal)/(double)2;

		MyForm->RecLevelEdit->Text = FloatToStrF(RecLevel, ffNumber , 8, 2);


		ScoreLearningData(RecLevel);
		ScoreControlData(RecLevel);
		*/
}
//---------------------------------------------------------------------------
//Выдает в MyForm распознан объект контроля или нет
void ObjProcessor::ScoreControlData(double score)
{

     /*
        double fval;
        int counter=0;
        double SecErr;

    if ( ErrCountNegSource == __ERRCOUNTNEG_CONTROL_ )
    {
        for (int i=0; i< NewSeqCount; i++)
        {
            if ( m_SEQStorage->NewStorage[i].GetClassId() < EC_FirstClass || m_SEQStorage->NewStorage[i].GetClassId() > EC_LastClass)
            {
                MyForm->NewSeqsList->Cells[6][i+1] = "-NotRecognized";
            }
            else if ( m_SEQStorage->NewStorage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->NewStorage[i].FVal;

                if ( fval >= score)
                {
                        counter++;
                        MyForm->NewSeqsList->Cells[6][i+1] = "+Recognized";
                }
                else
                {
                        MyForm->NewSeqsList->Cells[6][i+1] = "-NotRecognized";
                }
            }

        }
    }
    else
    {
        for (int i=0; i< NegSeqCount; i++)
        {
            if ( m_SEQStorage->LrnNegStorage[i].GetClassId() < EC_FirstClass || m_SEQStorage->LrnNegStorage[i].GetClassId() > EC_LastClass)
            {
                MyForm->NegSequencesList->Cells[6][i+1] = "-NotRecognized";
            }
            else
            {
				fval = m_SEQStorage->LrnNegStorage[i].FVal;

                if ( fval >= score)
                {
                        counter++;
                        MyForm->NegSequencesList->Cells[6][i+1] = "+Recognized";
                }
                else
                {
                        MyForm->NegSequencesList->Cells[6][i+1] = "-NotRecognized";
                }
            }

        }

     }
        SecErr = (double)counter/(double)NewSeqCount;
		MyForm->SecondLErr->Caption = FloatToStrF(SecErr, ffNumber , 8, 8);
		AnsiString s = "Recognized " + IntToStr(counter) + " out of " + IntToStr((int)NewSeqCount);
        MyForm->SecErrInW->Caption = s;
*/
}
//---------------------------------------------------------------------------
//Выдает в MyForm распознан объект обучения или нет
void ObjProcessor::ScoreLearningData(double score)
{
    /*
        double fval;
        int counter=0;
        int NCount=0;
        double FstErr;

        if ( ErrCountPosSource == __ERRCOUNTPOS_CONTROL_ )
		for (int i=0; i< CtrlPosSeqCount; i++)
        {
            if( m_SEQStorage->CtrlPosStorage[i].GetClassId() == nRecClass)
            {

                NCount++;
                fval = m_SEQStorage->CtrlPosStorage[i].FVal;

                if ( fval < score)
                {
                        counter++;
                        MyForm->PosCtrlSeqsList->Cells[6][i+1] = "-NotRecognized";
                }
				else
                {
                        MyForm->PosCtrlSeqsList->Cells[6][i+1] = "+Recognized";
                }
            }
            else if( m_SEQStorage->CtrlPosStorage[i].GetClassId() == -1 )
                MyForm->PosCtrlSeqsList->Cells[6][i+1] = "-NotRecognized";

        }
        else
        for (int i=0; i< SeqCount; i++)
        {
            if ( m_SEQStorage->Storage[i].GetClassId() == nRecClass)
            {
                fval = m_SEQStorage->Storage[i].FVal;

                NCount++;
                if ( fval < score)
                {
                        counter++;
                        MyForm->SequencesList->Cells[6][i+1] = "-NotRecognized";
                }
                else
                {
                        MyForm->SequencesList->Cells[6][i+1] = "+Recognized";
                }
            }
        }
        if ( NCount!= 0. ) FstErr = (double)counter/(double)NCount;
        else FstErr = 1.;
		MyForm->FirstLErrEd->Text = FloatToStrF(FstErr, ffNumber , 8, 8);
		BootStrCtrl->FstLevelErr->Text = FloatToStrF(FstErr, ffNumber , 8, 8);
		BootStrCtrl->dFstErr = FstErr;

        AnsiString s = "Recognized " + IntToStr(NCount - counter) + " out of " + IntToStr(NCount);
        MyForm->FstErrInW->Caption = s;
        */
}
//---------------------------------------------------------------------------
//Пересчитывает порог по заданной ошибке первого рода
void ObjProcessor::ReCount(double FstErr)
{
        int NCount=0;

        if ( ErrCountPosSource == __ERRCOUNTPOS_LEARNING_ )
        for (int i=0; i< SeqCount; i++)
		{
            if ( m_SEQStorage->getClass(i) == nRecClass)
            {

                NCount++;
            }
        }
        else
        for (int i=0; i< CtrlPosSeqCount; i++)
        {
            if ( m_SEQStorage->CtrlPosStorage[i].GetClassId() == nRecClass)
            {
                NCount++;
            }
        }



        if (NCount <= 1)
        {
                return;
        }

        LDScoreArray = new double[NCount];

        double fval;
        int counter=0;
        int j;

        if ( ErrCountPosSource == __ERRCOUNTPOS_LEARNING_ )
        for (int i=0; i< SeqCount; i++)
        {
			if ( m_SEQStorage->getClass(i) == nRecClass)
            {
                fval = 0;//m_SEQStorage->Storage[i].FVal;
                if ( counter == 0 )
                {
                    LDScoreArray[0] = fval;
                }
                else
                {
                    j=0;
                    do
                    {
                        if ( fval <= LDScoreArray[j])
						{
                            for( int k=counter; k>j; k--)
                            {
                                LDScoreArray[k] = LDScoreArray[k-1];
                            }
                            LDScoreArray[j] = fval;
                            j = counter + 1;

                        }
                        j++;
                    }while (j<counter);
                    if ( j == counter )
                    {
                        LDScoreArray[counter] = fval;
                    }

                }


                counter++;
            }
        }
        else
        for (int i=0; i< CtrlPosSeqCount; i++)
        {
            if ( m_SEQStorage->CtrlPosStorage[i].GetClassId() == nRecClass )
            {
				fval = 0;//m_SEQStorage->CtrlPosStorage[i].FVal;
                if ( counter == 0 )
                {
                    LDScoreArray[0] = fval;
                }
				else
                {
                    j=0;
                    do
                    {
                        if ( fval <= LDScoreArray[j])
                        {
                            for( int k=counter; k>j; k--)
                            {
                                LDScoreArray[k] = LDScoreArray[k-1];
                            }
                            LDScoreArray[j] = fval;
                            j = counter + 1;

                        }
                        j++;
                    }while (j<counter);
                    if ( j == counter )
                    {
                        LDScoreArray[counter] = fval;
                    }

                }


                counter++;
            }
        }


        double RecLevel=0.;

        if (FstErr == 0.0)
        {
                RecLevel = LDScoreArray[0] - 1.0;

        }
        else
        {
           double aleph, beta;

           if (NCount > 2)
           {
           for ( int i=0; i < NCount; i++)
           {
				aleph = (double)i/(double)NCount;
                beta =  (double)(i+1)/(double)NCount;
                if ( (aleph<= FstErr) && (beta>= FstErr))
                {
                    if ( (FstErr - aleph)<=(beta - FstErr))
                        RecLevel =  (LDScoreArray[i] + LDScoreArray[i+1])/2.0;
                    else
                        RecLevel =  (LDScoreArray[i+1] + LDScoreArray[i+2])/2.0;
                    break;
                }
           }
           }
           else
		   {
                RecLevel =  (LDScoreArray[0] + LDScoreArray[1])/2.0;
           }
        }
        char v[50];
		sprintf(v, "RecLevel= %f", RecLevel);
        //ReportLog(v);
        ScoreLearningData(RecLevel);
        ScoreControlData(RecLevel);

        delete[] LDScoreArray;

}
