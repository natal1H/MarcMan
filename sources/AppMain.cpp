
#include "CConvertor.h"
#include "CMarcFileReader.h"
#include <time.h>

int codeVersion;

int main(int argc, char* argv[])
{
    ofstream log;
    if (argc==1)
        {
        cerr << "\n";
        cerr << "\n";
        cerr << "--------------------------------------------\n";
        cerr << "MarcMan " << MARCMAN_VERSION << " - conversion library formats    \n";
        cerr << "--------------------------------------------\n";
#ifdef ADMIN_VERSION
        cerr << "Administration version \n";
        cerr << "Working with coded and plain configuration files\n";
        cerr << "Creating coded files \n"; 
#endif  
        cerr << "--------------------------------------------\n";
        cerr << "\n";          
        cerr << "********************************************\n";
        cerr << "* Multidata Praha s.r.o.                   *\n";
        cerr << "* Sircova 202                              *\n"; 
        cerr << "* 196 00 Praha 9                           *\n"; 
        cerr << "* tel.: 283931741, 603259479, 603824881    *\n"; 
        cerr << "* -----------------                        *\n";
        cerr << "* Jindrich.Stejskal@ruk.cuni.cz            *\n";
        cerr << "********************************************\n";
        cerr << "\n";
        cerr << "ARGUMENT 1: Config file\n";
#ifdef ADMIN_VERSION
        cerr << "(ARGUMENT 2): Out code configuration file\n";
#endif  
        cerr << "\n";
        cerr << "\n";
		return 1;
        }
        
    CConvertor* MyC=new CConvertor();
    CMarcFileReader* MyFR=new CMarcFileReader();

    FILE *fw;
    FILE *fwerr;
    FILE *fwOK;
    int numberErrors;
    
    CConfigFileReader* MyConfFile=new CConfigFileReader();
    MyConfFile->SetLog(&log,0);
    
    if (MyConfFile->ReadFile(argv[1],&(MyC->Variables)))
    {
       cerr << "ERROR_ReadFile:" << argv[1] << "\n";
       return 0;
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
            cerr << "This version works only with coded files\n";
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
        }
        else
            MyC->SetLog(&log,0);
    }
    
    if (codeVersion==VERSION_ADMIN && argc==3)
    {
        if (MyC->SaveCodeRulesFile(argv[2]))
        {
              cerr << "Error coding file\n";
              return 0;    
        }     
        cerr << "OK - file created\n";
        return 0; 
    }
    
    //source file
    if (MyC->Variables.count("SourceFile")==0)
    {
        cerr << "ERROR_Config:" << "SourceFile not set" << "\n"; 
        return 1;
    }             
    const char * SourceFile = MyC->Variables["SourceFile"]->sName->c_str();

    //destination file
    if (MyC->Variables.count("DestinationFile")==0)
    {
        cerr << "ERROR_Config:" << "DestinationFile not set" << "\n"; 
        return 1;
    }             
    const char * DestinationFile = MyC->Variables["DestinationFile"]->sName->c_str();    
    
    //file for error input records
    if (MyC->Variables.count("InRecordErrorFile")==0)
    {
        cerr << "ERROR_Config:" << "InRecordErrorFile not set" << "\n"; 
        return 1;
    }             
    const char * ErrorFile = MyC->Variables["InRecordErrorFile"]->sName->c_str();

    //file for OK input records
    if (MyC->Variables.count("InRecordOKFile")==0)
    {
        cerr << "ERROR_Config:" << "InRecordOKFile not set" << "\n"; 
        return 1;
    }             
    const char * OKFile = MyC->Variables["InRecordOKFile"]->sName->c_str();

    string ControlFile = "";
    //if ((licenseUserData&0x0f)!=VERSION_LIGHT) // todo: just testing what does it do
    if (1 == 1)
    {
        //Control file
            if (MyC->Variables.count("ControlFile")>0)
            {
                ControlFile = *(MyC->Variables["ControlFile"]->sName);
            }             
    }
    //SourceFormat
    if (MyC->Variables.count("SourceFormat")==0)
    {
        cerr << "ERROR_Config:" << "SourceFormat not set" << "\n"; 
        return 1;
    }       
    int SFormat=0;      
    string SourceFormat = *(MyC->Variables["SourceFormat"]->sName);    

    //DestinationFormat
    if (MyC->Variables.count("DestinationFormat")==0)
    {
        cerr << "ERROR_Config:" << "DestinationFormat not set" << "\n"; 
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
        cout << "ERROR_Config:" << "SourceFormat invalid set" << "\n"; 
        return 1;
    }
    
    if(DFormat==0)
    {
        cout << "ERROR_Config:" << "DestinationFormat invalid set" << "\n"; 
        return 1;
    }
    //lang
    string Lang="ENG";
    if (MyC->Variables.count("Language"))
    {
        Lang = *(MyC->Variables["Language"]->sName);
    }       
    
    //open source, destination, Inerror and InOK files
    MyFR->OpenFile((char*) SourceFile,SFormat);
    if (MyFR->g_Error!="")
    {
        cerr << "ERROR_OPEN_SOURCE_FILE:" << MyFR->g_Error << "\n"; 
        return 1;
    }    
    cout << "OK: " << MyFR->GetCountRecords() << " Records opened" << "\n"; 
    log << "OK: " << MyFR->GetCountRecords() << " Records opened" << "\n"; 
    if ((fw=fopen((char*) DestinationFile,"wb"))== NULL)
    {
        cerr << "ERROR_OPEN_DESTINATION_FILE:" << "Not open" << "\n"; 
        return 1;
    }   
    
    if ((fwerr=fopen((char*) ErrorFile,"wb"))== NULL)
    {
        cerr << "ERROR_OPEN_ERROR_FILE:" << "Not open" << "\n"; 
        return 1;
    }
    
    if ((fwOK=fopen((char*) OKFile,"wb"))== NULL)
    {
        cerr << "ERROR_OPEN_OK_FILE:" << "Not open" << "\n"; 
        return 1;
    }
    
    if(ControlFile!="") {
              MyC->MarcIn->LoadCheck(ControlFile.c_str());
              if (MyC->sError!="") {
                  cerr << "ERROR_LOAD_CHECK: " + MyC->sError + "\n";
                  return 1;
              } 
     }
    
    //Load inicialize                                                                                                            
    if (MyC->LoadAll(CodeFile))
    {
       cerr << "ERROR_LOAD_ALL: " + MyC->sError + "\n";
       return 1;
    }      
    cout << "OK: " << "Rules file is recognized" << "\n"; 
    log << "OK: " << "Rules file is recognized" << "\n"; 
    cout << "\n"; 
    
    long i;
    numberErrors=0;
    for(i=0;i<MyFR->GetCountRecords();i++)
    {
       char* TRec = MyFR->GetRecord(i);
       string myR = TRec;
       
       if (i%100==0) cout << "Converting record " << i+1 << "/" << MyFR->GetCountRecords() << "\n";
       log << "-----------------------------------------------------------------\n";
       log <<"Converting record " << i+1 << "/" << MyFR->GetCountRecords() << "\n";
       
       if (MyC->InsertRecord(&myR,SFormat))
       {
          fwrite((const void *) TRec,1,strlen(TRec),fwerr); 
          fwrite("\n",1,1,fwerr); 
          fwrite(MyC->sError.c_str(),1,MyC->sError.length(),fwerr); 
          fwrite("\n",1,1,fwerr);           
          fwrite("[[ERROR]] ",1,9,fwerr); 
          cerr << "ERROR_INSERT_RECORD: " + MyC->sError + "\n";     
          log << "Error inserting record: " << MyC->sError << "\n";
          log << "-----------------------------------------------------------------\n";
          MyC->ClearErrors();
          numberErrors++;
          delete[] TRec;
          continue;
       }  
       
       log << "ID RECORD: '" << *(MyC->MarcIn->g_FieldPrefix) << "_";
       
       int Size=MyC->MarcIn->Fields.size();
       for(int f=0;f<Size;f++) {
         if (*(MyC->MarcIn->Fields[f]->ID)=="001") {
             int Size2 = MyC->MarcIn->Fields[f]->ptrCollSubf.size();
             for(int j=0;j<Size2;j++) {
                if (*(MyC->MarcIn->Fields[f]->ptrCollSubf[j]->ID)=="E") {
                   log << *(MyC->MarcIn->Fields[f]->ptrCollSubf[j]->Value);
			    }
			}
         }
       }
       
       log << "'\n";
       log << "-----------------------------------------------------------------\n";
       string CheckErr;
       if(ControlFile!="") {
          log << "General verification" << "\n";
          log << "====================" << "\n";
          string sErr = MyC->MarcIn->CheckRecord(Lang);
          log << sErr << "\n";
          if (sErr!="") 
             CheckErr="CHECK ERROR";
       }
       
       MyC->Convert();
       
       if(MyC->sError=="") 
          MyC->sError = CheckErr; 
       
       if (MyC->sError!="") 
       {
          fwrite((const void *) TRec,1,strlen(TRec),fwerr); 
          fwrite("\n",1,1,fwerr); 
          fwrite(MyC->sError.c_str(),1,MyC->sError.length(),fwerr); 
          fwrite("\n",1,1,fwerr);           
          fwrite("[[ERROR]] ",1,9,fwerr); 
          fwrite("\n",1,1,fwerr);
          //cerr << "ERROR_CONVERT: " + MyC->sError + "\n";
          log << "Error converting record: " << MyC->sError << "\n";
          numberErrors++;
          delete[] TRec;
          continue;
       }        
       string* OutS =MyC->GetOut(DFormat);          
       if (MyC->sError!="") 
       {
          fwrite((const void *) TRec,1,strlen(TRec),fwerr); 
          fwrite("\n",1,1,fwerr); 
          fwrite("[[ERROR]] ",1,9,fwerr); 
          fwrite(MyC->sError.c_str(),1,MyC->sError.length(),fwerr); 
          fwrite("\n",1,1,fwerr);           
          cerr << "ERROR_CONVERT: " + MyC->sError + "\n";
          log << "Error outing record: " << MyC->sError << "\n";
          numberErrors++;
          delete[] TRec;
          delete OutS;
          continue;
       }
       //OK
       fwrite((const void *) TRec,1,strlen(TRec),fwOK); 
       fwrite("\n",1,1,fwOK);           
       //Convert
       fwrite(OutS->c_str(),1,OutS->length(),fw);
       
       delete[] TRec;
	   delete OutS;
       
    }
    
    MyC->DoneAll();
    /*if (MyC->sError!="") 
    {
        cerr << "ERROR_DONE_ALL: " + MyC->sError + "\n";
    }*/
    
    fclose(fw);
    fclose(fwerr);
    fclose(fwOK);
    cout << "\n";
    cout << "CONVERT " << MyFR->GetCountRecords() << " Records" << "\n";
    cout << numberErrors;
    cout << " RECORDS WITH ERRORS";
    cout << "\n";
    log << "*****************************************************************\r\n";
    log << "STATISTIC" << "\r\n";
    log << "CONVERT " << MyFR->GetCountRecords() << " RECORDS\r\n";
    log << (MyFR->GetCountRecords() - numberErrors) << " RECORDS OK\r\n";
    log << numberErrors << " RECORDS WITH ERRORS\r\n";
    log << "*****************************************************************\r\n";
    
   // delete MyC;
   delete MyConfFile;
   // delete MyFR;
   
   return 0;
}


