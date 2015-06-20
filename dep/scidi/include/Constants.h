
//---------------------------------------------------------------------------

#ifndef ConstantsH
#define ConstantsH
//---------------------------------------------------------------------------

const int RULE_NO_DATA = 0;
const int RULE_NO_DATA_FISHER = 1;
const int RULE_NEG_CORRELATION = 2;//Negative correlation
const int RULE_INDEPENDENT_EXPERTION = 3;
const int RULE_LOW_COND_PROBABILITY = 4;
const int RULE_MINIMIZED = 5;
const int RULE_ADD = 6;
const int RULE_DUBLICATE = 7;
const int RULE_LOW_LEVEL_PROBABILITY = 8;
const int RULE_HAS_CP = 9;

const int RETURN_ERROR = -1;
const int RETURN_EMPTY = 0;
const int RETURN_SUCCESS = 1;
// Типы выравнивания нуклеотидных последовательностей
const int AT_FIXED_POSITIONS = 0;// Фиксированые позиции
const int AT_SHIFT_POSITIONS = 1;// Смещенные позиции

const int DECLINE_GROW = 0;
const int ACCEPT_GROW = 1;

const int JUST_POSITIVE = 0;
const int DECLINE_TARGET = 1;
const int DECLINE_PREMIS = 2;
const int DECLINE_PREMIS_TARGET = 3;

const unsigned char READSEQ_MODE = 2; // Режим файлового менеджера для считывания из файла последовательнностей

const unsigned long STACK_SIZE = 4096000;

const bool CASE_SANSITIVE = false; // Различать большие и маленькие буквы при кодировании последовательностей

const long SEQ_MAX_LENGTH = 1024; // максимальная длина загружаемых последовательностей

const char _NEWPRJNAME[] 	= "Project";
const char _PRJEXT[]		= ".sdp";


const int __IDEALIZTYPE_DELETEFIRST_ = 0;
const int __IDEALIZTYPE_INSERTFIRST_ = 1;
const int __IDEALIZTYPE_DELETEINSERT_ = 2;
const int __IDEALIZTYPE_OFFICIAL_ = 3;

const int __RULE_NOTAPPLICABLE_ONID_ = 0;
const int __RULE_ISCONFIRMED_ONID_ = 1;
const int __RULE_ISDISPROVED_ONID_ = 2;

const int __IDEALSOURCE_ORIGINOBJS_ = 0;
const int __IDEALSOURCE_REGULARITIES_ = 1;

const int __ERRCOUNTPOS_LEARNING_ = 0;
const int __ERRCOUNTPOS_CONTROL_ = 1;
const int __ERRCOUNTNEG_LEARNING_ = 0;
const int __ERRCOUNTNEG_CONTROL_ = 1;
//long __DEBUG_IO_COUNTER;

	const int __IDMSG_TASKCOMPLETE_ = -1;
	const int __IDMSG_NOSEQFORIDEALIZ_ = 0;
	const int __IDMSG_NORULESFORIDEALIZ_ = 1;
	const int __IDMSG_THREADSTARTERROR_ = 2;
	const int __IDMSG_PROJECTNOTSAVING_ = 3;
	const int __IDMSG_PROJECTISNEW_ = 4;
	const int __IDMSG_SAVEPROJECTERROR_ = 5;
	const int __IDMSG_REGULARITIESSOURCE_ = 6;
	const int __IDMSG_NOSEQFORCLASSIFICATION_ = 7;
	const int __IDMSG_NOOBJSFORCLASSIFICATION_ = 8;
	const int __IDMSG_NORULESFORCLASSIFICATION_ = 9;
	const int __IDMSG_NOSEQSFORBOOTSTRAP_ = 10;
	const int __IDMSG_PROGRAMMERROR_ = -100;

#endif
