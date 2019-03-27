
#include "CConvertor.h"

#include <stdio.h>
#include <fcntl.h>


char* KEYWORD_RULESFILE=       "RulesFile";
char* KEYWORD_GROUP=           "GROUP";
char* KEYWORD_INNOIDFIELD=     "InNoIDField";
char* KEYWORD_OUTNOIDFIELD=    "OutNoIDField";
char* KEYWORD_IDALEPHCODING=   "IDAlephCoding";
char* KEYWORD_INLINKINGFIELDS= "InLinkingFields";
char* KEYWORD_OUTLINKINGFIELDS="OutLinkingFields";

int EcsapeString(string *strText,char Escape)
{
    // (*log)<< "ESCAPE: " << *strText << "\n";
    for(int i=strText->length()-1;i>(-1);i--)
    {
        if ((*strText)[i]==Escape)
        {
            if (i<strText->length()-1)
            { 
                switch((*strText)[i+1])
                {
                    case 'n':
                        (*strText)[i]='\n';
                        strText->erase(i+1,1);
                        break;
                    case 'r':
                        (*strText)[i]='\r';
                        strText->erase(i+1,1);
                        break;
                    case '\\':
                        (*strText)[i]='\\';
                        strText->erase(i+1,1);
                        break;
                    case 't':
                        (*strText)[i]='\t';
                        strText->erase(i+1,1);
                        break;
                }
            }
            if (i<strText->length()-3)
            {
                int NumS=EscapeToInt(strText->substr(i+1,3));
                if (NumS>0)
                {
                    (*strText)[i]=NumS;
                    strText->erase(i+1,3);
                }
            }
        }
    }
    //(*log)<< "ESCAPEEND: " << *strText << "\n";      
    return 0;
}

void Replace(string* strSplit, char* findStr, char* repStr)
{
    int Poss;
    while((Poss=strSplit->find(findStr))!=-1)
    {
        strSplit->replace(Poss,strlen(findStr),repStr);
    }
}



string OrdinalForm(int number, int size)
{
    string myNumber;
    char buffer[20];

    sprintf(buffer,"%d",number);
    myNumber=buffer;
    if (size!=0)
        for (int j=myNumber.length();j<size;j++)
            myNumber="0"+myNumber;
    return myNumber;
}

void TrimLeft(string* sStr, char* ArraySep)
{
    int finds=1;
    int i;
    while(finds==1)
    {
        finds=0;
        for(i=0;i<strlen(ArraySep);i++)
        {
            if (sStr->size()>0)
            {
                if ((*sStr)[0]==ArraySep[i])
                {
                    finds=1;
                    sStr->erase(0,1);
                }
            }
        }
    }
}

int SplitString(string* strSplit, vector<string*>* outArray, char* Separator)
{
    int iPos = 0;
    int newPos = -1;
    string sSeparator=Separator;
    int sizeS2 = sSeparator.size();
    int isize = strSplit->size();

    vector<int> positions;

    newPos = strSplit->find(Separator, 0);

    if(newPos==(-1)) 
    {
        outArray->push_back(new string(strSplit->substr(0)));
        return 0;
    }

    while(newPos!=(-1))
    {
        positions.push_back(newPos);
        iPos = newPos;
        newPos = strSplit->find (Separator, iPos+sizeS2+1);
    }

    for(int i=0;i<=positions.size();i++)
    {
        string s;
        if(i==0) 
        { 
            s = strSplit->substr(0, positions[i]);
        }
        else
        {
            int offset = positions[i-1] + sizeS2;
            if(offset<isize)
            {
                if(i==positions.size())
                {
                    s=strSplit->substr(offset);
                }
                else 
                {
                    s=strSplit->substr(positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2);
                }
            }
        }
        outArray->push_back(new string(s));
    }
    return 0;
}

void TrimRight(string* sStr, char* ArraySep)
{
    int finds=1;
    int i;
    while(finds==1)
    {
        finds=0;
        for(i=0;i<strlen(ArraySep);i++)
        {
            if (sStr->size()>0)
            {
                if ((*sStr)[sStr->size()-1]==ArraySep[i])
                {
                    finds=1;
                    sStr->erase(sStr->size()-1,1);
                }
            }
        }
    }
}

void StripAsciiChars(string* sStr, const char* chars, int complement) {
    
    int nchars = strlen(chars);
    int i;
    int j;
    
    for(i =0; i < sStr->size(); i++) 
    {
        for(j = 0; j < nchars; j++) 
        {
            if((*sStr)[i] == chars[j])
                break;
        }
        if ((j < nchars && complement == 0) || ( j == nchars && complement == 1)) 
        {
            sStr->erase(i--,1);
        }
    }
}

void TranslateAsciiChars(string* sStr, const char* srcChars, const char *destChars) {
    
    int nchars = strlen(srcChars);
    int i;
    int j;
    
    for(i =0; i < sStr->size(); i++) 
    {
        for(j = 0; j < nchars; j++) 
        {
            if((*sStr)[i] == srcChars[j])
                (*sStr)[i] = destChars[j];
        }
    }
}

void GetMaxUnsignedInteger(string* sStr) {
    
    int i;
    int p1 = 0;
    int l1 = 0;
    int n;
    int maxVal = -1;
    string maxStr = "";
    string t = "";
     
    for(i =0; i < sStr->size(); i++) 
    {
        
        if(isdigit((*sStr)[i])) 
        {
            if(l1 == 0) 
            {
                p1 = i;
                l1 = 1;
            }
            else
            {
                l1++;
            }
        }
        else 
        {
            if(l1 > 0) 
            {
                t = sStr->substr(p1,l1);
                if((n = atoi(t.c_str())) > maxVal) 
                {
                    maxVal = n;
                    maxStr = t;
                }                
                l1 = 0;
            }
        }
    }
    if(l1 > 0) {
        t = sStr->substr(p1,l1);
        if((n = atoi(t.c_str())) > maxVal) 
        {
            maxVal = n;
            maxStr = t;
        }                
    }
    (*sStr) = maxStr;
}

int CConfigFileReader::ReadFile(char * sPath, map<string, M_EXPRESSION_ELEMENT*>* VIt)
{
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConfigFileReader::ReadFile\n";
        (*log)<< "sPath:" << sPath << "\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug

    FILE *fr;
    vector<string *> Elements;
    string strAnalyzeText;
    if ((fr=fopen(sPath,"r"))== NULL)
    {
        //(*log)<< "Soubor se nepodarilo otevrit\n";
        return 1;
    }          
    int nBytesRead=0;
    char* buffer = new char[MAXBUF];

    while ((nBytesRead = fread(buffer, 1, MAXBUF - 1,fr))!=0)
    {
        buffer[nBytesRead] = '\0';
        strAnalyzeText+=buffer;	
    };    

    SplitString(&strAnalyzeText,&Elements,"\n");
    for(int i=0;i<Elements.size();i++)
    {
        int Poss=Elements[i]->find("=");
        if (Poss!=(-1))
        {
            M_EXPRESSION_ELEMENT* NewEl = new M_EXPRESSION_ELEMENT();
            NewEl->iType=EX_STRING;
            NewEl->sName=new string(Elements[i]->substr(Poss+1));
            TrimLeft(NewEl->sName,CUTTRIMED);
            TrimRight(NewEl->sName,CUTTRIMED);
            EcsapeString(NewEl->sName,'\\');
            string Key =Elements[i]->substr(0,Poss);  
            if (LogAll) { //Debug
                (*log)<< "NEWVAR: " << Key << "-" <<  *(NewEl->sName) << "\n";         
            } //End Debug                
            VIt->insert(make_pair(Key,NewEl));
        }
    }
    for (int z=0;z<Elements.size();z++)
        delete Elements[z];
    fclose(fr);
    delete[] buffer;
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConfigFileReader::ReadFile\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    return 0;
}

int ControlArguments(vector<M_EXPRESSION_ELEMENT *>* outarrElements, string mask)
{
    if(outarrElements->size()==0)
    {
        return 0;
    }
    if(outarrElements->size() != mask.length()) return 0;
    for(int i=0;i<outarrElements->size();i++)
    {
        if ((*outarrElements)[i]->iType == EX_STRING)
        {
            if (mask.substr(i, 1) != "S" && mask.substr(i, 1) != "X") return 0;
        }
        if ((*outarrElements)[i]->iType == EX_INTEGER)
        {
            if (mask.substr(i, 1) != "I" && mask.substr(i, 1) != "X") return 0;
        }
    }
    return 1;
}

int VIn(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iN)
{
    return (*outarrElements)[iN]->iIntValue;
}

string* VSt(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iN)
{
    return (*outarrElements)[iN]->sName;
}

int VTy(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iN)
{
    return (*outarrElements)[iN]->iType;
}


M_EXPRESSION_ELEMENT * CallFunc(void* ptrClientClass, string* MethodName, vector<M_EXPRESSION_ELEMENT *>* El)
{

    CConvertor * PCC = (CConvertor *) ptrClientClass;

    if (PCC->LogAll) { //Debug
        *(PCC->log)<< "---------------------------------------------\n";
        *(PCC->log)<< "BEGIN: CallFunc\n";
        *(PCC->log)<< "MethodName :" << *MethodName << "\n";
        for(int v=0;v<El->size();v++)
        {
            *(PCC->log)<< "PARAMETR " << v << ":";
            if ((*El)[v]->iType==EX_INTEGER)
                *(PCC->log)<< "(INT) " << (*El)[v]->iIntValue;
            if ((*El)[v]->iType==EX_STRING)
                *(PCC->log)<< "(STR) " << *((*El)[v]->sName);    
            *(PCC->log)<< "\n";
        }
        *(PCC->log)<< "---------------------------------------------\n";
    } //End Debug
    M_EXPRESSION_ELEMENT * retValue=new M_EXPRESSION_ELEMENT();
    retValue->iType=EX_INTEGER;
    retValue->sName=new string();
    retValue->iIntValue=0;
    //-------------------------------------------------------------------------------------------------------
    // Try plugins
    //-------------------------------------------------------------------------------------------------------
    if(PCC->PluginManager->callPluginFunction(PCC->MarcIn, PCC->MarcOut, MethodName, El, retValue) == 0)
        goto exitCallFunc; 
    //-------------------------------------------------------------------------------------------------------
    // WriteToLog
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="WriteToLog")
    {    
        if (ControlArguments(El,"X"))
        {
            if (VTy(El,0) == EX_STRING)
                *(PCC->log)<< *(VSt(El,0)) << "\n";
            else
                *(PCC->log)<< OrdinalForm(VIn(El,0),0) << "\n";    
        }
        else
        {
            PCC->sError="WriteToLog - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }
    //-------------------------------------------------------------------------------------------------------
    // Mid
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="Mid")
    {   
        if (ControlArguments(El,"SI") || ControlArguments(El,"SII") )
        { 
            if (ControlArguments(El,"SI"))
            {
                retValue->iType=EX_STRING;
                if (VIn(El,1) > 0 && VIn(El,1) <= VSt(El,0)->length())
                    *(retValue->sName) = VSt(El,0)->substr(VIn(El,1)-1);
                else
                    *(retValue->sName) = "";
            }
            else
            {
                retValue->iType=EX_STRING;
                if ((VIn(El,1) > 0 && VIn(El,1) <= VSt(El,0)->length()) && (VIn(El,2) > 0))
                    *(retValue->sName) = VSt(El,0)->substr((VIn(El,1)-1),VIn(El,2));
                else
                    *(retValue->sName) = "";  
            }
        }    
        else
        {
            PCC->sError="Mid - Invalid number of arguments";    
            retValue->iType=EX_ERROR;
        }
    }    
    //-------------------------------------------------------------------------------------------------------
    // Len
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="Len")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iType = EX_INTEGER;
            retValue->iIntValue = VSt(El,0)->length();
        }    
        else
        {
            PCC->sError="Len - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }        
    //-------------------------------------------------------------------------------------------------------
    // Find
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="Find")
    {    
        if (ControlArguments(El,"SS") || ControlArguments(El,"SSI") )
        { 
            if (ControlArguments(El,"SS"))
            {
                retValue->iType = EX_INTEGER;           
                retValue->iIntValue = VSt(El,0)->find(VSt(El,1)->c_str()) + 1;
            }
            else
            {
                retValue->iType = EX_INTEGER;           
                retValue->iIntValue = VSt(El,0)->find(VSt(El,1)->c_str(),VIn(El,2)) + 1 ;
            }
        }
        else
        {
            PCC->sError="Find - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }    
    //-------------------------------------------------------------------------------------------------------
    // FindReverse
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="FindReverse")
    {    
        if (ControlArguments(El,"SS") || ControlArguments(El,"SSI") )
        { 
            if (ControlArguments(El,"SS"))
            {
                retValue->iType = EX_INTEGER;           
                retValue->iIntValue = VSt(El,0)->rfind(VSt(El,1)->c_str()) + 1 ;
            }
            else
            {
                retValue->iType = EX_INTEGER;           
                retValue->iIntValue = VSt(El,0)->rfind(VSt(El,1)->c_str(),VIn(El,2)) + 1 ;
            }
        }
        else
        {
            PCC->sError="FindReverse - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }
    //-------------------------------------------------------------------------------------------------------
    // StripAsciiChars
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="StripAsciiChars")
    {    
        if (ControlArguments(El,"SSI"))
        {
            retValue->iType = EX_STRING;
            StripAsciiChars(VSt(El,0),VSt(El,1)->c_str(),VIn(El,2));
            *(retValue->sName) = *VSt(El,0);
        }    
        else
        {
            PCC->sError="StripAsciiChars - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  
    //-------------------------------------------------------------------------------------------------------
    // TranslateAsciiChars
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="TranslateAsciiChars")
    {    
        if (ControlArguments(El,"SSS"))
        {
            if(VSt(El,1)->size() == VSt(El,2)->size())
            {
                retValue->iType = EX_STRING;
                TranslateAsciiChars(VSt(El,0),VSt(El,1)->c_str(),VSt(El,2)->c_str());
                *(retValue->sName) = *VSt(El,0);
            }
            else 
            {
                PCC->sError="TranslateAsciiChars - Lengths of translation strings don't match";
                retValue->iType=EX_ERROR;
             }
        }    
        else
        {
            PCC->sError="TranslateAsciiChars - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  
    //-------------------------------------------------------------------------------------------------------
    // GetMaxUnsignedInteger
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetMaxUnsignedInteger")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iType = EX_STRING;
            GetMaxUnsignedInteger(VSt(El,0));
            *(retValue->sName) = *VSt(El,0);
        }    
        else
        {
            PCC->sError="GetMaxUnsignedInteger - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  


    //-------------------------------------------------------------------------------------------------------
    // Trim
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="Trim")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iType = EX_STRING;
            TrimLeft(VSt(El,0),CUTTRIMED);
            TrimRight(VSt(El,0),CUTTRIMED);
            *(retValue->sName) = *VSt(El,0);
        }    
        else
        {
            PCC->sError="Trim - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  

    if((*MethodName)=="TrimChars")
    {    
        if (ControlArguments(El,"SS"))
        {
            retValue->iType = EX_STRING;
            TrimLeft(VSt(El,0),(char *) VSt(El,1)->c_str());
            TrimRight(VSt(El,0),(char *) VSt(El,1)->c_str());
            *(retValue->sName) = *VSt(El,0);
        }    
        else
        {
            PCC->sError="TrimChars - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  

    if((*MethodName)=="Replace")
    {    
        if (ControlArguments(El,"SSS"))
        {
            retValue->iType = EX_STRING;
            Replace(VSt(El,0),(char *) VSt(El,1)->c_str(),(char *) VSt(El,2)->c_str());
            *(retValue->sName) = *VSt(El,0);
        }    
        else
        {
            PCC->sError="Replace - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }  

    if((*MethodName)=="GetString")
    {    
        if (ControlArguments(El,"I"))
        {
            retValue->iType = EX_STRING;
            *(retValue->sName) = OrdinalForm(VIn(El,0),0);
        }    
        else
        {
            PCC->sError="GetString- Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }   

    if((*MethodName)=="GetInteger")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iType = EX_INTEGER;
            retValue->iIntValue = atoi(VSt(El,0)->c_str());
        }    
        else
        {
            retValue->iType = EX_INTEGER;
            retValue->iIntValue = (-1);
        }
    }

    if((*MethodName)=="Modulo")
    {    
        if (ControlArguments(El,"II"))
        {
            retValue->iType = EX_INTEGER;
            retValue->iIntValue = VIn(El,0)%VIn(El,1);
        }    
        else
        {
            PCC->sError="Modulo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }


    //-------------------------------------------------------------------------------------------------------
    // Table
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="Table")
    {    
        if (ControlArguments(El,"SS") || ControlArguments(El,"SSS"))
        {
            CConfigFileReader* myCF;
            if (PCC->Tables.count(*(VSt(El,0)))==0)
            {
                myCF = new CConfigFileReader();
                myCF->SetLog(PCC->log,PCC->LogAll); 
                string Adr=*(PCC->Variables["TablesDirectory"]->sName) + *(VSt(El,0));             
                PCC->Tables.insert(make_pair(*(VSt(El,0)),myCF));
                if (myCF->ReadFile((char *) Adr.c_str(),&(myCF->VItems))==1)
                {
                    PCC->sError="Invalid path to Tables Directory";
                    retValue->iType=EX_ERROR;
                    return retValue;
                }
            }
            else
            {
                myCF = PCC->Tables[*(VSt(El,0))];
            }
            //tady dal - zkontrolovat
            if (myCF->VItems.count(*(VSt(El,1)))>0)
            {
                *(retValue->sName)=*(myCF->VItems[*(VSt(El,1))]->sName);
                retValue->iType=EX_STRING;
            }
            else
            {
                if (ControlArguments(El,"SSS")) 
                {
                    *(retValue->sName)=*(VSt(El,2));
                }
                else
                {
                    string OutE="";
                    for(int i=0;i<VSt(El,1)->length();i++) {
                        OutE+="X";
                    }
                    *(retValue->sName)=OutE;
                }
                retValue->iType=EX_STRING;
            }
        }    
        else
        {
            PCC->sError="Table - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }       


    //-------------------------------------------------------------------------------------------------------
    // SetMem
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="SetMem")
    {    
        if (ControlArguments(El,"SI") || ControlArguments(El,"SS"))
        {
            M_EXPRESSION_ELEMENT* Varb;
            if (PCC->Variables.count(*(VSt(El,0)))>0)
            {
                Varb = PCC->Variables[*(VSt(El,0))];        
            }
            else
            {
                Varb=new M_EXPRESSION_ELEMENT();
                PCC->Variables.insert(make_pair(string(*(VSt(El,0))),Varb));
                Varb->sName=new string("");
            }
            Varb->iType=VTy(El,1);
            if (VTy(El,1)==EX_INTEGER)
            {
                Varb->iIntValue=VIn(El,1);
            } 
            if (VTy(El,1)==EX_STRING)
            {
                //            (*log)<< "V: " << *(VSt(El,0));
                *(Varb->sName)=*(VSt(El,1));
            }    
        }    
        else
        {
            PCC->sError="SetMem - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }     

    //-------------------------------------------------------------------------------------------------------
    // IsSet
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="IsSet")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iType=EX_INTEGER;
            if (PCC->Variables.count(*(VSt(El,0)))>0)
                retValue->iIntValue=1;
            else
                retValue->iIntValue=0;     
        }
        else
        {
            PCC->sError="IsSet - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }    

    //-------------------------------------------------------------------------------------------------------
    // GetMem
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetMem")
    {    
        if (ControlArguments(El,"S"))
        {
            if (PCC->Variables.count(*(VSt(El,0)))>0)
            {
                retValue->iType=PCC->Variables[*(VSt(El,0))]->iType;
                *(retValue->sName)=*(PCC->Variables[*(VSt(El,0))]->sName);
                retValue->iIntValue=PCC->Variables[*(VSt(El,0))]->iIntValue;
            }
            else
            {
                retValue->iType=EX_INTEGER;
                retValue->iIntValue=0;
            }    
        }    
        else
        {
            PCC->sError="GetMem - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }     

    //-------------------------------------------------------------------------------------------------------
    // GetMarcInValue
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="GetMarcInValue")
    {    
        if (ControlArguments(El,"S"))
        {
            string * RetS = PCC->MarcIn->ValueByCommand(VSt(El,0));
            *(retValue->sName)= *RetS;
            retValue->iType=EX_STRING;
            delete RetS;
            if (PCC->MarcIn->g_Error!="")
            {
                PCC->sError=PCC->MarcIn->g_Error;
                retValue->iType=EX_ERROR;
                return retValue;
            }   
        }    
        else
        {
            PCC->sError="GetMarcInValue - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }     

    //-------------------------------------------------------------------------------------------------------
    // GetMarcOutValue
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetMarcOutValue")
    {    
        if (ControlArguments(El,"S"))
        {
            string * RetS = PCC->MarcOut->ValueByCommand(VSt(El,0));
            *(retValue->sName)= *RetS;
            retValue->iType=EX_STRING;
            delete RetS;
            if (PCC->MarcOut->g_Error!="")
            {
                PCC->sError=PCC->MarcOut->g_Error;
                retValue->iType=EX_ERROR;
                return retValue;
            }   
        }    
        else
        {
            PCC->sError="GetMarcOutValue - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }
    //-------------------------------------------------------------------------------------------------------
    // MaxO
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="MaxO")
    {    
        if (ControlArguments(El,"S"))
        {
            PCC->MarcOut->FillFieldsDepth();
            retValue->iIntValue = PCC->MarcOut->MaxIndexByCommand(VSt(El,0),0);
            retValue->iType=EX_INTEGER;
            if (PCC->MarcOut->g_Error!="")
            {
                PCC->sError=PCC->MarcOut->g_Error;
                retValue->iType=EX_ERROR;
                return retValue;
            }   
        }    
        else
        {
            PCC->sError="MaxO - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MaxI
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="MaxI")
    {    
        if (ControlArguments(El,"S"))
        {
            retValue->iIntValue = PCC->MarcIn->MaxIndexByCommand(VSt(El,0),0);
            retValue->iType=EX_INTEGER;
            if (PCC->MarcIn->g_Error!="")
            {
                PCC->sError=PCC->MarcIn->g_Error;
                retValue->iType=EX_ERROR;
                return retValue;
            }   
        }    
        else
        {
            PCC->sError="MaxI - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // NeibField
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="NeibField")
    {  
        if (ControlArguments(El,"I"))
        {   
            if ((PCC->lPointerField + VIn(El,0) > (-1)) && (PCC->lPointerField + VIn(El,0) < PCC->MarcIn->Fields.size()))
                *(retValue->sName) = *(PCC->MarcIn->Fields[PCC->lPointerField + VIn(El,0)]->ID);
            else
                *(retValue->sName) = "";
            retValue->iType = EX_STRING;
        }    
        else
        {
            PCC->sError="NeibField - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // NeibSubField
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="NeibSubField")
    {  
        if (ControlArguments(El,"I"))
        {   
            if (!(PCC->DoingLinking))
            {
                if ((PCC->lPointerField > (-1)) && (PCC->lPointerField < PCC->MarcIn->Fields.size()))
                {
                    int Ind = PCC->lPointerSubField + VIn(El,0); 
                    if ((Ind > (-1)) && (Ind < PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf.size()))
                        *(retValue->sName) = *(PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf[Ind]->ID);
                    else
                        *(retValue->sName) = "";
                }
                else
                {
                    *(retValue->sName) = "";
                }
            }    
            else
            {
                if ((PCC->lPointerField > (-1)) && (PCC->lPointerField < PCC->MarcIn->Fields.size()))
                {
                    int Ind = PCC->lPointerSubField; 
                    if ((Ind > (-1)) && (Ind < PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf.size()))
                    {
                        int Ind2 = PCC->lPointerLinkingField + VIn(El,0); 
                        if ((Ind2 > (-1)) && (Ind2< PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf[Ind]->LinkingField->ptrCollSubf.size()))
                            *(retValue->sName) =  *(PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf[Ind]->LinkingField->ptrCollSubf[Ind2]->ID);
                        else
                            *(retValue->sName) = "";
                    }
                    else
                    {
                        *(retValue->sName) = "";
                    }
                }
                else
                {
                    *(retValue->sName) = "";
                }
            }
            retValue->iType = EX_STRING;
        }    
        else
        {
            PCC->sError="NeibSubField - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // IsSubBefore
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="IsSubBefore")
    {  
        if (ControlArguments(El,"S"))
        {   
            if ((PCC->lPointerField > (-1)) && (PCC->lPointerField < PCC->MarcIn->Fields.size()))
            {
                int Ind = PCC->lPointerSubField; 
                if ((Ind > (-1)) && (Ind < PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf.size()))
                {      
                    retValue->iIntValue = 0;
                    for(int i=0;i<=Ind;i++)
                    {
                        if (*(PCC->MarcIn->Fields[PCC->lPointerField]->ptrCollSubf[i]->ID) == *(VSt(El,0)))
                            retValue->iIntValue = 1;
                    }
                }
                else
                {
                    retValue->iIntValue = 0;
                }
            }
            else
            {
                retValue->iIntValue = 0;
            }
            retValue->iType=EX_INTEGER;
        }    
        else
        {
            PCC->sError="IsSubBefore - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }



    //-------------------------------------------------------------------------------------------------------
    // SetValue
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="SetValue")
    {  
        if (ControlArguments(El,"SSIII"))
        {
            PCC->MarcOut->SetValueByCommand(VSt(El,0), VSt(El,1), VIn(El,2), VIn(El,3), VIn(El,4));
            if (PCC->MarcOut->g_Error!="")
            {
                PCC->sError=PCC->MarcOut->g_Error;
                retValue->iType=EX_ERROR;
                return retValue;         
            } 
            else
            {
                retValue->iType = EX_INTEGER;
                retValue->iIntValue = 0;
            }
        }
        else
        {
            PCC->sError="SetValue - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // Array
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="Array")
    { 
        if(El->size()>0)
        {
            if (VTy(El,0)==EX_STRING)
            {
                CConfigArray* myCF;
                if (PCC->Arrays.count(*(VSt(El,0)))>0)
                {
                    //smazání vsech prvku
                    // cout <<  "Pole již existuje: "  << *(VSt(El,0));
                    myCF = PCC->Arrays[*(VSt(El,0))];
                    for(int j=0;j<myCF->VItems.size();j++)
                        delete myCF->VItems[j];
                    myCF->VItems.clear();
                }
                else
                {
                    myCF = new CConfigArray();
                    PCC->Arrays.insert(make_pair(*(VSt(El,0)),myCF));  
                }
                for(int i=1;i<El->size();i++)
                {
                    M_EXPRESSION_ELEMENT* NewEl=new M_EXPRESSION_ELEMENT();
                    NewEl->sName=new string(*(VSt(El,i)));
                    NewEl->iIntValue=VIn(El,i);
                    NewEl->iType=VTy(El,i);
                    myCF->VItems.push_back(NewEl);
                }
            }
            else
            {   
                PCC->sError="Array - Invalid number of arguments";
                retValue->iType=EX_ERROR;
            }   
        }
        else
        {
            PCC->sError="Array- Invalid number of arguments";
            retValue->iType=EX_ERROR; 
        }   
    }      

    //-------------------------------------------------------------------------------------------------------
    // AddArray
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="AddArray")
    {  
        if(El->size()>0)
        {
            if (VTy(El,0)==EX_STRING)
            {
                if (PCC->Arrays.count(*(VSt(El,0)))>0)
                {
                    CConfigArray* myCF = PCC->Arrays[*(VSt(El,0))];
                    for(int i=1;i<El->size();i++)
                    {
                        M_EXPRESSION_ELEMENT* NewEl=new M_EXPRESSION_ELEMENT();
                        NewEl->sName=new string(*(VSt(El,i)));
                        NewEl->iIntValue=VIn(El,i);
                        NewEl->iType=VTy(El,i);
                        myCF->VItems.push_back(NewEl);
                    }
                }
                else
                {
                    PCC->sError="AddArray - Unknown array";      
                    retValue->iType=EX_ERROR;   
                }
            }
            else
            {   
                PCC->sError="AddArray - Invalid number of arguments";
                retValue->iType=EX_ERROR;
            }   
        }
        else
        {
            PCC->sError="AddArray- Invalid number of arguments";
            retValue->iType=EX_ERROR; 
        }   
    }


    //-------------------------------------------------------------------------------------------------------
    // GetArray
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetArray")
    {  
        if (ControlArguments(El,"SI"))
        {
            if (PCC->Arrays.count(*(VSt(El,0)))>0)
            {
                CConfigArray* myCF = PCC->Arrays[*(VSt(El,0))];
                if ((VIn(El,1)>0) && (VIn(El,1)<=myCF->VItems.size()))
                {
                    M_EXPRESSION_ELEMENT* GEl=myCF->VItems[VIn(El,1)-1];
                    retValue->iType=GEl->iType;
                    retValue->iIntValue=GEl->iIntValue;
                    *(retValue->sName)=*(GEl->sName);
                }
                else
                {
                    PCC->sError="GetArray - Invalid index"; 
                    retValue->iType=EX_ERROR; 
                }     
            }
            else
            {
                PCC->sError="GetArray - Unknown array";      
                retValue->iType=EX_ERROR;   
            }
        }
        else
        {
            PCC->sError=" GetArray - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // CountArray
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="CountArray")
    {  
        if (ControlArguments(El,"S"))
        {
            if (PCC->Arrays.count(*(VSt(El,0)))>0)
            {
                CConfigArray* myCF = PCC->Arrays[*(VSt(El,0))];
                retValue->iType=EX_INTEGER;
                retValue->iIntValue=myCF->VItems.size();
            }
            else
            {
                PCC->sError="CountArray - Unknown array";     
                retValue->iType=EX_ERROR;            
            }     
        }
        else
        {
            PCC->sError="CountArray - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // SplitByChars
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="SplitByChars")
    {  
        if (ControlArguments(El,"SSSSS"))
        {
            string SplitedS = *(VSt(El,0));
            string NowCharSplited;
            int Pointer = 0;
            int Min=0;

            CConfigArray* myNA; //NameArray
            CConfigArray* myNAD; //NameArrayDelim
            CConfigArray* myAD; //ArrayDelimiters

            //NAjdu prislusne pole nebo ho vytvorim

            if (PCC->Arrays.count(*(VSt(El,2)))>0)
            {
                myNA = PCC->Arrays[*(VSt(El,2))]; 
                for(int j=0;j<myNA->VItems.size();j++)
                    delete myNA->VItems[j];                 
                myNA->VItems.clear();   
            }
            else
            {
                myNA = new CConfigArray();
                PCC->Arrays.insert(make_pair(*(VSt(El,2)),myNA));  
            }
            //NAjdu prislusne pole nebo ho vytvorim
            if (PCC->Arrays.count(*(VSt(El,3)))>0)
            {
                myNAD = PCC->Arrays[*(VSt(El,3))];                  
                for(int j=0;j<myNAD->VItems.size();j++)
                    delete myNAD->VItems[j]; 
                myNAD->VItems.clear();   
            }
            else
            {
                myNAD = new CConfigArray();
                PCC->Arrays.insert(make_pair(*(VSt(El,3)),myNAD));  
            }
            //NAjdu prislusne pole 
            if (PCC->Arrays.count(*(VSt(El,1)))>0)
            {
                myAD = PCC->Arrays[*(VSt(El,1))];                   
            }
            else
            {
                PCC->sError="SplitByChars - Unknown array";     
                retValue->iType=EX_ERROR;
                return retValue;
            }
            if (*(VSt(El,4))=="BYALL")
            {             
                while(Min!=100000)
                {
                    Min = 100000;
                    NowCharSplited = "";
                    //Najdu znak ktery je od pozice poiter nejblíže
                    for(int i=0;i<myAD->VItems.size();i++)
                    {
                        int NowN = SplitedS.find(myAD->VItems[i]->sName->c_str(), Pointer);
                        if((NowN < Min) && (NowN!=(-1)))
                        {
                            Min = NowN;
                            NowCharSplited = *(myAD->VItems[i]->sName);
                        }
                    }
                    if(Min!=100000)
                    {
                        M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                        NewEl->sName=new string("");
                        *(NewEl->sName)=SplitedS.substr(Pointer, Min - Pointer);
                        NewEl->iType=EX_STRING;
                        myNA->VItems.push_back(NewEl);

                        M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                        NewEl2->sName=new string("");
                        *(NewEl2->sName)=NowCharSplited;
                        NewEl2->iType=EX_STRING;
                        myNAD->VItems.push_back(NewEl2);

                        Pointer = Min + NowCharSplited.length();
                    }
                }

                M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                NewEl->sName=new string("");
                *(NewEl->sName)=SplitedS.substr(Pointer);
                NewEl->iType=EX_STRING;
                myNA->VItems.push_back(NewEl);

                M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                NewEl2->sName=new string("");
                *(NewEl2->sName)="";
                NewEl2->iType=EX_STRING;
                myNAD->VItems.push_back(NewEl2);

                retValue->iType=EX_INTEGER;
                retValue->iIntValue=1;
            }
            if (*(VSt(El,4))=="BYORDER")
            {                     
                //cout << "Zacatek pole Separators velikosti " << myAD->VItems.size() << "\n";   
                for(int i=0;i<myAD->VItems.size();i++)
                {
                    int NowN = SplitedS.find(myAD->VItems[i]->sName->c_str(), Pointer);
                    //cout << "Finding char\""<< myAD->VItems[i]->sName->c_str() << "\" Nalezeno na pozici: " << NowN << " Pointer:" << Pointer << "\n";
                    if(NowN!=(-1))
                    {
                        NowCharSplited = *(myAD->VItems[i]->sName);

                        M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                        NewEl->sName=new string("");

                        *(NewEl->sName)=SplitedS.substr(Pointer, NowN - Pointer);
                        NewEl->iType=EX_STRING;
                        myNA->VItems.push_back(NewEl);

                        M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                        NewEl2->sName=new string("");
                        *(NewEl2->sName)=NowCharSplited;
                        NewEl2->iType=EX_STRING;
                        myNAD->VItems.push_back(NewEl2);

                        Pointer = NowN + NowCharSplited.length();
                    }
                }

                M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                NewEl->sName=new string("");
                *(NewEl->sName)=SplitedS.substr(Pointer);
                NewEl->iType=EX_STRING;
                myNA->VItems.push_back(NewEl);

                M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                NewEl2->sName=new string("");
                *(NewEl2->sName)="";
                NewEl2->iType=EX_STRING;
                myNAD->VItems.push_back(NewEl2);

                retValue->iType=EX_INTEGER;
                retValue->iIntValue=1;
            }
            if (*(VSt(El,4))=="BYORDERDIRECT")
            {     
                for(int i=0;i<myAD->VItems.size();i++)
                {
                    int NowN = SplitedS.find(myAD->VItems[i]->sName->c_str(), Pointer);
                    if(NowN!=(-1))
                    {
                        NowCharSplited = *(myAD->VItems[i]->sName);

                        M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                        NewEl->sName=new string("");
                        *(NewEl->sName)=SplitedS.substr(Pointer, NowN - Pointer);
                        NewEl->iType=EX_STRING;
                        myNA->VItems.push_back(NewEl);

                        M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                        NewEl2->sName=new string("");
                        *(NewEl2->sName)=NowCharSplited;
                        NewEl2->iType=EX_STRING;
                        myNAD->VItems.push_back(NewEl2);

                        Pointer = NowN + NowCharSplited.length();
                    }
                    else
                    {
                        PCC->sError="SplitByChars BYORDERDIRECT - mismash interpunction";
                        retValue->iType=EX_ERROR;
                        return retValue;
                    }
                }
                M_EXPRESSION_ELEMENT * NewEl=new M_EXPRESSION_ELEMENT();
                NewEl->sName=new string("");
                *(NewEl->sName)=SplitedS.substr(Pointer);
                NewEl->iType=EX_STRING;
                myNA->VItems.push_back(NewEl);

                M_EXPRESSION_ELEMENT * NewEl2=new M_EXPRESSION_ELEMENT();
                NewEl2->sName=new string("");
                *(NewEl2->sName)="";
                NewEl2->iType=EX_STRING;
                myNAD->VItems.push_back(NewEl2);
            }
        }
        else
        {
            PCC->sError="SplitByChars - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcFieldsCount
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="MarcFieldsCount")
    {  
        if (ControlArguments(El,"S"))
        {   
            if (*(VSt(El,0))=="I")
                retValue->iIntValue = PCC->MarcIn->Fields.size();
            else
                retValue->iIntValue = PCC->MarcOut->Fields.size();
            retValue->iType = EX_INTEGER;
        }    
        else
        {
            PCC->sError="MarcFieldsCount - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcGetFieldInfo
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="MarcGetFieldInfo")
    {  
        if (ControlArguments(El,"SIS"))
        { 
            if (*(VSt(El,0))=="I")
            {
                if((VIn(El,1)>(-1)) && (VIn(El,1) < PCC->MarcIn->Fields.size()))
                {
                    if (*(VSt(El,2))=="ID") 
                    {
                        *(retValue->sName)= *(PCC->MarcIn->Fields[VIn(El,1)]->ID);
                        retValue->iType = EX_STRING;
                    }
                    if (*(VSt(El,2))=="I1") 
                    {
                        *(retValue->sName)= *(PCC->MarcIn->Fields[VIn(El,1)]->ID1);
                        retValue->iType = EX_STRING;
                    }            
                    if (*(VSt(El,2))=="I2") 
                    {
                        *(retValue->sName)= *(PCC->MarcIn->Fields[VIn(El,1)]->ID2);
                        retValue->iType = EX_STRING;
                    }            
                    if (*(VSt(El,2))=="CountSub") 
                    {
                        retValue->iIntValue= PCC->MarcIn->Fields[VIn(El,1)]->ptrCollSubf.size();
                        retValue->iType = EX_INTEGER;
                    }   
                    if (*(VSt(El,2))=="Index") 
                    {
                        retValue->iIntValue= PCC->MarcIn->Fields[VIn(El,1)]->Index;
                        retValue->iType = EX_INTEGER;
                    }         
                }
            }
            else
            {
                if((VIn(El,1)>(-1)) && (VIn(El,1) < PCC->MarcOut->Fields.size()))
                {
                    if (*(VSt(El,2))=="ID") 
                    {
                        *(retValue->sName)= *(PCC->MarcOut->Fields[VIn(El,1)]->ID);
                        retValue->iType = EX_STRING;
                    }
                    if (*(VSt(El,2))=="I1") 
                    {
                        *(retValue->sName)= *(PCC->MarcOut->Fields[VIn(El,1)]->ID1);
                        retValue->iType = EX_STRING;
                    }            
                    if (*(VSt(El,2))=="I2") 
                    {
                        *(retValue->sName)= *(PCC->MarcOut->Fields[VIn(El,1)]->ID2);
                        retValue->iType = EX_STRING;
                    }            
                    if (*(VSt(El,2))=="CountSub") 
                    {
                        retValue->iIntValue= PCC->MarcOut->Fields[VIn(El,1)]->ptrCollSubf.size();
                        retValue->iType = EX_INTEGER;
                    }         
                    if (*(VSt(El,2))=="Index") 
                    {
                        retValue->iIntValue= PCC->MarcOut->Fields[VIn(El,1)]->Index;
                        retValue->iType = EX_INTEGER;
                    }   
                }       
            }   
        } 
        else
        {
            PCC->sError="MarcGetFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcSetFieldInfo
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="MarcSetFieldInfo")
    {  
        if (ControlArguments(El,"SIS"))
        { 
            if (*(VSt(El,0))=="I")
            {
                if((VIn(El,1)>(-1)) && (VIn(El,1) < PCC->MarcIn->Fields.size()))
                {
                    if (*(VSt(El,2))=="ID")  *(PCC->MarcIn->Fields[VIn(El,1)]->ID) = *(VSt(El,3));
                    if (*(VSt(El,2))=="I1")  *(PCC->MarcIn->Fields[VIn(El,1)]->ID1) = *(VSt(El,3));
                    if (*(VSt(El,2))=="I2")  *(PCC->MarcIn->Fields[VIn(El,1)]->ID2) = *(VSt(El,3));
                }
            }
            else
            {   
                if((VIn(El,1)>(-1)) && (VIn(El,1) < PCC->MarcOut->Fields.size()))
                {
                    if (*(VSt(El,2))=="ID")  *(PCC->MarcOut->Fields[VIn(El,1)]->ID) = *(VSt(El,3));
                    if (*(VSt(El,2))=="I1")  *(PCC->MarcOut->Fields[VIn(El,1)]->ID1) = *(VSt(El,3));
                    if (*(VSt(El,2))=="I2")  *(PCC->MarcOut->Fields[VIn(El,1)]->ID2) = *(VSt(El,3));
                }
            }   
        } 
        else
        {
            PCC->sError="MarcSetFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcGetSubFieldInfo
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="MarcGetSubFieldInfo")
    {     
        if (ControlArguments(El,"SIIS"))
        { 
            int NumField = VIn(El,1);
            int NumSubField = VIn(El,2);
            string Argument = *(VSt(El,3));
            if (*(VSt(El,0))=="I")
            {
                if((NumField>(-1)) && (NumField < PCC->MarcIn->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcIn->Fields[NumField]->ptrCollSubf.size()))
                    {
                        if (Argument == "ID")
                        {
                            *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->ID);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "Value")
                        {
                            *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->Value);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "Index")
                        {
                            retValue->iIntValue = PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->Index;
                            retValue->iType=EX_INTEGER;
                        }
                        if (Argument == "LinkingFieldID")
                        {
                            *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldI1")
                        {
                            *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID1);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldI2")
                        {
                            *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID2);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldCount")
                        {
                            retValue->iIntValue = PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size();
                            retValue->iType=EX_INTEGER;
                        }               
                    }
                }
            }
            else
            { 
                if((NumField>(-1)) && (NumField < PCC->MarcOut->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcOut->Fields[NumField]->ptrCollSubf.size()))
                    {
                        if (Argument == "ID")
                        {
                            *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->ID);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "Value")
                        {
                            *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->Value);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "Index")
                        {
                            retValue->iIntValue = PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->Index;
                            retValue->iType=EX_INTEGER;
                        }                        
                        if (Argument == "LinkingFieldID")
                        {
                            *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldI1")
                        {
                            *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID1);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldI2")
                        {
                            *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID2);
                            retValue->iType=EX_STRING;
                        }
                        if (Argument == "LinkingFieldCount")
                        {
                            retValue->iIntValue = PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size();
                            retValue->iType=EX_INTEGER;
                        }               
                    }
                }
            }   
        } 
        else
        {
            PCC->sError="MarcGetSubFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcSetSubFieldInfo
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="MarcSetSubFieldInfo")
    {  
        if (ControlArguments(El,"SIISS"))
        { 
            int NumField = VIn(El,1);
            int NumSubField = VIn(El,2);
            string Argument = *(VSt(El,3));
            if (*(VSt(El,0))=="I")
            {
                if((NumField>(-1)) && (NumField < PCC->MarcIn->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcIn->Fields[NumField]->ptrCollSubf.size()))
                    {           
                        if (Argument=="ID") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->ID) = *(VSt(El,4));
                        if (Argument=="Value") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->Value) = *(VSt(El,4));
                        if (Argument=="LinkingFieldID") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID) = *(VSt(El,4));
                        if (Argument=="LinkingFieldI1") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID1) = *(VSt(El,4));
                        if (Argument=="LinkingFieldI2") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID2) = *(VSt(El,4));
                    }
                } 
            }    
            else
            {
                if((NumField>(-1)) && (NumField < PCC->MarcOut->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcOut->Fields[NumField]->ptrCollSubf.size()))
                    {           
                        if (Argument=="ID") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->ID) = *(VSt(El,4));
                        if (Argument=="Value") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->Value) = *(VSt(El,4));
                        if (Argument=="LinkingFieldID") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID) = *(VSt(El,4));
                        if (Argument=="LinkingFieldI1") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID1) = *(VSt(El,4));
                        if (Argument=="LinkingFieldI2") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ID2) = *(VSt(El,4));
                    }
                } 
            }  
        } 
        else
        {
            PCC->sError="MarcSetSubFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // MarcGetLinkingFieldInfo
    //-------------------------------------------------------------------------------------------------------

    if((*MethodName)=="MarcGetLinkingSubFieldInfo")
    {  
        if (ControlArguments(El,"SIIIS"))
        { 
            int NumField = VIn(El,1);
            int NumSubField = VIn(El,2);
            int NumLinkSubField = VIn(El,3);
            string Argument = *(VSt(El,4));
            if (*(VSt(El,0))=="I")
            {
                if((NumField>(-1)) && (NumField < PCC->MarcIn->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcIn->Fields[NumField]->ptrCollSubf.size()))
                    {  
                        if((NumLinkSubField>(-1)) && (NumLinkSubField<PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size()))
                        {
                            if (Argument == "ID")
                            {
                                *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->ID);
                                retValue->iType=EX_STRING;
                            }
                            if (Argument == "Value")
                            {
                                *(retValue->sName) = *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->Value);
                                retValue->iType=EX_STRING;
                            }
                        }
                    }                            
                }
            }   
            else
            {
                if((NumField>(-1)) && (NumField < PCC->MarcOut->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcOut->Fields[NumField]->ptrCollSubf.size()))
                    {  
                        if((NumLinkSubField>(-1)) && (NumLinkSubField<PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size()))
                        {
                            if (Argument == "ID")
                            {
                                *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->ID);
                                retValue->iType=EX_STRING;
                            }
                            if (Argument == "Value")
                            {
                                *(retValue->sName) = *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->Value);
                                retValue->iType=EX_STRING;
                            }
                        }
                    }                            
                }
            }   
        } 
        else
        {
            PCC->sError="MarcGetLinkingSubFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // "MarcSetLinkingSubFieldInfo"
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="MarcSetLinkingSubFieldInfo")
    {  
        if (ControlArguments(El,"SIIISS"))
        { 
            int NumField = VIn(El,1);
            int NumSubField = VIn(El,2);
            int NumLinkSubField = VIn(El,3);
            string Argument = *(VSt(El,4));
            if (*(VSt(El,0))=="I")
            {
                if((NumField>(-1)) && (NumField < PCC->MarcIn->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcIn->Fields[NumField]->ptrCollSubf.size()))
                    {  
                        if((NumLinkSubField>(-1)) && (NumLinkSubField<PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size()))
                        {
                            if (Argument=="ID") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->ID) = *(VSt(El,4));
                            if (Argument=="Value") *(PCC->MarcIn->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->Value) = *(VSt(El,4));
                        }
                    }                            
                }
            }   
            else
            {
                if((NumField>(-1)) && (NumField < PCC->MarcOut->Fields.size()))
                {
                    if((NumSubField>(-1)) && (NumSubField < PCC->MarcOut->Fields[NumField]->ptrCollSubf.size()))
                    {  
                        if((NumLinkSubField>(-1)) && (NumLinkSubField<PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf.size()))
                        {
                            if (Argument=="ID") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->ID) = *(VSt(El,4));
                            if (Argument=="Value") *(PCC->MarcOut->Fields[NumField]->ptrCollSubf[NumSubField]->LinkingField->ptrCollSubf[NumLinkSubField]->Value) = *(VSt(El,4));
                        }
                    }                            
                }      
            }
        } 
        else
        {
            PCC->sError="MarcSetLinkingSubFieldInfo - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    //CopyField
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="CopyField")
    {  
        if (ControlArguments(El,"I"))
        { 
            if((VIn(El,0)>(-1)) && (VIn(El,0) < PCC->MarcIn->Fields.size()))
            {
                CField* ActF=PCC->MarcIn->Fields[VIn(El,0)];
                CField* NewField=new CField();
                PCC->MarcOut->CloneField(NewField,ActF);
                PCC->MarcOut->Fields.push_back(NewField); 
                retValue->iType=EX_INTEGER;
            }
            retValue->iType=EX_INTEGER;
        } 
        else
        {
            PCC->sError="CopyField - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    }


    //-------------------------------------------------------------------------------------------------------
    // GetLabel
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetLabel")
    {  
        if (ControlArguments(El,"S"))
        {
            if (*(VSt(El,0))=="I")
            {
                *(retValue->sName) = *(PCC->MarcIn->g_Label); 
                retValue->iType=EX_STRING;
            }
            else
            {
                *(retValue->sName) = *(PCC->MarcOut->g_Label); 
                retValue->iType=EX_STRING; 
            }
        }
        else
        {
            PCC->sError="GetLabel - Invalid number of arguments"; 
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // GetRecord
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="GetRecord")
    {  
        *(retValue->sName) = *(PCC->MarcOut->GetSerializeRecord(LOAD_TEXT_FORMAT)); 
        retValue->iType=EX_STRING;
    }

    //-------------------------------------------------------------------------------------------------------
    // BreakError
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="BreakError")
    {  
        if (ControlArguments(El,"S"))
        { 
            PCC->sError=*(VSt(El,0));
            retValue->iType=EX_ERROR;
        } else {
            PCC->sError="BreakError - Invalid number of arguments";   
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // ContinueError
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="ContinueError")
    {  
        if (ControlArguments(El,"S"))
        { 
            retValue->iType=EX_INTEGER;
            retValue->iIntValue = 0;
            *(PCC->log)<< *(VSt(El,0)) << "\n";
            PCC->sError="RULES ERROR";
        } else {
            PCC->sError="ContinueError - Invalid number of arguments";   
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // ClearFile
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="ClearFile")
    {  
        FILE *fwfc;
        if (ControlArguments(El,"S"))
        {
            string sOutF = *(VSt(El,0));	   
            if ((fwfc=fopen(sOutF.c_str() ,"wb"))== NULL) { 
                PCC->sError="ClearFile - Unable to open file '"+*(VSt(El,0))+"' for writing";   
                retValue->iType=EX_ERROR;
            } else {
                fclose(fwfc);
                retValue->iType=EX_INTEGER;
                retValue->iIntValue = 0;
            }
        }
        else
        {
            PCC->sError="ClearFile - Invalid number of arguments";   
            retValue->iType=EX_ERROR;
        }       
    }

    //-------------------------------------------------------------------------------------------------------
    // WriteOutRecordToFile
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="WriteOutRecordToFile")
    {  
        FILE *fwfc;
        if (ControlArguments(El,"SS"))
        {
            string *sRecord = 0;
            int incorrectFormat = 0;
            if (*(VSt(El,1))=="text") {
                sRecord = PCC->MarcOut->GetSerializeRecord(LOAD_TEXT_FORMAT); 
            }
            else if (*(VSt(El,1))=="aleph") {
                sRecord = PCC->MarcOut->GetSerializeRecord(LOAD_ALEPH_FORMAT); 
            }
            else if (*(VSt(El,1))=="iso") {
                sRecord = PCC->MarcOut->GetSerializeRecord(LOAD_ISO_FORMAT); 
            }
            else {
                PCC->sError="WriteOutRecordToFile - Unable to open file '"+*(VSt(El,0))+"' for writing";   
                retValue->iType=EX_ERROR;
                incorrectFormat = 1;
            }

            if(!incorrectFormat) {
                string sOutF = *(VSt(El,0));	   
                if ((fwfc=fopen(sOutF.c_str() ,"ab"))== NULL) { 
                    PCC->sError="WriteOutRecordToFile - Unable to open file '"+*(VSt(El,0))+"' for writing";   
                    retValue->iType=EX_ERROR;
                } else {
                    fwrite(sRecord->c_str(),1,sRecord->length(),fwfc);                  
                    fflush(fwfc);
                    fclose(fwfc);
                    retValue->iType=EX_INTEGER;
                    retValue->iIntValue = 0;
                }
                delete sRecord;
            }
        }
        else
        {
            PCC->sError="WriteRecordToFile - Invalid number of arguments";   
            retValue->iType=EX_ERROR;
        }       
    }

    //-------------------------------------------------------------------------------------------------------
    // SetLabel
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="SetLabel")
    {  
        if (ControlArguments(El,"SS"))
        {
            if (*(VSt(El,0))=="I")
            {
                *(PCC->MarcIn->g_Label) = *(VSt(El,1));
                retValue->iType=EX_INTEGER;
            }
            else
            {
                *(PCC->MarcOut->g_Label) = *(VSt(El,1));
                retValue->iType=EX_INTEGER; 
            }
        }
        else
        {
            PCC->sError="SetLabel - Invalid number of arguments";   
            retValue->iType=EX_ERROR;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    // SortSubfields
    //-------------------------------------------------------------------------------------------------------
    if((*MethodName)=="SortSubfields")
    {  
        int Search = 0;
        if (ControlArguments(El,"SIS") || ControlArguments(El,"SSS"))
        {
            if (ControlArguments(El,"SIS"))
            { 
                Search = 1;
                if (*(VSt(El,0))=="I")
                {
                    int NumField = VIn(El,1);
                    if ((NumField > (-1)) && (NumField < PCC->MarcIn->Fields.size()))
                    {           
                        if (!PCC->SortSubfields(&(PCC->MarcIn->Fields[NumField]->ptrCollSubf), VSt(El,2)))
                        {
                            retValue->iType=EX_INTEGER;
                            retValue->iIntValue = 1;
                        }
                        else
                        {
                            PCC->sError="SortSubfields - Unknown Error"; 
                            retValue->iType=EX_ERROR;
                        }
                    }
                }   
                else
                {
                    int NumField = VIn(El,1);
                    if ((NumField > (-1)) && (NumField < PCC->MarcOut->Fields.size()))
                    {
                        if (!PCC->SortSubfields(&(PCC->MarcOut->Fields[NumField]->ptrCollSubf), VSt(El,2)))
                        {
                            retValue->iType=EX_INTEGER;
                            retValue->iIntValue = 1;
                        }
                        else
                        {
                            PCC->sError="SortSubfields - Unknown Error"; 
                            retValue->iType=EX_ERROR;
                        }
                    }
                }
            }
            else
            { 
                Search = 1;
                if (*(VSt(El,0))=="I")
                {
                    string IDField = *(VSt(El,1));
                    for(int i=0;i<PCC->MarcIn->Fields.size();i++)
                    {
                        if(*(PCC->MarcIn->Fields[i]->ID) == IDField)
                        {
                            if (!PCC->SortSubfields(&(PCC->MarcIn->Fields[i]->ptrCollSubf), VSt(El,2)))
                            {
                                retValue->iType=EX_INTEGER;
                                retValue->iIntValue = 1;
                            }
                            else
                            {
                                PCC->sError="SortSubfields - Unknown Error"; 
                                retValue->iType=EX_ERROR;
                            }
                        }
                    }
                }   
                else
                {
                    string IDField = *(VSt(El,1));
                    for(int i=0;i<PCC->MarcOut->Fields.size();i++)
                    {
                        if(*(PCC->MarcIn->Fields[i]->ID) == IDField)
                        {
                            if (!PCC->SortSubfields(&(PCC->MarcOut->Fields[i]->ptrCollSubf), VSt(El,2)))
                            {
                                retValue->iType=EX_INTEGER;
                                retValue->iIntValue = 1;
                            }
                            else
                            {
                                PCC->sError="SortSubfields - Unknown Error"; 
                                retValue->iType=EX_ERROR;
                            }
                        }
                    }
                }
                /*    Case Else
                      Arguments.Item(0).iType = TYPE_ERROR
                      End Select*/
            }
        } 
        else
        {
            PCC->sError="SortSubfields - Invalid number of arguments";
            retValue->iType=EX_ERROR;
        }
    } 

exitCallFunc:

    if (PCC->LogAll) { //Debug
        *(PCC->log)<< "---------------------------------------------\n";
        *(PCC->log)<< "END: CallFunc\n";
        *(PCC->log)<< "RETURN: ";
        if (retValue->iType==EX_INTEGER)
            *(PCC->log)<< "(INT) " << retValue->iIntValue;
        if (retValue->iType==EX_STRING)
            *(PCC->log)<< "(STR) " << *(retValue->sName);    
        if (retValue->iType==EX_ERROR)
            *(PCC->log)<< "EX_ERROR";   
        *(PCC->log)<< "\n";

        *(PCC->log)<< "---------------------------------------------\n";
    } //End Debug

    return retValue;    
}


int CConvertor::SortSubfields(vector<CSubfield*>* Coll, string* Command)
{
    int BOpen=0;
    int PointerB = 0;
    int PointerType = 0;
    vector<string *> St;
    vector<string *> D;
    vector<int> DI;
    int Pointer;
    int h;
    int t;
    int i;

    //Ziskani vyrazu
    // (*log)<< "SortSubfields: " << *Command << "\n";
    for(i=0;i<Command->length();i++)
    {
        string MChar = Command->substr(i, 1);
        // (*log)<< MChar << "\n";
        if(MChar == "[")
        {
            if(BOpen)
            {
                return 0;
            }
            BOpen = 1;
            PointerB = i;
            PointerType = 1;
        }
        if(MChar == "(")
        {
            if(BOpen)
            {
                return 0;
            }
            BOpen = 1;
            PointerB = i;
            PointerType = 2;
        }
        if(MChar == "]")
        {
            if ((!BOpen) || (PointerType == 2)) 
            {
                return 0;
            }
            BOpen = 0;
            if (PointerB < Command->size()) 
            {
                //(*log)<< "PUSHBACK: " << Command->substr(PointerB + 1, i - PointerB - 1) << "\n"; 
                D.push_back(new string(Command->substr(PointerB + 1, i - PointerB - 1)));
                DI.push_back(2);
            }   
            PointerType = 0;
        }
        if(MChar == ")")
        {
            if ((!BOpen) || (PointerType == 1)) 
            {
                return 0;
            }           
            BOpen = 0;
            if (PointerB < Command->size()) 
            {
                //(*log)<< "PUSHBACK: " << Command->substr(PointerB + 1, i - PointerB - 1) << "\n"; 
                D.push_back(new string(Command->substr(PointerB + 1, i - PointerB - 1)));
                DI.push_back(1);
            }   
            PointerType = 0;
        }
    }
    Pointer = 0;
    for(i=0;i<D.size();i++)
    {
        //oddelit carkovane hodnoty
        SplitString(D[i],&St,",");
        //(*log)<< "SP:" << *(D[i]) << "\n";
        if (DI[i] == 2)
        {
            //Projizdim podpole
            for(h=Pointer;h<Coll->size();h++)
            {
                if (IsInS(&St, (*Coll)[h])) 
                {
                    //(*log)<< "ChangePossition:" << *((*Coll)[h]->ID) << ":" << Pointer << "x" << h << "\n";
                    ChangePossition(Coll,Pointer,h);
                    Pointer++;
                    if (Pointer==Coll->size()) break;
                }   
            }
        }
        else
        {
            //Projizdim Pravidla
            for(h=0;h<St.size();h++)
            {
                for(t=Pointer;t<Coll->size();t++)
                {
                    if (St[h]->substr(0, 1) == "L")
                    {
                        if (*((*Coll)[t]->LinkingField->ID) == St[h]->substr(1))
                        {
                            ChangePossition(Coll,Pointer,t);
                            Pointer++;
                            if (Pointer==Coll->size()) break;
                        }
                    }
                    else
                    { 
                        if (*((*Coll)[t]->ID) == *(St[h]))
                        {
                            //(*log)<< "ChangePossition:" << Pointer << "x" << t << "\n";                        
                            ChangePossition(Coll,Pointer,t);
                            Pointer++;
                            if (Pointer==Coll->size()) break;
                        }
                    }
                }
            }
        }
        for(int u=0;u<St.size();u++)
            delete St[u];
        St.clear();
    }
    for(int q=0;q<D.size();q++)
        delete D[q];
    D.clear();   
    return 0;
}


int CConvertor::IsInS(vector<string*>* St, CSubfield* SubF)
{
    for(int i=0;i<St->size();i++)
    {
        if ((*St)[i]->substr(0, 1) == "L")
        {
            if ((*St)[i]->substr(1) == *(SubF->LinkingField->ID))
                return 1;
        }
        else
        {
            if ((*St)[i]->substr(0,1) == *(SubF->ID))
                return 1;
        }
    }
    return 0;
}


int CConvertor::ChangePossition(vector<CSubfield*>* Coll, int Poss1, int Poss2)
{
    CSubfield *PossSecond;
    PossSecond = (*Coll)[Poss2];
    
    for ( int i = Poss2 ; i > Poss1 ; i-- ) {
        (*Coll)[i] = (*Coll)[i-1];       
    }
    
    (*Coll)[Poss1] = PossSecond;
    
    return 0;
}



void CConvertor::TrimLeft(string* sStr, char* ArraySep)
{
    int finds=1;
    int i;
    while(finds==1)
    {
        finds=0;
        for(i=0;i<strlen(ArraySep);i++)
        {
            if (sStr->size()>0)
            {
                if ((*sStr)[0]==ArraySep[i])
                {
                    finds=1;
                    sStr->erase(0,1);
                }
            }
        }
    }
}

void CConvertor::TrimRight(string* sStr, char* ArraySep)
{
    int finds=1;
    int i;
    while(finds==1)
    {
        finds=0;
        for(i=0;i<strlen(ArraySep);i++)
        {
            if (sStr->size()>0)
            {
                if ((*sStr)[sStr->size()-1]==ArraySep[i])
                {
                    finds=1;
                    sStr->erase(sStr->size()-1,1);
                }
            }
        }
    }
}

int CConvertor::SplitString(string* strSplit, vector<string*>* outArray, char* Separator)
{
    int iPos = 0;
    int newPos = -1;
    string sSeparator=Separator;
    int sizeS2 = sSeparator.size();
    int isize = strSplit->size();

    vector<int> positions;

    newPos = strSplit->find(Separator, 0);

    if(newPos==(-1)) 
    {
        outArray->push_back(new string(strSplit->substr(0)));
        return 0;
    }

    while(newPos!=(-1))
    {
        positions.push_back(newPos);
        iPos = newPos;
        newPos = strSplit->find (Separator, iPos+sizeS2+1);
    }

    for(int i=0;i<=positions.size();i++)
    {
        string s;
        if(i==0) 
        { 
            s = strSplit->substr(0, positions[i]);
        }
        else
        {
            int offset = positions[i-1] + sizeS2;
            if(offset<isize)
            {
                if(i==positions.size())
                {
                    s=strSplit->substr(offset);
                }
                else 
                {
                    s=strSplit->substr(positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2);
                }
            }
        }
        outArray->push_back(new string(s));
    }
    return 0;
}

int CodeString(string * sRet)
{
    for(int i=0;i<sRet->length();i++)
    {
        char Ret=(*sRet)[i];
        Ret=Ret^255;
        (*sRet)[i]=Ret;
    }
    return 0;
}

int CConvertor::SaveCodeRulesFile(char *sPath)
{
    if (codeVersion!=VERSION_ADMIN)
        return 1;

    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConvertor::SaveCodeRulesFile\n";
        (*log)<< "sPath:" << sPath << "\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    FILE *fw;
    if ((fw=fopen(sPath,"wb"))== NULL)
    {
        return 1;
    }
    //_setmode( _fileno(fw), _O_BINARY );
    RulesAnalyzer->OpenFromFile(Variables[KEYWORD_RULESFILE]->sName->c_str());

    if(RulesAnalyzer->g_Error!="")
    {
        sError = RulesAnalyzer->g_Error;
        return 1;
    }

    for (int i=0;i<RulesAnalyzer->CollRules.size();i++)
    {
        string* OutCommand;
        string NRule=RulesAnalyzer->CollRules[i]->Command;
        TrimLeft(&NRule,CUTTRIMED);
        TrimRight(&NRule,CUTTRIMED);
        if( NRule!="")
        {
            //Naplneni vsech skriptu
            ActualRule = i;
            CUseScript *NewScript=new CUseScript();
            //NewScript->SetLog(log,0); 
            if (NewScript->SetCode(&(RulesAnalyzer->CollRules[i]->Command),1))
            {
                sError = "Rule " + OrdinalForm(i,5)+ ":" + RulesAnalyzer->CollRules[i]->From + "->" + RulesAnalyzer->CollRules[i]->To + NewScript->GetStringError();
                return 1;       
            }
            OutCommand = NewScript->SerializeProgNodes();
        }
        else
        { 
            OutCommand=new string(" ");
        }

        if (LogAll) { //Debug
            (*log)<< "OutCommand:" << *OutCommand << "\n";
            (*log)<< "From:\"" << RulesAnalyzer->CollRules[i]->From << "\n";
            (*log)<< "To:\"" << RulesAnalyzer->CollRules[i]->From << "\n";
        } //End Debug

        CodeString(OutCommand);
        CodeString(&(RulesAnalyzer->CollRules[i]->From));
        CodeString(&(RulesAnalyzer->CollRules[i]->To));

        if (LogAll) { //Debug
            (*log)<< "OutCommand:" << *OutCommand << "\n";
            (*log)<< "From:\"" << RulesAnalyzer->CollRules[i]->From << "\n";
            (*log)<< "To:\"" << RulesAnalyzer->CollRules[i]->From << "\n";
        } //End Debug

        int SizeComm=OutCommand->length(); 
        int SizeFrom=RulesAnalyzer->CollRules[i]->From.length();
        int SizeTo  =RulesAnalyzer->CollRules[i]->To.length();
        int SizeAll =SizeComm+SizeFrom+SizeTo;


        fwrite((char *) &SizeAll,1,sizeof(int),fw);
        fwrite((char *) &SizeComm,1,sizeof(int),fw);
        fwrite((char *) &SizeFrom,1,sizeof(int),fw);
        fwrite((char *) &SizeTo,1,sizeof(int),fw);

        fwrite(OutCommand->c_str(),1,OutCommand->length(),fw);            
        fwrite(RulesAnalyzer->CollRules[i]->From.c_str(),1,RulesAnalyzer->CollRules[i]->From.length(),fw);            
        fwrite(RulesAnalyzer->CollRules[i]->To.c_str(),1,RulesAnalyzer->CollRules[i]->To.length(),fw);            

    }

    fclose(fw);
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConvertor::SaveCodeRulesFile\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    return 0;
}

int CConvertor::DoneAll()
{
    string Emp="";
    string AfterAll="AfterAll";
    if (DoElement(&Emp, &AfterAll, 0, 0)) {
        sError = "After All error";
        return 1;
    }
    return 0;
}

int CConvertor::LoadAll(int Code)
{

    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConvertor::LoadAll\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug

    string Emp="";
    string BeforeAll="BeforeAll";

    RulesAnalyzer->SetLog(log,LogAll); 
    MarcIn->SetLog(log,0);
    MarcOut->SetLog(log,0);
    PluginManager->setLog(log,LogAll);

    // Registrace pluginu
    PluginManager->registerPlugins(&Variables);
    if(PluginManager->getErrorMessage() != "")
    {
        sError = PluginManager->getErrorMessage();
        return 1;
    }

    // Natazeni pluginu
    PluginManager->loadPlugins(&Variables);
    if(PluginManager->getErrorMessage() != "")
    {
        sError = PluginManager->getErrorMessage();
        return 1;
    }

    int i;
    string* GNext;

    CreateVariable("S",EX_STRING);
    CreateVariable("NF",EX_INTEGER); 
    CreateVariable("NS",EX_INTEGER);
    CreateVariable("SET",EX_INTEGER);
    CreateVariable("ADD",EX_INTEGER);
    CreateVariable("TOC",EX_STRING);
    CreateVariable("FROMC",EX_STRING);
    CreateVariable("FROM_IDF",EX_STRING);
    if(Code)
        RulesAnalyzer->OpenFromCodeFile(Variables[KEYWORD_RULESFILE]->sName->c_str());
    else
    {
        if (codeVersion!=VERSION_LIGHT)
            RulesAnalyzer->OpenFromFile(Variables[KEYWORD_RULESFILE]->sName->c_str());
    }
    if(RulesAnalyzer->g_Error!="")
    {
        sError = RulesAnalyzer->g_Error;
        return 1;
    }
    for (i=0;i<RulesAnalyzer->CollRules.size();i++)
    {
        string NRule=RulesAnalyzer->CollRules[i]->Command;
        TrimLeft(&NRule,CUTTRIMED);
        TrimRight(&NRule,CUTTRIMED);
        if( NRule!="")
        {
            //Naplneni vsech skriptu
            ActualRule = i;
            CUseScript *NewScript=new CUseScript();
            //NewScript->SetLog(log,0); 
            if (Code)
            {
                if (NewScript->SetSerializeProgNodes(&(RulesAnalyzer->CollRules[i]->Command)))
                {
                    sError = "Rule " + OrdinalForm(i,5)+ ":" + RulesAnalyzer->CollRules[i]->From + "->" + RulesAnalyzer->CollRules[i]->To + NewScript->GetStringError();
                    return 1;       
                }
            }    
            else
            {
                if (NewScript->SetCode(&(RulesAnalyzer->CollRules[i]->Command),0))
                {
                    sError = "Rule " + OrdinalForm(i,5)+ ":" + RulesAnalyzer->CollRules[i]->From + "->" + RulesAnalyzer->CollRules[i]->To + NewScript->GetStringError();
                    return 1;       
                }
            }
            NewScript->gVariables=&Variables;
            NewScript->ptrClientClass=(void *) this;
            NewScript->ptrFunction=CallFunc;
            Codes.insert(make_pair(string(OrdinalForm(i,8)),NewScript));
        }
    }

    RulesAnalyzer->Update();    
    RulesAnalyzer->g_Error = "";

    //spustim pravidlo BeforeAll

    if (DoElement(&Emp, &BeforeAll, 0, 0)) {
        sError = "Before All error";
        return 1;
    }


    for (i=0;i<RulesAnalyzer->CollRules.size();i++)
    {
        //Rozvinuti grupy From
        GNext = GroupExpand(&(RulesAnalyzer->CollRules[i]->From));
        if(*GNext=="")
        {
            sError = "GroupExpand: " + RulesAnalyzer->CollRules[i]->From;
            return 1;
        }   

        RulesAnalyzer->CollRules[i]->From = *GNext;
        delete GNext;

        //Rozvinuti grupy To
        GNext = GroupExpand(&(RulesAnalyzer->CollRules[i]->To));
        if(*GNext=="")
        {
            sError = "GroupExpand: " + RulesAnalyzer->CollRules[i]->From;
            return 1;
        } 
        RulesAnalyzer->CollRules[i]->To = *GNext;
        delete GNext;
    }

    RulesAnalyzer->Update();    

    if(RulesAnalyzer->g_Error!="")
    {
        sError = RulesAnalyzer->g_Error;
        return 1;
    }
    //In no a out no
    if (Variables.count(KEYWORD_INNOIDFIELD)>0)
    {	
        if(*(Variables[KEYWORD_INNOIDFIELD]->sName)!="")
        {
            SplitString(Variables[KEYWORD_INNOIDFIELD]->sName, &(MarcIn->aNoIdenField), ",");
        }
     }   


    if (Variables.count(KEYWORD_INLINKINGFIELDS)>0)
    {	
        if(*(Variables[KEYWORD_INLINKINGFIELDS]->sName)!="")
        {
            SplitString(Variables[KEYWORD_INLINKINGFIELDS]->sName, &(MarcIn->aLinkingField), ",");
        }
    }

    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConvertor::LoadAll\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    return 0;
}

int CConvertor::SetLog(ofstream* lg, int LogAllMes)
{
    log=lg;
    LogAll=LogAllMes;
    return 0;
}

int CConfigFileReader::SetLog(ofstream* lg, int LogAllMes)
{
    log=lg;
    LogAll=LogAllMes;
    return 0;
}

string* CConvertor::GroupExpand(string* sText)
{
    /*
       if (LogAll) { //Debug
       (*log)<< "---------------------------------------------\n";
       (*log)<< "BEGIN: CConvertor::GroupExpand\n";
       (*log)<< "sText: " << *sText + "\n";
       (*log)<< "---------------------------------------------\n";
       } //End Debug*/

    //JE TAM GROUP A MUSIME ROZVIJET
    vector<string *> ArrayHlp;
    vector<string *> ArrayOut;
    vector<string *> ArrayItems;

    SplitString(sText,&ArrayHlp,",");
    string Before;
    string After;
    string NameVariable;
    string ValueVariable;
    int d;

    for (d=0;d<ArrayHlp.size();d++)
    {    
        int Poss =ArrayHlp[d]->find(KEYWORD_GROUP,0);
        if(Poss==(-1)) //Neni rady group - jenom pridame
        {
            ArrayOut.push_back(new string(*(ArrayHlp[d])));
        }
        else  //Je tu group musime rozvinout podle promenne
        {
            if(ArrayHlp[d]->substr(Poss + 5, 1)!="(")
            {         
                return new string("");
            }    
            int Poss2 = ArrayHlp[d]->find(")",Poss + 5);
            if(Poss2 ==(-1)) //Chybi uzaviraci zavorka
            {
                return new string("");
            }    
            NameVariable = ArrayHlp[d]->substr(Poss + 6, Poss2 - Poss - 6);
            //(*log)<< "NAME:" << NameVariable << "\n";
            ValueVariable = "";
            //Hledame promennou v globalnich promennych
            if (Variables.count(NameVariable)==0)
            {
                return new string("");
            }
            ValueVariable=*(Variables[NameVariable]->sName);
            //(*log)<< "VALUE:" << ValueVariable << "\n";
            SplitString(&ValueVariable,&ArrayItems,",");                
            for(int o=0; o<ArrayItems.size(); o++)
            { 
                //BEFORE
                if(Poss==0)
                    Before = "";
                else
                    Before = ArrayHlp[d]->substr(0, Poss - 1);

                //AFTER
                if(Poss2==(ArrayHlp[d]->length()-1))
                    After = "";
                else
                    After = ArrayHlp[d]->substr(Poss2 + 1);
                //(*log)<< "Before:" << Before << "After:" << After << "\n";   
                string TIte = *(ArrayItems[o]);
                TrimLeft(&TIte,CUTTRIMED);
                TrimRight(&TIte,CUTTRIMED);
                TIte = Before + TIte + After;
                //(*log)<< "TIte " << TIte << "\n";
                ArrayOut.push_back(new string(TIte));
            }
        }
    }
    //Vyliti na vystup

    string Out = "";
    for (d=0;d<ArrayOut.size();d++)
    { 
        if (d!=(ArrayOut.size()-1))
            Out = Out + *(ArrayOut[d]) + ",";
        else
            Out = Out + *(ArrayOut[d]);
    }
    //(*log)<< "Out: " << Out << "\n";
    for (d=0;d<ArrayOut.size();d++)
        delete ArrayOut[d];
    for (d=0;d<ArrayItems.size();d++)
        delete ArrayItems[d]; 
    for (d=0;d<ArrayHlp.size();d++)
        delete ArrayHlp[d];            
    /*if (LogAll) { //Debug
      (*log)<< "---------------------------------------------\n";
      (*log)<< "END: CConvertor::GroupExpand\n";
      (*log)<< "Return:" << Out << "\n";
      (*log)<< "---------------------------------------------\n";
      } //End Debug      */     

    return new string(Out); 
}


int CConvertor::DoOneRecord()
{
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConvertor::DoOneRecord\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    int i;
    int j;
    int k;
    CField* ActF;
    CSubfield* ActSubF;
    CField* ActLinkF;
    CSubfield* ActLinkSubF;

    string Emp="";
    string Emp2="";
    string Before="Before";
    string After="After";
    string LAB="LAB";
    string Comm;

    DoingLinking = 0;
    *(MarcOut->g_FieldPrefix)=*(MarcIn->g_FieldPrefix);

    if (Variables.count(KEYWORD_IDALEPHCODING)>0)
    {
        *(MarcOut->g_Format)=*(Variables[KEYWORD_IDALEPHCODING]->sName); 
    }

    if (DoElement(&Emp, &Before, 0, 0)) return 1;
    if (DoElement(MarcIn->g_Label, &LAB, 0, 0)) return 1;
    for(i=0;i<MarcIn->Fields.size();i++)
    {
        ActF = MarcIn->Fields[i];
        //(*log)<< "==========\n";
        //(*log)<< *(ActF->ID) << "\n";
        //(*log)<< "==========\n";
        lPointerField = i;
        if (RulesAnalyzer->IsForC(ActF->ID))
        {
            Comm=*(ActF->ID) + "I1";             
            if (DoElement(ActF->ID1, &Comm, ActF->Index, 0)) return 1;

            Comm=*(ActF->ID) + "I2";
            if (DoElement(ActF->ID2, &Comm, ActF->Index, 0)) return 1;

            for(j=0;j<ActF->ptrCollSubf.size();j++)
            {  
                lPointerSubField = j;     
                ActSubF = ActF->ptrCollSubf[j];
                DoingLinking = 0;
                if(*(ActSubF->LinkingField->ID)!="")
                {
                    ActLinkF = ActSubF->LinkingField;
                    Comm=*(ActF->ID) + "L" + *(ActLinkF->ID) + "I1";
                    if (DoElement(ActLinkF->ID1, &Comm, ActF->Index, 0)) return 1;

                    Comm=*(ActF->ID) + "L" + *(ActLinkF->ID) + "I2";
                    if (DoElement(ActLinkF->ID2, &Comm, ActF->Index, 0)) return 1;

                    DoingLinking = 1;
                    for(k=0;k<ActLinkF->ptrCollSubf.size();k++)
                    {
                        ActLinkSubF = ActLinkF->ptrCollSubf[k];
                        lPointerLinkingField = k;
                        Comm=*(ActF->ID) + "L" + *(ActLinkF->ID) + "$" + *(ActLinkSubF->ID);

                        if (DoElement(ActLinkSubF->Value, &Comm, ActF->Index, ActLinkSubF->Index)) return 1;
                    }
                    DoingLinking = 0;
                }
                else
                {
                    Comm=*(ActF->ID) + "$" + *(ActSubF->ID);         
                    if (LogAll) { //Debug
                        (*log)<< *(ActF->ID) << "$" << *(ActSubF->ID) << *(ActSubF->Value) << "\n";
                    } //End Debug 
                    if (DoElement(ActSubF->Value, &Comm, ActF->Index, ActSubF->Index)) return 1;
                }
            }
        } 
        else
        {
            //hloubkova kopie
            CField* NewField=new CField();
            MarcOut->CloneField(NewField,ActF);
            MarcOut->Fields.push_back(NewField); 
        } 
    }
    if (DoElement(&Emp, &After, 0, 0)) return 1;
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConvertor::DoOneRecord\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug    
    return 0;
}


int CConvertor::DoElement(string* Value, string* Command, long iField, long iSubfield)
{
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConvertor::DoElement\n";
        (*log)<< "Value :" << *Value << "\n";
        (*log)<< "Command :" << *Command << "\n";
        (*log)<< "iField :" << iField << "\n";
        (*log)<< "iSubfield :" << iSubfield << "\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    string outValue;
    int i;
    int IR;
    CCollRulesByC* CollR= RulesAnalyzer->GetRulesByC(Command);
    CRuleItem* myRuleItem;
    //    (*log)<< "FINDED RULES: " << CollR->CCollection.size() << "\n";
    for(i=0;i<CollR->CCollection.size();i++)
    {
        //V konkretnim pravidle element cislo
        IR = CollR->IndexInRuleColl[i];
        //Element pravidla ktery opravdu potrebuji
        myRuleItem = CollR->CCollection[i]->CollRulesItem[IR];
        //Oriznuti hodnoty
        string outValue = *Value;

        if (myRuleItem->Start > 0 && myRuleItem->Stop <= outValue.length())
            outValue = Value->substr(myRuleItem->Start-1, myRuleItem->Stop - myRuleItem->Start+1);

        //DoRule
        //        (*log)<< "RULE: " << Codes[OrdinalForm(CollR->IndexColl[i],8)] << "\n";
        if(DoRule(Codes[OrdinalForm(CollR->IndexColl[i],8).c_str()],
                    &(outValue),
                    &(myRuleItem->From), &(myRuleItem->To), 
                    &(CollR->CCollection[i]->Command),
                    CollR->IndexInRuleColl[i],
                    iField, iSubfield) == 1)
            return 1;
        MarcOut->FillFieldsDepth();
    }
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConvertor::DoElement\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug        
    return 0;
}

int CConvertor::ClearErrors() {
    sError="";
    MarcIn->g_Error="";
    MarcOut->g_Error="";
    return 0;
}

int CConvertor::CreateVariable(string Key, int iType)
{
    if (Variables.count(Key)==0)
    {
        M_EXPRESSION_ELEMENT* newEl= new  M_EXPRESSION_ELEMENT();
        newEl->sName=new string("");
        newEl->iType=iType;
        Variables.insert(make_pair(string(Key),newEl));
    }
    return 0;
}

int CConvertor::DoRule(CUseScript* UseScript, string* sValue, string* sFrom, string* sTo, string* sCommand, long IndexRule, long iField, long iSubfield)
{
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "BEGIN: CConvertor::DoRule\n";
        (*log)<< "sValue :\"" << *sValue << "\"\n";
        (*log)<< "sFrom :" << *sFrom << "\n";
        (*log)<< "sTo :" << * sTo << "\n";
        (*log)<< "sCommand :" << * sCommand << "\n";
        (*log)<< "IndexRule :" <<  IndexRule << "\n";
        (*log)<< "iField :" <<  iField << "\n";
        (*log)<< "iSubfield :" <<  iSubfield << "\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug
    if((*sFrom)!="Before" && (*sFrom)!="After" && (*sFrom)!="BeforeAll" && (*sFrom)!="AfterAll" )
    {
        //Spustime vzorec
        TrimLeft(sCommand,CUTTRIMED);
        TrimRight(sCommand,CUTTRIMED);
        if ((*sCommand)!="")
        {
            //vztahujici se k zaznamu
            *(Variables["S"]->sName)=*(sValue);
            Variables["S"]->iType=EX_STRING;
            Variables["NF"]->iIntValue=iField;
            Variables["NF"]->iType=EX_INTEGER;
            Variables["NS"]->iIntValue=iSubfield;
            Variables["NS"]->iType=EX_INTEGER;
            Variables["SET"]->iIntValue=1;
            Variables["SET"]->iType=EX_INTEGER;
            Variables["ADD"]->iIntValue=0;
            Variables["ADD"]->iType=EX_INTEGER;
            *(Variables["TOC"]->sName)=*(sTo);
            Variables["TOC"]->iType=EX_STRING;
            *(Variables["FROMC"]->sName)=*(sFrom);
            Variables["FROMC"]->iType=EX_STRING;
            *(Variables["FROM_IDF"]->sName)=sFrom->substr(0, 3);
            Variables["FROM_IDF"]->iType=EX_STRING;
            if (UseScript->Execute())
            {
                if (UseScript->intError==ERR_METHOD_RETURNED_ERROR)
                    sError = *(sFrom) + " - " + *(sTo) + " MethodReturn :" +  sError;
                else
                    sError = *(sFrom) + " - " + *(sTo) + " : " + UseScript->GetStringError();
                return 1;
            }
            //Nastavime hodnotu
            if (Variables["SET"]->iIntValue == 1) //Jinak se nic nenastavuje
            {
                MarcOut-> SetValueByCommand(Variables["TOC"]->sName,
                        Variables["S"]->sName,
                        Variables["NF"]->iIntValue,
                        Variables["NS"]->iIntValue, 
                        Variables["ADD"]->iIntValue);
                if (LogAll) { //Debug
                    (*log)<< "SetValue:\n";
                    (*log)<< "TOC:" << *(Variables["TOC"]->sName) << "\n";
                    (*log)<< "S:" << *(Variables["S"]->sName) << "\n";
                    (*log)<< "NF:" << Variables["NF"]->iIntValue << "\n";
                    (*log)<< "NS:" << Variables["NS"]->iIntValue << "\n";
                } //End Debug                                                     
                if(MarcOut->g_Error!="")
                {
                    sError = "Error:" + *(sFrom) + " - " + *(sTo) + ":" + MarcOut->g_Error;
                    return 1;
                }
            }
        }
        else
        {
            MarcOut->SetValueByCommand(sTo,sValue,iField,iSubfield,0);
            if(MarcOut->g_Error!="")
            {
                sError = "Error:" + *(sFrom) + " - " + *(sTo) + ":" + MarcOut->g_Error;
                return 1;
            }
        }
    }    
    else
    {
        if (UseScript->Execute())
        {
            if (UseScript->intError==ERR_METHOD_RETURNED_ERROR)
                sError = *(sFrom) + " - " + *(sTo) + "MethodReturn :" +  sError;
            else
                sError = *(sFrom) + " - " + *(sTo) + UseScript->GetStringError();
            return 1;
        }
    }
    if (LogAll) { //Debug
        (*log)<< "---------------------------------------------\n";
        (*log)<< "END: CConvertor::DoRule\n";
        (*log)<< "---------------------------------------------\n";
    } //End Debug     

    return 0;
}

int CConvertor::InsertRecord(string* Record, int Format)
{
    if (MarcIn->OpenRecordFromString(Record,Format)) {
        sError  = MarcIn->g_Error;                                           
        return 1;
    }
    if (LogAll) { //Debug
        for (int i=0;i<MarcIn->Fields.size();i++)
        {
            //(*log)<< *(MarcIn->Fields[i]);
            (*log)<< "\n"; 
        }
    } //End Debug 
    return 0;
}

int CConvertor::Convert()
{
    sError="";
    MarcIn->g_Error="";
    MarcOut->g_Error="";
    MarcIn->FillFieldsDepth();
    delete MarcOut;
    MarcOut=new CMarcAnalyzer();
    if (Variables.count(KEYWORD_OUTNOIDFIELD)>0)
    {
        if(*(Variables[KEYWORD_OUTNOIDFIELD]->sName)!="")
        {
            SplitString(Variables[KEYWORD_OUTNOIDFIELD]->sName, &(MarcOut->aNoIdenField), ",");    
        }
    }
    if (Variables.count(KEYWORD_OUTLINKINGFIELDS)>0)
    {
        if(*(Variables[KEYWORD_OUTLINKINGFIELDS]->sName)!="")
        {
            SplitString(Variables[KEYWORD_OUTLINKINGFIELDS]->sName, &(MarcOut->aLinkingField), ",");    
        }
    }
    
    
    //MarcOut->g_Label=new string("                        "); // memory leak: g_Label se inicializuje uz v konstruktoru
    if(MarcIn->g_Error!="")
    {
        sError = MarcIn->g_Error;
        return 1;
    }
    /*for(int i=0;i< MarcOut->Fields.size();i++) // neni nutne - o kus vyse se vytvari nova instance, neni co mazat
        delete MarcOut->Fields[i];
    MarcOut->Fields.clear();    
    */
    if(DoOneRecord() == 1)
    {
        if(MarcOut->g_Error!="")
        {
            sError = sError + " : " +  MarcOut->g_Error;
            return 1;
        }
        if(MarcIn->g_Error!="")
        {
            sError = sError + " : " + MarcIn->g_Error;
            return 1; 
        }
        return 1;
    }
    MarcOut->SortFields();
    return 0;
}

string* CConvertor::GetOut(int Format)
{
    //cout << "GET_OUT\n";
    string* OutR= MarcOut->GetSerializeRecord(Format);    
    if(MarcOut->g_Error!="")
    {
        sError = MarcOut->g_Error;
        return new string("");
    }    
    return OutR;
}
