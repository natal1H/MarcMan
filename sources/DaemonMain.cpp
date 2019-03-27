#include "CConvertor.h"
#include "CMarcFileReader.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <errno.h>

#ifndef __socklen_t_defined
#define __socklen_t_defined
typedef int socklen_t;
#endif

#define UNIX_PATH_MAX    108

   // pomocna funkce, ktera vytvari pracovni podprocesy
int ForkWorkers(int workers);
   // pomocna funkce pro nacteni Marc zaznamu ze sekvencniho vstupu
int GetSeqRecord(int desc, char **buffer, int fmt);

int codeVersion;

int main(int argc, char* argv[])
{
   if (argc<2 || argc>3 || (argc==3 && strcmp(argv[2], "-k")))
   {
      cerr << "Usage: marcmand <configuration file>\n";
//      cerr << "Usage: marcmand <configuration file> [-k]\n";
//      cerr << "         -k kills marcmand (using pid file from configuration)\n";
      return 1;
   }

   CConvertor* MyC=new CConvertor();
   ofstream log;
   int logging = 0;

   CConfigFileReader* MyConfFile=new CConfigFileReader();
   MyConfFile->SetLog(&log,0);

   if (MyConfFile->ReadFile(argv[1],&(MyC->Variables)))
   {
      cerr << "marcmand: can't read configuration file " << argv[1] << "\n";
      return 1;
   }

   codeVersion = VERSION_ADMIN;

   int CodeFile=0;
   if (MyC->Variables.count("CodeFile")!=0)
   {
      if (*(MyC->Variables["CodeFile"]->sName)=="Yes") CodeFile=1;
   }
   
   if (codeVersion==VERSION_LIGHT)
   {
      if(CodeFile!=1)
      {
         cerr << "marcmand: this version works only with coded files\n";
         return 0;
      }
      MyC->SetLog(&log,0);
   }
   else
   {
      //set log
      if (MyC->Variables.count("LogFile")!=0 && MyC->Variables.count("EnableLogAll")!=0)
      {
         M_EXPRESSION_ELEMENT* myVLF=MyC->Variables["LogFile"];
         M_EXPRESSION_ELEMENT* myVLFE=MyC->Variables["EnableLogAll"];
         log.open(myVLF->sName->c_str());
         if (*(myVLFE->sName)=="Yes")
            MyC->SetLog(&log,1);
         else
            MyC->SetLog(&log,0);
         logging = 1;
      }
      else
         MyC->SetLog(&log,0);
   }

   if (MyC->Variables.count("SourceFormat")==0)
   {
      cerr << "marcmand: SourceFormat is not set\n"; 
      return 1;
   }
   int SFormat=0;      
   string SourceFormat = *(MyC->Variables["SourceFormat"]->sName);    

   if (MyC->Variables.count("DestinationFormat")==0)
   {
      cerr << "marcmand: DestinationFormat is not set\n"; 
      return 1;
   }
   int DFormat=0;
   string DestinationFormat = *(MyC->Variables["DestinationFormat"]->sName);        
    
   if (SourceFormat=="iso") SFormat=1;
   if (SourceFormat=="txt") SFormat=2;
   if (SourceFormat=="aleph") SFormat=3;
    
   if (DestinationFormat=="iso") DFormat=1;
   if (DestinationFormat=="txt") DFormat=2;
   if (DestinationFormat=="aleph") DFormat=3;
    
   if(SFormat==0)
   {
      cerr << "marcmand: unknown SourceFormat type\n"; 
      return 1;
   }
    
   if(DFormat==0)
   {
      cerr << "marcmand: unknown DestinationFormat type\n"; 
      return 1;
   }

   string listenAddr;
   string protoStr;
   int protocol, workers;
   if (MyC->Variables.count("Protocol")==0)
      protoStr = "unix";
   else
      protoStr = *(MyC->Variables["Protocol"]->sName);
   if (MyC->Variables.count("ListenAddress")==0)
      listenAddr = "marcmand.sock";
   else
      listenAddr = *(MyC->Variables["ListenAddress"]->sName);
   if (MyC->Variables.count("Workers")==0)
      workers = 4;
   else
   {
      workers = atoi(MyC->Variables["Workers"]->sName->c_str());
      
      if (workers<=0)
      {
         cerr << "marcmand: invalid Workers number\n"; 
         return 1;
      }
   }
   if (MyC->Variables.count("PidFile")==0)
      ; //cerr << "marcmand: warning: no PidFile in configuration (-k option won't work)\n";
   else
   {
      FILE *pidFile;
      
      pidFile = fopen(MyC->Variables["PidFile"]->sName->c_str(), "w");
      if (pidFile==NULL)
      {
         cerr << "marcmand: warning: can't open PidFile " <<
            MyC->Variables["PidFile"]->sName->c_str() << " (" <<
            strerror(errno) << ")\n";
      }
      else
      {
         fprintf(pidFile, "%d\n", getpid());
         fclose(pidFile);
      }
   }

   if (protoStr=="unix") protocol = PF_UNIX;
   else if (protoStr=="tcp") protocol = PF_INET;
   else
   {
      cerr << "marcmand: unknown Protocol type\n"; 
      return 1;
   }

      //Load inicialize
   if (MyC->LoadAll(CodeFile))
   {
      cerr << "marcmand: error while loading rule set: " + MyC->sError + "\n";
      return 1;
   }
   
   int lSock, cSock;
   struct sockaddr_in inetAddr;
   struct sockaddr_un unixAddr;
   struct sockaddr *addr;
   socklen_t addrLen, addrLen2;
   int retVal, accFail, wrlen;
   char* TRec;
   string myR, *OutS;

   if (protocol==PF_INET)
   {
      inetAddr.sin_port = htons(atoi(listenAddr.c_str()));
      inetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      inetAddr.sin_family = AF_INET;
      
      if (inetAddr.sin_port==0)
      {
         cerr << "marcmand: ListenAddress must be valid port number (1-65535)\n";
         return 1;
      }
      
      addr = (struct sockaddr *)&inetAddr;
      addrLen = sizeof(inetAddr);
   }
   else
   {
      unixAddr.sun_family = AF_UNIX;
      if (listenAddr.length()>=UNIX_PATH_MAX)
      {
         cerr << "marcmand: ListenAddress too long\n";
         return 1;
      }
      strcpy(unixAddr.sun_path, listenAddr.c_str());
      
      addr = (struct sockaddr *)&unixAddr;
      addrLen = sizeof(unixAddr);
   }
   
   lSock = socket(protocol, SOCK_STREAM, 0);
   if (lSock==-1)
   {
      perror("marcmand: socket failed");
      return 2;
   }

   if (bind(lSock, addr, addrLen)==-1)
   {
      perror("marcmand: bind failed");
      close(lSock);
      return 2;
   }
   if (listen(lSock, 32)==-1)
   {
      perror("marcmand: listen failed");
      close(lSock);
      return 2;
   }
   
   retVal = fork();
   if (retVal==-1)
   {
      perror("marcmand: fork failed");
      close(lSock);
      return 3;
   }
      // hlavni proces skonci a necha zit jen potomky
   if (retVal!=0)
   {
      close(lSock);
      return 0;
   }
      // znezavisli proces na ridicim shellu
   setsid();
   
      // forkne podprocesy, ktere teprve pracuji
   if (ForkWorkers(workers)==-1) return 4;
   
   accFail = 0;
   retVal = 0;
   do
   {
      addrLen2 = addrLen;
      cSock = accept(lSock, addr, &addrLen2);
      if (cSock==-1)
      {
         accFail++;
         if (accFail>10)
         {
            close(lSock);
            return 2;
         }
         continue;
      }
      accFail = 0;
    
      for (;;)
      {
         TRec = NULL;
         OutS = NULL;
         retVal = GetSeqRecord(cSock, &TRec, SFormat);
         if (retVal<=0) break;
   
         myR = TRec;
         retVal = MyC->InsertRecord(&myR,SFormat);
         if (retVal)
         {
            if (logging)
               log << "Error inserting record: " << MyC->sError << "\n";
            break;
         }

         if (logging)
         {
            int Size=MyC->MarcIn->Fields.size();
            for(int f=0;f<Size;f++)
               if (*(MyC->MarcIn->Fields[f]->ID)=="001")
               {
                  int Size2 = MyC->MarcIn->Fields[f]->ptrCollSubf.size();
                  for(int j=0;j<Size2;j++)
                     if (*(MyC->MarcIn->Fields[f]->ptrCollSubf[j]->ID)=="E")
                        log << *(MyC->MarcIn->Fields[f]->ptrCollSubf[j]->Value);
               }
         }
         
         MyC->Convert();
         if (MyC->sError!="") 
         {
            if (logging)
               log << "Error converting record: " << MyC->sError << "\n";
            retVal = -1;
            break;
         }
   
         OutS = MyC->GetOut(DFormat);
         if (MyC->sError!="")
         {
            if (logging)
               log << "Error outing record: " << MyC->sError << "\n";
            retVal = -1;
            break;
         }

         do
         {
            wrlen = write(cSock, OutS->c_str(), OutS->length());
         } while (wrlen==-1 && errno==EINTR);
         
         if (wrlen<0)
         {
            retVal = -1;
            break;
         }

         delete[] TRec;
         delete OutS;
      }
   
      close(cSock);
      
   } while (retVal!=-1);
   
   close(lSock);

   return 4;
}
// --------------------------------------------------
// pomocne funkce - prace s podprocesy

int ForkWorkers(int workers)
{
   int chldNum;
   pid_t newPid, chldPid;
   
   chldNum = 0;
   
   for (;;)
   {
      while (chldNum<workers)
      {
         newPid = fork();
         
            // ukonci funkci pokud jsem dite nebo fork selhal
         if (newPid==-1 || newPid==0)
            return newPid;

         chldNum++;
      }
      
      chldPid = wait(NULL);
      if (chldPid>0) chldNum--;
   }
}

// --------------------------------------------------
// pomocne funkce - nacitani zaznamu

int GetIsoSeqRecord(int desc, char **buffer);
int GetTxtSeqRecord(int desc, char **buffer);
int GetAlephSeqRecord(int desc, char **buffer);

#define BUFFER_SIZE     8192
#define BUFFER_BORDER   32
#define STRING_INC      512
#define ALEPH_ID_LEN    9

unsigned char readBuffer[BUFFER_SIZE];
unsigned char *readBufferPtr;
int readPos = 0;
int readLen = 0;

int ReadByte(int desc)
{
   int val;
    
   if (readPos>=readLen)
   {
      if (readLen>=BUFFER_BORDER)
      {
         memmove(readBuffer, readBuffer+readPos-BUFFER_BORDER, BUFFER_BORDER);
         do {
            readLen = read(desc, readBuffer+BUFFER_BORDER,
                           BUFFER_SIZE-BUFFER_BORDER);
         } while (readLen==-1 && errno==EINTR);
         if (readLen>0) readLen += BUFFER_BORDER;
       
         readPos = BUFFER_BORDER;
      }
      else
      {
         do {
            readLen = read(desc, readBuffer, BUFFER_SIZE);
         } while (readLen==-1 && errno==EINTR);
         readPos = 0;
      }
       
      if (readLen==-1) return -2;
      if (readLen==0) return -1;
       
      readBufferPtr = readBuffer + readPos;
   }
    
   val = *readBufferPtr;
   readBufferPtr++;
   readPos++;
    
   return val;
}

int GetBytes(int desc, unsigned char *buffer, int len)
{
   int pos, val;
   unsigned char *ptr;
    
   ptr = buffer;
   for (pos=0; pos<len; pos++)
   {
      val = ReadByte(desc);
      if (val==-1) break;
      if (val==-2) return -1;
   
      *ptr = val; ptr++;
   }
    
   return pos;
}

int UnGetBytes(int len)
{
   if (len>readPos) return -1;
   readPos -= len;
   readBufferPtr -= len;
    
   return 0;
}

int GetSeqRecord(int desc, char **buffer, int fmt)
{
   switch (fmt)
   {
         // ISO format
      case 1:
         return GetIsoSeqRecord(desc, buffer);
       
         // TXT format
      case 2:
         return GetTxtSeqRecord(desc, buffer);
       
         // ALEPH format
      case 3:
         return GetAlephSeqRecord(desc, buffer);
       
      default:
         return -1;
   }
}

int GetIsoSeqRecord(int desc, char **buffer)
{
   unsigned char data[6];
   int retVal, len;
    
   retVal = GetBytes(desc, data, 5);
   if (retVal<=0) return retVal;
   if (retVal<5) return -1;
    
   data[5] = 0;
   len = atoi((char*)data);
   if (len<=5) return -1;
    
   *buffer = new char[len+1];
   if (*buffer==NULL) return -1;
    
   memcpy(*buffer, data, 5);
   len -= 5;
    
   retVal = GetBytes(desc, (unsigned char*)*buffer+5, len);
   if (retVal==-1 || retVal<len)
      return -1;
   else
   {
      (*buffer)[retVal] = 0;
      return retVal;
   }
}

int GetTxtSeqRecord(int desc, char **buffer)
{
   unsigned char *data, *ptr, *tmp;
   int dataLen, dataPos;
   int retVal, ct;
    
   data = new unsigned char[STRING_INC];
   if (data==NULL) return -1;
   dataLen = STRING_INC;

   retVal = GetBytes(desc, data, 3);
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
      return -1;
   }
    
   ptr = data; dataPos = 0;
   for (;;)
   {
      if (dataPos+BUFFER_BORDER+1>dataLen)
      {
         tmp = new unsigned char[dataLen+STRING_INC];
         if (tmp==NULL)
         {
            delete[] data;
            return -1;
         }
       
         memcpy(tmp, data, dataLen);
         delete[] data;
         data = tmp;
         dataLen += STRING_INC;
         ptr = data+dataPos;
      }
   
      retVal = GetBytes(desc, ptr, BUFFER_BORDER);
      if (retVal==-1)
      {
         delete[] data;
         return -1;
      }
      if (retVal==0)
      {
         *ptr = 0;
         *buffer = (char*)data;
         return dataPos;
      }
   
      tmp = ptr;
      for (ct=0; ct<retVal; ct++)
      {
         if (*tmp=='@' && *(tmp-1)=='\n')
         {
            UnGetBytes(retVal-ct);
            *tmp = 0;
            *buffer = (char*)data;
            return dataPos+ct;
         }
       
         tmp++;
      }
   
      ptr += retVal;
      dataPos += retVal;
   }
}

int GetAlephSeqRecord(int desc, char **buffer)
{
   unsigned char *data, *ptr, *tmp, *tmp2;
   int dataLen, dataPos;
   int retVal, ct, ct2, id, newId;
    
   data = new unsigned char[STRING_INC];
   if (data==NULL) return -1;
   dataLen = STRING_INC;

   retVal = GetBytes(desc, data, ALEPH_ID_LEN+1);
   if (retVal!=ALEPH_ID_LEN+1)
   {
      delete[] data;
      if (retVal==0)
         return 0;
      else
         return -1;
   }

      // vezmi prvnich 9 znaku - ID zaznamu
   tmp = data; id = 0;
   for (ct=0; ct<ALEPH_ID_LEN; ct++)
   {
      if (*tmp<'0' || *tmp>'9')
      {
         delete[] data;
         return -1;
      }
   
      id = 10*id+(*tmp-'0');
      tmp++;
   }
   if (*tmp!=' ')
   {
      delete[] data;
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
            return -1;
         }
       
         memcpy(tmp, data, dataLen);
         delete[] data;
         data = tmp;
         dataLen += STRING_INC;
         ptr = data+dataPos;
      }
   
      retVal = GetBytes(desc, ptr, BUFFER_BORDER-12);
      if (retVal==-1)
      {
         delete[] data;
         return -1;
      }
      if (retVal==0)
      {
         *ptr = 0;
         *buffer = (char*)data;
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
               *buffer = (char*)data;
               return dataPos+ct-ALEPH_ID_LEN;
            }
         }
       
         tmp++;
      }
   
      ptr += retVal;
      dataPos += retVal;
   }
}
