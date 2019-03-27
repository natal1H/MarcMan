#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef WIN32
#include <unistd.h>
#define O_BINARY        0
#else
#include <io.h>
#endif

#include "CMarcFileReader.h"

#define LOAD_ISO_FORMAT   1
#define LOAD_TEXT_FORMAT  2
#define LOAD_ALEPH_FORMAT 3
#define NEW_ISO_FORMAT    4
#define NEW_TEXT_FORMAT   5

#define SOURCE_DATAFILE   1
#define SOURCE_TEMPFILE   2

#define BUFFER_SIZE     8192
#define BUFFER_BORDER   32
#define STRING_INC      512
#define ALEPH_ID_LEN    9

CRecordReader::CRecordReader(char *sPath)
{
   fileDesc = open(sPath, O_RDONLY|O_BINARY);
   if (fileDesc==-1)
   {
      errString = "Cannot open file ";
      errString += sPath;
      errString += ": ";
      errString += strerror(errno);
      
      errState = 1;
   }
   else
      errState = 0;

   readPos = 0;
   readLen = 0;
   filePos = 0;
   readBuffer = new unsigned char [BUFFER_SIZE];
   readBufferPtr = readBuffer;
}

CRecordReader::CRecordReader(int fd)
{
   fileDesc = fd;

   readPos = 0;
   readLen = 0;
   filePos = 0;
   readBuffer = new unsigned char [BUFFER_SIZE];
   readBufferPtr = readBuffer;
   
   errState = 0;
}

CRecordReader::~CRecordReader()
{
   if (fileDesc!=-1)
      close(fileDesc);

   delete[] readBuffer;
}

int CRecordReader::SkipRecord(void)
{
   int retVal;
   unsigned char *buffer = NULL;
   
   retVal = GetRecord(&buffer);
   if (buffer!=NULL)
      delete[] buffer;

   return retVal;
}

int CRecordReader::ReadByte(void)
{
   int val;
    
   if (readPos>=readLen)
   {
      if (readLen>=BUFFER_BORDER)
      {
         memmove(readBuffer, readBuffer+readPos-BUFFER_BORDER, BUFFER_BORDER);
         do {
            readLen = read(fileDesc, readBuffer+BUFFER_BORDER,
                           BUFFER_SIZE-BUFFER_BORDER);
         } while (readLen==-1 && errno==EINTR);
         if (readLen>0) readLen += BUFFER_BORDER;
       
         readPos = BUFFER_BORDER;
      }
      else
      {
         do {
            readLen = read(fileDesc, readBuffer, BUFFER_SIZE);
         } while (readLen==-1 && errno==EINTR);
         readPos = 0;
      }
       
      if (readLen==-1) // read error
      {
         errString = "Read error: ";
         errString += strerror(errno);
         errState = 1;
         
         return -2; 
      }
      if (readLen==0) return -1; // end-of-line
       
      readBufferPtr = readBuffer + readPos;
   }
    
   val = *readBufferPtr;
   readBufferPtr++;
   readPos++;
   filePos++;
    
   return val;
}

long CRecordReader::GetBytes(unsigned char *buffer, long len)
{
   long pos;
   int val;
   unsigned char *ptr;
    
   ptr = buffer;
   for (pos=0; pos<len; pos++)
   {
      val = ReadByte();
      if (val==-1) break;
      if (val==-2) return -1;
   
      *ptr = val; ptr++;
   }
    
   return pos;
}

int CRecordReader::UnGetBytes(long len)
{
   if (len>readPos)
   {
      errString = "Internal error: UnGetBytes";
      errState = 1;
      return -1;
   }
   readPos -= len;
   readBufferPtr -= len;
   filePos -= len;
    
   return 0;
}

int CIsoRecordReader::GetRecord(unsigned char **buffer)
{
   unsigned char data[6];
   long retVal, len;

   MarkRecordStart();
   retVal = GetBytes(data, 5);
   if (retVal<=0) return retVal;
   if (retVal<5)
   {
      errString = "Invalid format of iso records";
      errState = 1;
      return -1;
   }
    
   data[5] = 0;
   len = atoi((char*)data);
   if (len<=5)
   {
      errString = "Invalid format of iso records";
      errState = 1;
      return -1;
   }
    
   *buffer = new unsigned char[len+1];
   if (*buffer==NULL)
   {
      errString = "Out of memory";
      errState = 1;
      return -1;
   }
    
   memcpy(*buffer, data, 5);
   len -= 5;
    
   retVal = GetBytes(*buffer+5, len);
   if (retVal==-1)
      return -1;
   if (retVal<len)
   {
      errString = "Invalid format of iso records";
      errState = 1;
      return -1;
   }

   (*buffer)[retVal] = 0;
   MarkRecordEnd();
   
   return retVal;
}

int CTextRecordReader::GetRecord(unsigned char **buffer)
{
   unsigned char *data, *ptr, *tmp;
   long dataLen, dataPos;
   long retVal, ct;
    
   data = new unsigned char[STRING_INC];
   if (data==NULL)
   {
      errString = "Out of memory";
      errState = 1;
      return -1;
   }
   dataLen = STRING_INC;

   retVal = GetBytes(data, 3);
   if (retVal<=0)
   {
      delete[] data;
      return retVal;
   }
   if (data[0]=='@' && data[1]=='\n')
      UnGetBytes(1);
   else if (!(data[0]=='@' && data[1]=='\r' && data[2]=='\n'))
   {
      delete[] data;
      errString = "Invalid format of text records";
      errState = 1;
      return -1;
   }
   
   MarkRecordStart(); 
   ptr = data; dataPos = 0;
   for (;;)
   {
      if (dataPos+BUFFER_BORDER+1>dataLen)
      {
         tmp = new unsigned char[dataLen+STRING_INC];
         if (tmp==NULL)
         {
            delete[] data;
            errString = "Out of memory";
            errState = 1;
            return -1;
         }
       
         memcpy(tmp, data, dataLen);
         delete[] data;
         data = tmp;
         dataLen += STRING_INC;
         ptr = data+dataPos;
      }
   
      retVal = GetBytes(ptr, BUFFER_BORDER);
      if (retVal==-1)
      {
         delete[] data;
         return -1;
      }
      if (retVal==0)
      {
         *ptr = 0;
         *buffer = data;
         
         MarkRecordEnd();
         return dataPos;
      }
   
      tmp = ptr;
      for (ct=0; ct<retVal; ct++)
      {
         if (*tmp=='@' && *(tmp-1)=='\n')
         {
            UnGetBytes(retVal-ct);
            *tmp = 0;
            *buffer = data;
            
            MarkRecordEnd();
            return dataPos+ct;
         }
       
         tmp++;
      }
   
      ptr += retVal;
      dataPos += retVal;
   }
}

int CAlephRecordReader::GetRecord(unsigned char **buffer)
{
   unsigned char *data, *ptr, *tmp, *tmp2;
   long dataLen, dataPos;
   long retVal, ct, ct2;
   int id, newId;
    
   data = new unsigned char[STRING_INC];
   if (data==NULL)
   {
      errString = "Out of memory";
      errState = 1;
      return -1;
   }
   dataLen = STRING_INC;

   MarkRecordStart();
   retVal = GetBytes(data, ALEPH_ID_LEN+1);
   if (retVal!=ALEPH_ID_LEN+1)
   {
      delete[] data;
      if (retVal==0)
         return 0;
      if (retVal==-1)
         return -1;

      errString = "Invalid format of aleph records";
      errState = 1;
      return -1;
   }

      // vezmi prvnich 9 znaku - ID zaznamu
   tmp = data; id = 0;
   for (ct=0; ct<ALEPH_ID_LEN; ct++)
   {
      if (*tmp<'0' || *tmp>'9')
      {
         delete[] data;
         errString = "Invalid format of aleph records";
         errState = 1;
         return -1;
      }
   
      id = 10*id+(*tmp-'0');
      tmp++;
   }
   if (*tmp!=' ')
   {
      delete[] data;
      errString = "Invalid format of aleph records";
      errState = 1;
      return -1;
   }
    
   ptr = data+ALEPH_ID_LEN+1; dataPos = ALEPH_ID_LEN+1;
   for (;;)
   {
      if (dataPos+BUFFER_BORDER-ALEPH_ID_LEN+1>dataLen)
      {
         tmp = new unsigned char[dataLen+STRING_INC];
         if (tmp==NULL)
         {
            delete[] data;
            errString = "Out of memory";
            errState = 1;
            return -1;
         }
       
         memcpy(tmp, data, dataLen);
         delete[] data;
         data = tmp;
         dataLen += STRING_INC;
         ptr = data+dataPos;
      }
   
      retVal = GetBytes(ptr, BUFFER_BORDER-12);
      if (retVal==-1)
      {
         delete[] data;
         return -1;
      }
      if (retVal==0)
      {
         *ptr = 0;
         *buffer = data;
         
         MarkRecordEnd();
         return dataPos;
      }
   
      tmp = ptr;
      for (ct=0; ct<retVal; ct++)
      {
            // hleda vzorek EOL .. 9 cislic .. mezera (musi byt ID)
         if (*tmp==' ' && *(tmp-ALEPH_ID_LEN-1)=='\n')
         {
            tmp2 = tmp-ALEPH_ID_LEN; newId = 0;
            for (ct2=0; ct2<ALEPH_ID_LEN; ct2++)
            {
               if (*tmp2<'0' || *tmp2>'9')
               {
                  delete[] data;
                  errString = "Invalid format of aleph records";
                  errState = 1;
                  return -1;
               }
               
               newId = newId*10+(*tmp2-'0');
               tmp2++;
            }
            
               // zacalo nove ID
            if (newId!=id)
            {
               UnGetBytes(retVal-ct+ALEPH_ID_LEN);
               *(tmp-ALEPH_ID_LEN) = 0;
               *buffer = data;
               
               MarkRecordEnd();
               return dataPos+ct-ALEPH_ID_LEN;
            }
         }
       
         tmp++;
      }
   
      ptr += retVal;
      dataPos += retVal;
   }
}

int CMarcFileReader::OpenFile(char *sPath, int format)
{
   CRecordReader *recReader;
   int retVal;
   CFlag *cflag;
   
   switch (format)
   {
      case LOAD_ISO_FORMAT:
         recReader = new CIsoRecordReader(sPath);
         break;

      case LOAD_TEXT_FORMAT:
         recReader = new CTextRecordReader(sPath);
         break;

      case LOAD_ALEPH_FORMAT:
         recReader = new CAlephRecordReader(sPath);
         break;
      
      default:
         g_Error = "Unknown record format";
         return 1;
   }
   
   if (recReader->IsError())
   {
      g_Error = recReader->StrError();
      return 1;
   }
   
   for (;;)
   {
      retVal = recReader->SkipRecord();
      
      if (retVal<0)
      {
         g_Error = recReader->StrError();
         delete recReader;
         return 1;
      }
      if (retVal==0) break;

      cflag = new CFlag;
      cflag->FPointer = recReader->GetRecordStart();
      cflag->Length = recReader->GetRecordLength();
      CollFlags.push_back(cflag);
   };
   
   delete recReader;
   
   if ((fr=fopen(sPath,"rb"))==NULL)
   {
      g_Error = "Cannot reopen input file";
      return 1;
   }

   return 0;
}

int CMarcFileReader::GetCountRecords()
{
	return CollFlags.size(); 
}

char* CMarcFileReader::GetRecord(long Index)
{
	if(Index<CollFlags.size())
	{
		int nBytesRead=0;
		CFlag* MyM;
		MyM = (CFlag*) CollFlags[Index];
            
		if (fseek(fr,MyM->FPointer,SEEK_SET))
			return (char*) 0;
		
		char* AcurateBuffer = new char[MyM->Length+1];
				
		nBytesRead = fread(AcurateBuffer, 1, MyM->Length, fr);

		if (nBytesRead<MyM->Length) 
		{
			return (char*) 0;
		}
		
		AcurateBuffer[nBytesRead] = '\0';
		
		return AcurateBuffer;
	}
    return 0;
}

int CMarcFileReader::CloseFile()
{
	fclose(fr);
	return 0;
}
