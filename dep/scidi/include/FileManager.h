//---------------------------------------------------------------------------
#ifndef FileManagerH
#define FileManagerH
//---------------------------------------------------------------------------
#include "Sequence.h"
#include "Rule.h"
#include "RulesStorage.h"

#include "../setup.h"

namespace FileManager
{
	int CountLines(const char * file);
	SCIDI_API SEQStorage* ReadCSVData(const char* name);
	SCIDI_API int ReadPlainTextRules(const char* file, SEQStorage* data, RulesStorage* rules);
//	SCIDI_API void WriteXMLRules(Rule* set, unsigned long length, const char* name);
}
//---------------------------------------------------------------------------
#include <fstream>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>     /* assert */

#ifdef WIN32
	typedef __int64 int64_t;
#else
	#include <stdint.h> // int64_t
#endif

typedef FILE			*bstream;
typedef void			*PTR;
typedef int64_t			bigint;
typedef unsigned int	UINT;
typedef std::pair<PTR, UINT> pointer_data;

class BinaryIO
{
	
	
public:
	BinaryIO(const char* FileName, const char* Mode){ 
		FileHandler = fopen(FileName, Mode);
		if (FileHandler == NULL) assert("can't load file to binary i/o");
	}
	BinaryIO(bstream Handler){ 
		FileHandler = Handler;
		if (FileHandler == NULL) assert("can't load binary stream");
	}
	~BinaryIO()	{ if (FileHandler != NULL) fclose(FileHandler); }
	//save (output) from program data to file
	BinaryIO& operator<< (int x){ WriteInt(FileHandler, x); return *this; }
	BinaryIO& operator<< (char* x){ WriteStr(FileHandler, x); return *this; }
	BinaryIO& operator<< (UINT x){ WriteUINT(FileHandler, x); return *this; }
	BinaryIO& operator<< (double x){ WriteDouble(FileHandler, x); return *this; }
	BinaryIO& operator<< (long x){ WriteLong(FileHandler, x); return *this; }
	BinaryIO& operator<< (unsigned long x){ fwrite(&x, sizeof(unsigned long), 1, FileHandler); return *this; }
	BinaryIO& operator<< (pointer_data x){ WriteVoid(FileHandler, x.first, x.second); return *this; }
	
	//load (output) from program data to file
	BinaryIO& operator>> (int& x){ x = ReadInt(FileHandler); return *this; }
	BinaryIO& operator>> (char* x){ x = ReadStr(FileHandler); return *this; }
	BinaryIO& operator>> (UINT& x){ x = ReadUINT(FileHandler); return *this; }
	BinaryIO& operator>> (double& x){ x = ReadDouble(FileHandler); return *this; }
	BinaryIO& operator>> (long& x){ x = ReadLong(FileHandler); return *this; }
	BinaryIO& operator>> (unsigned long& x){ fread(&x, sizeof(unsigned long), 1, FileHandler); return *this; }
	BinaryIO& operator>> (pointer_data& x){ ReadVoid(FileHandler, x.first, x.second); return *this; }
	
private:
	//binary read/write functions
	int ReadInt(FILE* fl)
	{
		int buf;
		fread(&buf, sizeof(int), 1, fl);
		return buf;
	}
	
	long ReadLong(FILE* fl)
	{
		long buf;
		fread(&buf, sizeof(long), 1, fl);
		return buf;
	}
	
	UINT ReadUINT(FILE* fl)
	{
		UINT buf;
		fread(&buf, sizeof(UINT), 1, fl);
		return buf;
	}
	
	char* ReadStr(FILE* fl)
	{
		int bufsz = ReadInt(fl);
		char* buf = (char*)malloc(bufsz+1);
		buf[bufsz] = 0;
		fread(buf, bufsz, 1, fl);

		return buf;
	}
	
	double ReadDouble(FILE* fl)
	{
		double buf;
		fread(&buf, sizeof(double), 1, fl);
		return buf;
	}
	
	void ReadVoid(FILE* fl, PTR buf, UINT sz)
	{
		fread(buf, sz, 1, fl);
	}
	
	int WriteInt(FILE* fl, int val) {
		return fwrite(&val, sizeof(int), 1, fl);
	}
	
	int WriteStr(FILE* fl, char* val)	{
		if (WriteInt(fl, strlen(val))==0)
			return 0;
		return fwrite((PTR)val, strlen(val), 1, fl);
	}
	
	int WriteUINT(FILE* fl, UINT val) {
		return fwrite(&val, sizeof(UINT), 1, fl);
	}
	
	int WriteDouble(FILE* fl, double val)	{
		return fwrite(&val, sizeof(double), 1, fl);
	}
	
	int WriteLong(FILE* fl, long val)	{
		return fwrite(&val, sizeof(long), 1, fl);
	}
	
	int WriteVoid(FILE* fl, PTR val, UINT sz)	{
		return fwrite(val, sz, 1, fl);
	}
	//---------------------------
	FILE* FileHandler;
};

#endif
