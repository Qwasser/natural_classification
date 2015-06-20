//---------------------------------------------------------------------------
#include "RulesStorage.h"
#include "FileManager.h"
#include <algorithm>
//---------------------------------------------------------------------------

RulesStorage::RulesStorage()
	:sz_Storage(0), tree_id(-1)
{
	ppred = 0;
	CBBTree = 0;
}

RulesStorage::RulesStorage(unsigned long n, unsigned long alf)
	:sz_Storage(0), tree_id(-1)
{
	Charset = alf;
	Attrset = n;
	ppred = CodeTable::Instance()->InitPredicates(n, alf);
	CBBTree = new OneTargetTree[ 2*alf*n ];
}


RulesStorage::RulesStorage(FILE* arh)
	:tree_id(-1)
{
	BinaryIO bin(arh);
	
	//Attrset = ReadLong(arh);
	//Charset = ReadLong(arh);
	//sz_Storage = ReadLong(arh);
	bin >> Attrset >> Charset >> sz_Storage;
	pointer_data tree = std::make_pair((void *)CBBTree, (unsigned int)(sizeof(OneTargetTree)*(2*Charset*Attrset)));
	bin >> tree;
	ppred = CodeTable::Instance()->InitPredicates(Attrset, Charset);
	CBBTree = new OneTargetTree[ 2*Charset*Attrset ];
	//ReadVoid(arh, CBBTree, 2*Charset*Attrset*sizeof(OneTargetTree));
	//UINT size = ReadUINT(arh);//m_blocks.size
	UINT size;
	bin >> size;
	RuleChains *rules_array = new RuleChains[size];
	//ReadVoid(arh, rules_array, size*sizeof(RuleChains));
	pointer_data rules = std::make_pair((void *)rules_array, (unsigned int)(size*sizeof(RuleChains)));
	bin >> rules;

	std::list<RuleChains*>::allocator_type pl = m_blocks.get_allocator();
	RuleChains** ref = pl.allocate(size);
	for (UINT i = 0; i < size; ++i) 
		ref[i] = &(rules_array[i]);
		
	m_blocks.assign(
	    blocks(ref, sizeof(RuleChains)),
	    blocks(ref + size)
	);
	
	MakePointersArray();
}

void RulesStorage::Load(Rule** Ref, long Size)
{
	assert("call RulesStorage::Load undefined");
//	sz_Storage += Size;
//	Storage.insert(Storage.end(), blocks(Ref[0],sizeof(Rule)), blocks( (Rule*)((UINT)Ref[Size-1]+sizeof(Rule)) ));
}

void RulesStorage::Save(FILE* arh)
{
	/*
	WriteLong(arh, Attrset);
	WriteLong(arh, Charset);
	WriteLong(arh, sz_Storage);

	WriteVoid(arh, CBBTree, sizeof(OneTargetTree)*(2*Charset*Attrset));
	WriteUINT(arh, m_blocks.size());
	for(blocksID pr = m_blocks.begin(); pr!=m_blocks.end(); pr++) 
	{
		WriteVoid(arh, (*pr), sizeof(RuleChains));
	}
	*/
	BinaryIO bin(arh);
	bin << Attrset << Charset << sz_Storage;
	bin << std::make_pair(CBBTree, sizeof(OneTargetTree)*(2*Charset*Attrset));
	bin << m_blocks.size();
	for(blocksID pr = m_blocks.begin(); pr!=m_blocks.end(); pr++) 
	{
		bin << std::make_pair((*pr), sizeof(RuleChains));
	}
}

Rule* RulesStorage::ConvertFromLinkToRule(RuleLink* R)
{
	Rule* t_rule = new Rule(R->getRuleLength());
	for(int l = 0; l < R->getRuleLength(); l++)
		t_rule->AddSection(GetPredicatePointer(R->Chain(l).Shift, R->Chain(l).Sign, R->Chain(l).Value));

	t_rule->setTTPos(R->getTTPos());
	t_rule->setTTSign(R->getTTSign());
	t_rule->setTTValue(R->getTTValue());
	t_rule->setCP(R->getCP());

	return t_rule;
}

Rule* RulesStorage::c_array()
{
	Rule* r_array = new Rule[getSize()];
	unsigned long i = 0;
	for(ruleID p = Storage.begin(); p != Storage.end(); p++)
	{
		Rule* t = ConvertFromLinkToRule(&(*p));
		r_array[i++] = *t;
	}

	return r_array;
}

RuleSection* RulesStorage::GetPredicatePointer(int Shift, int Sign, int SendToken)
{
	auto target_id = GetPredicatePos(Shift, Sign, SendToken);
	return &ppred[target_id];
}

UINT RulesStorage::GetPredicatePos(int Shift, int Sign, int SendToken)
{
	return (2*Shift + ((1-Sign)/2))*Charset + SendToken;
}
//---------------------------------------------------------------------------

RulesStorage::~RulesStorage()
{
	Storage.clear();
	//Sort.clear();

	delete[] CBBTree;
}

// add hash of rule presented as string to hash array
void RulesStorage::AddRuleHash(Rule* R)
{
	// reserve memory for new expected hashes
	if ( sz_Storage%ONE_STEP_SIZE == 0 )
		hash_array.reserve(sz_Storage + ONE_STEP_SIZE);
	// create hash from string notion of rule
	char ForCompChainString[MYMAXSTR];
	ForCompChainString[0] = '\0';
	R->getChainStr(ForCompChainString);
	std::size_t sh = string_hash(ForCompChainString);
	// add to hash array
	hash_array.push_back(sh);
}

//----------------------------------------------------------------------
//добавляет в хранилище правило,
//подошедшее по доверительному интервалу и уровню усл вероятности.
//при этом хранилище никак не упорядочено
//sz_Storage - число правил на текущий момент
bool RulesStorage::Add(Rule* R)
{ 
	if (R->getRuleLength() == 0  || isFind(R))
		return(false);
	
	AddRuleHash(R);
	//asynchron start write rule R to disk
	
	AddCorrectRule(R); 
	sz_Storage++;
	return(true);
}

void RulesStorage::AddCorrectRule(Rule* ptr_Rule)
{
	long Target_p = ptr_Rule->getTTPos();
	char Target_s = ptr_Rule->getTTSign();
	int Target_v = ptr_Rule->getTTValue();
	UINT pChain;
	pChain = AddTail( ptr_Rule );
	if( tree_id != GetPredicatePos(Target_p, Target_s, Target_v) )
	{
		tree_id = GetPredicatePos(Target_p, Target_s, Target_v);
		AddHead( tree_id, pChain );
	}
	UpdateHead( tree_id );
}
//---------------------------------------------------------------------------
std::vector<UINT> RulesStorage::PackTail(Rule* ptr_Rule)
{
	UINT l = ptr_Rule->getRuleLength();
	std::vector<UINT> tail(l);
	for (auto i = 0; i < l; i++)
	{
		Predicate p = ptr_Rule->Chain(i);
		tail[i] = GetPredicatePos(p.Shift, p.Sign, p.Value);
	}
	return tail;
}
//---------------------------------------------------------------------------
UINT RulesStorage::AddTail(Rule* ptr_Rule)
{
	if( ptr_Rule->getRuleLength() == 0 )
		return 0;

	MiniRule uRule = {
		ptr_Rule->getRuleLength(),
		ptr_Rule->prob_numerator,
		ptr_Rule->prob_denominator
	};
	uRule.Chain = PackTail(ptr_Rule);

	if( sz_Storage == 0 )
	{
		RuleChains* chains = new RuleChains();
		m_blocks.push_front( chains );
	}
	UINT pointer = m_blocks.front()->GetLastChain();
	int Res = m_blocks.front()->Store(&uRule);
	if( Res >= 0 )
	{
		RuleChains* chains = new RuleChains();
		m_blocks.push_front( chains );
		//Каждый блок начинаем с массива. оставляем оборванным
		//Обновляем указатель - он не будет указывать на 
		pointer = m_blocks.front()->GetLastChain();
		m_blocks.front()->Store(&uRule);
	}
	
	return (m_blocks.size() - 1) * BlocksSize() + pointer;
}
//---------------------------------------------------------------------------
void RulesStorage::AddHead(UINT Id, unsigned long pChain)
{
	OneTargetTree* tree = SetTargetInTree( Id, Id );
	tree->ConnectChain( pChain );
}
//---------------------------------------------------------------------------
void RulesStorage::MakePointersArray()
{
	Storage.resize( sz_Storage, RuleLink() );
	unsigned long j=0;
	RuleSection* temp;
	UINT* address;
	UINT bid=0;
	unsigned long chain_shift=0, total_shift=0;
	ruleID i=Storage.begin();
	
	std::list<RuleChains*>::reverse_iterator iblocks = m_blocks.rbegin();
	//FILE* tt = fopen("cbb.txt","w");
	//for(UINT j=0; j <2*Charset*Attrset; j++ )
	//{
	//	fprintf(tt, "%i\t%i\t%i\n", CBBTree[j].GetTarget(), CBBTree[j].GetVolume(), CBBTree[j].GetChains());
	//}
	//fclose(tt);
	for(UINT j=0; j <2*Charset*Attrset; j++ )
	{
		temp = GetTargetByShift( CBBTree[j].GetTarget() );//conclusion predicate
		total_shift = CBBTree[j].GetChains();//индекс в блоке стартовой позиции
		//если для данного заключения ничего нет, то не надо ничего лишнего
		if( CBBTree[j].GetVolume() > 0 )
		{
			//1.Определим с какого блока нужно считывать
			iblocks = m_blocks.rbegin();
			if( total_shift >= BlocksSize() )//totalshift выходит за размер блока, 
				//значит будем сдвигать
				//для каждой цели нужно начинать с конца и двигаться в начало (инверт)
			{
				bid = total_shift / BlocksSize();
				while( bid > 0 )
				{
					iblocks++;
					bid--;
				}
				total_shift = total_shift % BlocksSize();
			}
			//2.Правильный блок дает необходимый адрес
			//start address for current block to target
			address = &((*iblocks)->GetChains()[total_shift]);
			//all chains for one target
			for(UINT k=0; k<CBBTree[j].GetVolume(); k++)
			{
				//проверяем на конец блока
				//загружаем абсолютный сдвиг
				//получаем относительный
				chain_shift = (*iblocks)->Load( total_shift );
				total_shift += chain_shift;
				if( chain_shift==0 )
				{
					iblocks++;//следующий блок
					address = &((*iblocks)->GetChains()[0]);
					//вычисляем длину считываемой последовательности
					//как предикатов, так и сопутствующей длине и вероятности
					//т.е. здесь заложен весь сдвиг для нового адреса
					chain_shift = (*iblocks)->Load( 0 );
					total_shift = chain_shift;
				}
				i->setTarget( temp );
				i->setChain( address );
				i++;
				address = &((*iblocks)->GetChains()[total_shift]);
			}
		}
	}
	currentRule = begin();
	currentRule_id = 0;
}

//---------------------------------------------------------------------------
void RulesStorage::Clear()
{
	Storage.clear();
	sz_Storage = 0;
}
//---------------------------------------------------------------------------
void RulesStorage::DelSubRules()
{
	sz_Storage = DelSubRules(Storage, sz_Storage);
}

bool RulesStorage::isFind(Rule* ptr_Rule)
{
	char ForCompChainString[1024];
	ForCompChainString[0] = '\0';

	ptr_Rule->getChainStr(ForCompChainString);
	std::size_t sh = string_hash(ForCompChainString);

	if( std::find(hash_array.begin(), hash_array.end(), sh) == hash_array.end() )
		return false;

	return(true);
}

RuleLink& RulesStorage::operator[](UINT lStoreIndex)
{
	if( !Storage.empty() )
	{
		if( lStoreIndex > TABLE_LIMIT )
			return Storage.front();
		else
		{	
			if( currentRule_id > lStoreIndex )
			{
				currentRule = begin();
				currentRule_id = 0;
			}
			while( currentRule_id < lStoreIndex )
			{
				currentRule_id++;
				currentRule++;
			}
			if( currentRule_id == lStoreIndex )
				return (*currentRule);
		}
	}
}
//---------------------------------------------------------------------------
int RulesStorage::setRuleClass( long Index, long nClass)
{
	//need a solution
	//Storage[Index]->nClassNumber = nClass;

	return(1);
}
//---------------------------------------------------------------------------
//r2 - sub (low length)
bool RulesStorage::DefSubRules(RuleLink* r1, RuleLink* r2)
{
	//одно ли СВВ дерево?
	if( r1->getTTPos()!=r2->getTTPos() || r1->getTTSign()!=r2->getTTSign() || r1->getTTValue()!=r2->getTTValue() )
		return false;

	int lenDiff = r1->getRuleLength() - r2->getRuleLength();
	int rLen = r1->getRuleLength();
	int subLen = r2->getRuleLength();

	//не утверждает ли длина обратное
	if(lenDiff <= 0)
		return false;

	//определим какая переменная максимальная содержится в наших правилах
	//так как правила упорядочены по возрастанию переменных (Shift)
	//то заглянем в поледние предикаты и сравним
	int lSeqLength = std::max( (*r1)[rLen-1].Shift, (*r2)[subLen-1].Shift );
	
	int i1=0, i2=0;
	int coin=0, diff=0;//совпадения и различия
	//цикл, в котором мы переберем все посылки
	for(int l=0; l<lSeqLength; l++)//переменные по порядку
	{
		// 1.в правиле найдется такая переменная не позже, чем в подправиле
		// i1!=rLen - условие, что посылка еще не вся перебрана
		if(i1!=rLen && (*r1)[i1].Shift == l)//в правиле нашли переменную
		{
			if(i2!=subLen && (*r2)[i2].Shift == l)//в проверяемом подправиле есть такая переменная
			{

				//тогда сверяем значения и знаки
				if((*r1)[i1].Value == (*r2)[i2].Value && (*r1)[i1].Sign == (*r2)[i2].Sign)
					coin++;//совпали
				else
					diff++;//нет
				//предикат в подправиле так или иначе рассмотрен
				//указываем на следующий
				i2++;
			}
			//либо в проверяемом подправиле нет такой переменной
			else diff++;
			//аналогично: предикат в правиле рассмотрен
			//указываем на следующий
			i1++;
		}
		// 2.в подправиле все-таки переменная возникла раньше
		else if(i2!=subLen && (*r2)[i2].Shift == l)
		{
			//равенство исключается в силу предыдущего случая (1.)
			//значит увеличиваем разницу
			diff++;
			//и в подправиле указываем на след. предикат посылки
			i2++;//т.к. нет пересечения с (1.), то работает только один инкремент
		}
		//если разница в длинах меньше, чем найдено различий, то можно обрывать перебор
		if(lenDiff < diff)
			return false;
	}
	//видимо найдено подправило!
	return true;
}

long RulesStorage::DelSubRules(std::list<RuleLink>& Array, int size)
{
	ruleID p1_Rule, p2_Rule;
	Array.resize(size);
	p1_Rule=Array.begin();
	bool NewBegin = false;
	int n1=0, n2=0;
	//нужно правило p1 сравнить со всеми законами из того же дерева
	while( p1_Rule!=Array.end() )
	{
		NewBegin = false;
		//со следующими по списку, т.к. множество изначально упорядочено
		p2_Rule=p1_Rule;
		//сле-е-е-едующим :)
		p2_Rule++;
		n2=n1;
		
		while( p2_Rule!=Array.end() )
		{
			n2++;
			//просто дерево для одного атома кончилось
			if( (*p1_Rule).getTTSign()!=(*p2_Rule).getTTSign() || 
				(*p1_Rule).getTTValue()!=(*p2_Rule).getTTValue() || 
				(*p1_Rule).getTTPos()!=(*p2_Rule).getTTPos() )
			{
				p1_Rule++;
				NewBegin = true;
				break;
			}
			//p1 - оказался подправилом - удалим его и будем сравнивать следующий
			if( DefSubRules(&(*p2_Rule), &(*p1_Rule)) )
			{
				p1_Rule = Array.erase(p1_Rule);
				NewBegin = true;
				break;
			}
			//для p1 нашли подправило - продолжим поиск
			// =>по p1 вылета не было, значит p2 либо получит новое значение
			// или сами прибавим
			if( DefSubRules(&(*p1_Rule), &(*p2_Rule)) )
			{
				p2_Rule = Array.erase(p2_Rule);
			}
			else
				p2_Rule++;
		}
		n1++;
		if(!NewBegin)
			p1_Rule++;
		
	}

	return Array.size();
}

void RulesStorage::CalcAgain(SEQStorage* Data, double ConfInterval, double ConfInterval2)
{
	ruleID p_Rule = Storage.begin();
	Rule* p_rule;
	while( p_Rule!=Storage.end() )
	{
		p_rule = ConvertFromLinkToRule(&(*p_Rule));
		if( p_rule->Criteria(Data, ConfInterval, ConfInterval2) == DECLINE_GROW || 
			p_rule->Minimize(Data, ConfInterval, ConfInterval2) == true )
				p_Rule = Storage.erase(p_Rule);
		else
				p_Rule++;
	}
	sz_Storage = Storage.size();
}
