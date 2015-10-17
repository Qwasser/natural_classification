//-------------------------------------------------------------
#ifndef DISCOV
#define DISCOV
//-------------------------------------------------------------
typedef unsigned long long QWORD, *PQWORD;
typedef unsigned int DWORD, *PDWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef void *PTR;
typedef char *CPTR;
//-------------------------------------------------------------
#pragma pack(1)
//-------------------------------------------------------------
#define bnotfound ((DWORD)-1)
//-------------------------------------------------------------
struct _SDEngine;
struct _HYPO;
struct _ATTR;
struct _PREDICATE;
//-------------------------------------------------------------
typedef struct _SDEngine SDEngine, *PSDEngine;
typedef struct _HYPO HYPO, *PHYPO;
typedef struct _ATTR ATTR, *PATTR;
typedef struct _PREDICATE PRED, *PPRED;
typedef struct _PREDICATE_INFO PRED_INFO, *PPRED_INFO;
//-------------------------------------------------------------
typedef void (*RegHandle) (PHYPO, PPRED const, DWORD, DWORD);
typedef double (*scale_f) (PTR o1);
typedef BYTE (*cmp_f) (PTR o1, PTR o2);
//-------------------------------------------------------------
struct _PREDICATE
{
	PTR addit_info;
	QWORD par_idx;
	QWORD scale_val;
};
//-------------------------------------------------------------
struct _PREDICATE_INFO
{
	double m00;
	double m01;
	double m10;
	double m11;
	double yule_value;
	double fisher_value;
};
//-------------------------------------------------------------
struct _ATTR
{
	char* name;
	DWORD atype;
	PTR borders;
	DWORD bnum;
	BYTE bsz;
	DWORD totalnum;
	WORD maskblock;
	BYTE maskbit;
	QWORD maskoffs;
	WORD bordersz;
	DWORD fullval;
	scale_f scalef;
	cmp_f cmpf;
	DWORD objoffset;
	DWORD is_addrbased;
	char free_to_use[58];
};
//-------------------------------------------------------------
struct _HYPO
{
	DWORD used_count;
	double last_prob;
	double fisher_lvl;
	double yule_lvl;
	DWORD yule_minfreq;
	
	PTR used_mask;
	PTR asked_mask;
	PTR asked_noconc;
	
	DWORD masksz;
	
	DWORD conc_addr;
	DWORD conc_par;
	DWORD conc_idx;
	DWORD conc_size;
	DWORD conc_val;
	
	QWORD* pars_c;
	QWORD* pars_nc;
	QWORD* init_dump;
	
	DWORD cur_par;
	DWORD cur_val;
	DWORD fulld;
	PTR cbck_arg;
	PTR thread_arg;
};
//-------------------------------------------------------------
struct _SDEngine
{
	DWORD objlen;
	QWORD objcnt;
	QWORD bitlen;
	
	ATTR* attrs;
	DWORD attr_cnt;
	
	QWORD* objects;
	
	DWORD* bitidx;
};
//-------------------------------------------------------------
extern "C" QWORD AdjustAttrOffsets(PTR attrs, DWORD anum);
extern "C" void AdjustBitIdxs(PTR attrs, DWORD anum, DWORD* indexes);
extern "C" void GetBinaryObjectDescriptor_s(void* objdescr, DWORD* objvalues, PTR attrs, DWORD anum);
extern "C" void genhypos(PTR hypos, PTR conclusions, DWORD conc_cnt, PTR attrs, DWORD anum, DWORD fulldepth);
extern "C" void genhypos_reverse(PTR hypos, PTR conclusions, DWORD conc_cnt, PTR attrs, DWORD anum, DWORD fulldepth);
extern "C" void bust(PTR objs, QWORD numobj,
                    PTR atrs, DWORD anum,
                    PTR hyp,
                    DWORD* bits,
                    QWORD* pars_nc, QWORD* pars_c,
                    PTR reg_cbck);

extern "C" void border_f(PTR attrs, DWORD attrc, PTR object, DWORD* buf);

extern "C" PSDEngine SDInitEngine(ATTR* atrs, DWORD atrc);
extern "C" void SDInitRawObjects(PSDEngine engine, PTR raw_objects, DWORD rawobj_sz, QWORD obj_cnt);
extern "C" void SDInitIntegerObjects(PSDEngine engine, PTR integer_objects, DWORD initobj_sz, QWORD obj_cnt);
extern "C" PHYPO SDFormHypo(PSDEngine engine, DWORD conclusion, DWORD val, DWORD fulldepth, double fisher);
extern "C" PHYPO SDFormHypoWithYule(PSDEngine engine, DWORD conclusion, DWORD val, DWORD fulldepth, double fisher, DWORD yule_minfreq, double yule_critval);
extern "C" void SDBust(PSDEngine engine, PHYPO hyp, RegHandle reg_here);
extern "C" void SDMakeScale(DWORD grad_num, ATTR* atrs, PTR objects, DWORD object_sz, QWORD obj_cnt);
extern "C" void SDMakeScales(DWORD grad_num, ATTR* atrs, DWORD atr_num, PTR objects, DWORD object_sz, QWORD obj_cnt);
extern "C" void SDMakeDiffGradScales(ATTR* atrs, DWORD atr_num, PTR objects, DWORD object_sz, QWORD obj_cnt);
extern "C" PATTR SDInitAttrs(DWORD atrc);
extern "C" void SDCloseEngine(PSDEngine engine);
extern "C" void SDCloseHypo(PHYPO hyp);
extern "C" void SDCloseHypos(PHYPO hyp, QWORD hcnt);
extern "C" PHYPO SDPenetratedOneD(PSDEngine engine, PPRED conclusions, DWORD conc_cnt, DWORD full_depth, double fisher, DWORD* hyp_cnt);
extern "C" PHYPO SDPenetratedYuleOneD(PSDEngine engine, PPRED conclusions, DWORD conc_cnt, DWORD full_depth, double fisher, DWORD* hyp_cnt, DWORD yule_freq, double yule_critlvl);
extern "C" PHYPO SDBackwardHypos(PSDEngine engine, PPRED conclusions, DWORD conc_cnt, DWORD target_par, DWORD full_depth, double fisher, DWORD* hyp_cnt, DWORD yule_freq, double yule_critlvl);
//-------------------------------------------------------------
extern "C" void SDSaveAttr(PATTR atr, PTR fout);
extern "C" void SDLoadAttr(PATTR atr, PTR fin);
extern "C" void SDSaveEngine(PSDEngine engine, PTR fout);
extern "C" PSDEngine SDLoadEngine(PTR fin);
//-------------------------------------------------------------
extern "C" double scale_int(void* o);
extern "C" double scale_dword(void* o);
extern "C" double scale_short(void* o);
extern "C" double scale_lagre(void* o);
extern "C" double scale_char(void* o);
extern "C" double scale_double(void* o);
extern "C" double scale_bool(void* o);

extern "C" BYTE cmp_double(PTR pd1, PTR pd2);
extern "C" BYTE cmp_long(PTR pl1, PTR pl2);
extern "C" BYTE cmp_str(PTR ps1, PTR ps2);
extern "C" BYTE cmp_int(PTR pi1, PTR pi2);
extern "C" BYTE cmp_short(PTR ps1, PTR ps2);
extern "C" BYTE cmp_large(PTR pl1, PTR pl2);
extern "C" BYTE cmp_char(PTR pc1, PTR pc2);
extern "C" BYTE cmp_bool(PTR pb1, PTR pb2);
//-------------------------------------------------------------
#endif
