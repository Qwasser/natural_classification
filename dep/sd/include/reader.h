#ifndef reader
#define reader
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <stdio.h>
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef unsigned long long QWORD;
typedef unsigned int DWORD;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ReadInt(FILE* fl);
bool ReadBool(FILE* fl);
long ReadLong(FILE* fl);
DWORD ReadDWORD(FILE* fl);
QWORD ReadQWORD(FILE* fl);
char* ReadStr(FILE* fl);
double ReadDouble(FILE* fl);
bool WriteDWORD(FILE* fl, DWORD val);
bool WriteLong(FILE* fl, long val);
bool WriteVoid(FILE* fl, PTR val, DWORD sz);
bool WriteQWORD(FILE* fl, QWORD val);
bool WriteDouble(FILE* fl, double val);
bool WriteStr(FILE* fl, char* val);
bool WriteInt(FILE* fl, int val);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif