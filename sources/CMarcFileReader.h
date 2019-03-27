#ifndef CMARCHFILEREADER_H
#define CMARCHFILEREADER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class CFlag
{
public:
	long FPointer;
	long Length;
};

class CRecordReader
{
public:
   CRecordReader(char *sPath);
   CRecordReader(int fd = -1);
   virtual ~CRecordReader();
   
   virtual int GetRecord(unsigned char **buffer) = 0;
   int SkipRecord(void);
   int GetPosition(void)
      { return filePos; }
   int GetRecordStart(void)
      { return recStart; }
   int GetRecordLength(void)
      { return recEnd-recStart; }
      
   int IsError(void)
      { return errState; }
   string StrError(void)
      { return errString; }
   
protected:
   int ReadByte(void);
   long GetBytes(unsigned char *buffer, long len);
   int UnGetBytes(long len);
   
   void MarkRecordStart(void)
      { recStart = filePos; }
   void MarkRecordEnd(void)
      { recEnd = filePos; }

   string errString;
   int errState;

private:
   int fileDesc;

   unsigned char *readBuffer;
   unsigned char *readBufferPtr;
   long readPos;
   long readLen;
   long filePos;
   
   long recStart;
   long recEnd;
};

class CIsoRecordReader : public CRecordReader
{
public:
   CIsoRecordReader(char *sPath) : CRecordReader(sPath) {};

   virtual int GetRecord(unsigned char **buffer);
};

class CTextRecordReader : public CRecordReader
{
public:
   CTextRecordReader(char *sPath) : CRecordReader(sPath) {};

   virtual int GetRecord(unsigned char **buffer);
};

class CAlephRecordReader : public CRecordReader
{
public:
   CAlephRecordReader(char *sPath) : CRecordReader(sPath) {};

   virtual int GetRecord(unsigned char **buffer);
};

class CMarcFileReader
{
public:
    int OpenFile(char * sPath, int format);
	int GetCountRecords();
    char* GetRecord(long Index);
	int CloseFile();

    string g_Error;
    ~CMarcFileReader()
    {
       
    }
private:
	FILE *fr; 
	vector<CFlag*> CollFlags;
};

#endif

