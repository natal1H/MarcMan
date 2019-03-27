#include "CRulesAnalyzer.h"

#include <stdio.h>
#include <fcntl.h>


char* CUTTRIMED=                   "\n\t\r ";

ostream& operator<<(ostream &os, CRule &objekt)
{
    os << "FROM: " + objekt.From + " TO: " + objekt.To;
    return os;
}

int CRulesAnalyzer::SetLog(ofstream* lg, int LogAllMes)
{
	log=lg;
	LogAll=LogAllMes;
	return 0;
}

string CRulesAnalyzer::OrdinalForm(int number, int size)
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

void  CRulesAnalyzer::TrimLeft(string* sStr, char* ArraySep)
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

void  CRulesAnalyzer::TrimRight(string* sStr, char* ArraySep)
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


int CRulesAnalyzer::SplitString(string* strSplit, vector<string*>* outArray, char* Separator)
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

/***********************************************************************
 *
 * FUNCTION:     MarkSections
 *
 * DESCRIPTION:  funkce vezme analyzovany text a vyhleda v nìm èásti zaèínající chBegin a konèící chEnd a oznaèí
 *               je ve stejnì dlouhém stringu strMark identifikatory chMarkBegin a chMarkEnd. Je-li promenna iInsertField
 *               1 oznaèí se vcetne konecneho chEnd
 *
 * PARAMETERS:   (IN)   string* strAnalyzeText - analyzovany text
 *               (OUT)	string* strMarks - oznaèkovaný text
 *               (IN)	char chBegin - hledaný zaèátek
 *               (IN)	char chEnd - hledaný konec
 *               (IN)	char chMarkBegin - znacka zacatku
 *               (IN)	char chMarkEnd - znacka konce
 *               (IN)	int iInsertEnd - má se vložit i konecny znak?
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CRulesAnalyzer::MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd)
{
    int iCounter=0;
    int iPossitionStart;
    int iPossitionStop;
    
    while (iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(chBegin,iCounter);
        
        if (iPossitionStart==(-1)) //Žádný øetìzec nenalezen
            return 0;
      
        (*strMarks)[iPossitionStart]=chMarkBegin; //Zacatek retezce

        if (iPossitionStart+1<=strAnalyzeText->length())
            iPossitionStop=strAnalyzeText->find(chEnd,iPossitionStart+1);
        else
            return 0;

        if (iPossitionStop==(-1)) //Konec øetìzce nenalezen
            return 0;
        
        if (iInsertEnd)
            (*strMarks)[iPossitionStop]=chMarkEnd;
        else
            (*strMarks)[iPossitionStop-1]=chMarkEnd;
        iCounter=iPossitionStop+1;
    }
    return 0;
}


int CRulesAnalyzer::MarkSections2(string* strAnalyzeText, string* strMarks, char* chBegin, char* chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd)
{
    int iCounter=0;
    int h;
    int iPossitionStart;
    int iPossitionStop;
    string chB=chBegin;
    string chE=chEnd;
    while (iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(chB,iCounter);
        
        if (iPossitionStart==(-1)) //Žádný øetìzec nenalezen
            return 0;
      
        if (iPossitionStart>52522)
        {
           h=0;   
        }

        (*strMarks)[iPossitionStart]=chMarkBegin; //Zacatek retezce

        if (iPossitionStart+1<=strAnalyzeText->length()) 
            iPossitionStop=strAnalyzeText->find(chEnd,iPossitionStart+1);
        else
            return 0;

        if (iPossitionStop==(-1)) //Konec øetìzce nenalezen
            return 0;
        
        if (iInsertEnd)
            (*strMarks)[iPossitionStop+chE.length()]=chMarkEnd;
        else
            (*strMarks)[iPossitionStop-1+chE.length()]=chMarkEnd;
        iCounter=iPossitionStop+1;
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     FillMarkSection
 *
 * DESCRIPTION:  funkce vezme omarkovaný text tvaru XXXXXXXCXXXMXXXXEXXXXNXX a vyplní v nìm 
 *               sekce zaèínající chMarkBegin a konèící chMarkEnd charem chMarkBegin.Pøi C,N by
 *               vysledek byl XXXXXXXCCCCCCCCCCXXXXNXX
 *
 * PARAMETERS:   (OUT)	string* strMarks - oznaèkovaný text
 *               (IN)	char chMarkBegin - hledaný zaèátek
 *               (IN)	char chMarkEnd - hledaný konec
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CRulesAnalyzer::FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd)
{
    int iPossitionStart;
    int iPossitionStop;    
    int iCounter=0;
    while (iCounter<strMark->length())
    {
        iPossitionStart=strMark->find(chMarkBegin,iCounter);
    
        if (iPossitionStart==(-1)) //Žádný øetìzec nenalezen
            return 0;

        if (iPossitionStart+1<=strMark->length())
            iPossitionStop=strMark->find(chMarkEnd,iPossitionStart+1);
        else
            iPossitionStop=strMark->length()-1;

        if (iPossitionStop==(-1)) //Konec øetìzce nenalezen
            iPossitionStop=strMark->length()-1;
    
        for (int i=iPossitionStart;i<iPossitionStop+1;i++)
            (*strMark)[i]=chMarkBegin; //Zacatek retezce

        iCounter=iPossitionStop+1;
    }

    return 0;
} 

/***********************************************************************
 *
 * FUNCTION:     CutCommandByType
 *
 * DESCRIPTION:  funkce vezme analyzovaný a omarkovaný text a vytvoøí pole analyzovaných
 *               stringù a markovacích stringù délky jedna. Funkce analyzovaný text dìlí podle 
 *               identifikovaných sekcí vzniklých vyplnìním markovacího stringu
 *
 * PARAMETERS:   (IN)    string* strCommand - analyzovaný text
 *               (IN)	 string* strMarkCommand -omarkovaný text
 *               (OUT)	 stringArray *ptrElements - jednotlivé èásti analyzovaného textu podle sekcí
 *               (IN)	 stringArray *ptrMarkElements - markovací stringy délky jedna oznaèující typ sekce
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

void CRulesAnalyzer::CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements)
{
    char chLastChar='J';
    char chNowChar;
    int iElementCounter=-1;
    string newChar;

    for (int i=0;i<strMarkCommand->length();i++)
    {
        chNowChar=(*strMarkCommand)[i];
        if (chNowChar!=chLastChar)
        {
            iElementCounter++;
            newChar=(*strCommand)[i];
            ptrElements->push_back(new string(newChar));
            newChar=(*strMarkCommand)[i];
            ptrMarkElements->push_back(new string(newChar));
            chLastChar=chNowChar;
        }
        else
        {
            string* ptrStr;
            ptrStr =(*ptrElements)[iElementCounter];
            *ptrStr+=(*strCommand)[i];
            (*ptrElements)[iElementCounter]= ptrStr;
        }
    }
} 

int CRulesAnalyzer::RecognizeCommand(string* Comm, string *LAB, string *IDF, string *ILF, string *IDS, long* IF, long* IS, int* Indic, int* Sta, int* Sto)
{
    string OutStr;

    string IDField;      //Field
    string IDSub;          //Subfile
    long IndexField=0;      //pozice fieldu
    long IndexSubField=0;   //pozicesubfieldu
    string LinkField;      //Cislo linkovaciho fieldu
    int Indikator=0;
    int Start=-1;
    int Stop=-1;

    int Pointer; 
    string mV;
    string CommNext;

    //Pole
    Pointer=0;

    if (Comm->length()<Pointer+3) return 1;
    //LABEL
    if (Comm->substr(Pointer,3)=="LAB")
    {
        if (Comm->length()==3)
        {
            *LAB="OK";
            *Sta=Start;
            *Sto=Stop;
            return 0;
        }
        else
        {
            Pointer+=3;
            CommNext=Comm->substr(Pointer,1);
            if (CommNext=="/")
            {
                int Poss=Comm->find("-",Pointer+1);
                if (Poss==(-1))
                    return 1;
                mV=Comm->substr(Pointer+1,Poss-(Pointer+1));
                if ((Start=atoi(mV.c_str()))==0)
                    return 1;
                else
                {
                    Pointer+=Poss-Pointer+1;
                    int Poss=Comm->find("/",Pointer);
                    if (Poss==(-1))
                        return 1;
                    mV=Comm->substr(Pointer,Poss-Pointer);
                    if ((Stop=atoi(mV.c_str()))==0)
                        return 1;
                }

            }
            else
                return 1;
            if (Start-1<Stop && Stop<24+1) 
            {
                *LAB="OK";
                *Sta=Start;
                *Sto=Stop;
                return 0;
            }
        }
    }


    
    IDField=Comm->substr(Pointer,3);

    //Co nasleduje
    Pointer+=3;
    if (Comm->length()>Pointer)
    {
        CommNext=Comm->substr(Pointer,1);
        if (CommNext=="(") //Zapiseme index
        {
            int Poss=Comm->find(")",Pointer+1);
            if (Poss==(-1))
                return 1;
            mV=Comm->substr(Pointer+1,Poss-(Pointer+1));
            if ((IndexField=atoi(mV.c_str()))==0)
                return 1;   
            Pointer+=Poss-Pointer+1;
        }

        //Co nasleduje
        if (Comm->length()>Pointer) 
        {
            CommNext=Comm->substr(Pointer,1);
    
            if (CommNext=="L") //Kdyz nasleduje L tak je to linkovany pole napr 200L100(n)$a(n)
            {
                if (Comm->length()<Pointer+4) return 1;
                LinkField=Comm->substr(Pointer+1,3);
                Pointer+=4;
            }
    
            if (Comm->length()>Pointer) 
            {
                CommNext=Comm->substr(Pointer,1);
                if (CommNext=="I") //Indikator
                {
                    if (Comm->length()<Pointer+1) return 1;
                    string CommNext2=Comm->substr(Pointer+1,1);
                    if (CommNext2=="1")
                    {
                        Indikator=1;
                    }
                    if (CommNext2=="2")
                    {
                        Indikator=2;
                    }
                }
    
                if (CommNext=="$")
                {
                    if (Comm->length()<Pointer+1) return 1;
                    IDSub=Comm->substr(Pointer+1,1);
                }

                //Zase bud zavorka a nebo /10-20/
                Pointer+=2;
                if (Comm->length()>Pointer)
                {
                    CommNext=Comm->substr(Pointer,1);
    
                    if (CommNext=="(")
                    {
                        int Poss=Comm->find(")",Pointer+1);
                        if (Poss==(-1))
                            return 1;
                        if (Poss<5)
                            return 1;
                        mV=Comm->substr(Pointer+1,Poss-(Pointer+1));
                        if ((IndexSubField=atoi(mV.c_str()))==0)
                            return 1;
                        Pointer+=Poss-Pointer+1;
                    }

                    if (Comm->length()>Pointer)
                    {
                        CommNext=Comm->substr(Pointer,1);
    
                        if (CommNext=="/")
                        {
                            int Poss=Comm->find("-",Pointer+1);
                            if (Poss==(-1))
                                return 1;
                            mV=Comm->substr(Pointer+1,Poss-(Pointer+1));
                            if ((Start=atoi(mV.c_str()))==0)
                                return 1;
                            else
                            {
                                Pointer+=Poss-Pointer+1;
                                int Poss=Comm->find("/",Pointer);
                                if (Poss==(-1))
                                    return 1;
                                mV=Comm->substr(Pointer,Poss-Pointer);
                                if ((Stop=atoi(mV.c_str()))==0)
                                    return 1;
                            }

                        }
                        else
                            return 1;
                    }
                }
            }
        }
    }
    *IDF=IDField;
    *ILF=LinkField;
    *IDS=IDSub;
    *IF=IndexField;
    *IS=IndexSubField;
    *Indic=Indikator;
    *Sta=Start;
    *Sto=Stop;
    return 0;
}

int CRulesAnalyzer::OpenFromFile(const char* sPath)
{
        // pro jistotu kontola verze
    if (codeVersion==VERSION_LIGHT)
        return 1;

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CRulesAnalyzer::OpenFromFile\n";
(*log)<< "sPath :" << sPath << "\n";  
(*log)<< "---------------------------------------------\n";
} //End Debug

    int i;
    FILE *fr;
    vector<string *> Elements;
    vector<string *> MarkElements;
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
    
    string* strMark=new string(strAnalyzeText.length(), 'x');
 
	MarkSections2(&strAnalyzeText,strMark,"/*","*/",'C','R',0);
    MarkSections2(&strAnalyzeText,strMark,"{","}",'B','E',0);
    MarkSections2(&strAnalyzeText,strMark,"\'","\n",'W','N',0);    
    FillMarkSection(strMark,'C','R');
    FillMarkSection(strMark,'B','E');
    FillMarkSection(strMark,'W','N');

    
    CutCommandByType(&strAnalyzeText,strMark,&Elements,&MarkElements);    

    //Ostranime prazdny radky a komentare
    for(i=Elements.size()-1;i>(-1);i--)
    {
        TrimLeft(Elements[i],CUTTRIMED);
        TrimRight(Elements[i],CUTTRIMED);
        if (*(Elements[i])=="" || *(MarkElements[i])=="C" || *(MarkElements[i])=="W")
        {
            delete Elements[i];
            delete MarkElements[i]; 
            Elements.erase(Elements.begin()+i);
            MarkElements.erase(MarkElements.begin()+i); 
        }
        
    }
    
    if (Elements.size()%2==1) g_Error="Rules file involved";

    for(i=0;i<Elements.size();i+=2)
    {
        if (*(MarkElements[i])!="x" || *(MarkElements[i+1])!="B")
        {
            g_Error="Rules file involved - Rule number. " + OrdinalForm(i,5); ;
            return 1;
        }
        int PossArrow = Elements[i]->find("->",0);
        if (PossArrow==(-1)) 
        {
            g_Error="Rules file involved - Rule number. " + OrdinalForm(i,5); ;
            return 1;
        }
        
        if (Elements[i+1]->length()<2)  
        {
            g_Error="Rules file involved - Rule number. " + OrdinalForm(i,5); ;
            return 1;
        }
        CRule* NewRule=new CRule();
        NewRule->From=Elements[i]->substr(0,PossArrow);
        NewRule->To=Elements[i]->substr(PossArrow+2);
        NewRule->Command=Elements[i+1]->substr(1,Elements[i+1]->length()-2);

        /*if (LogAll) { //Debug
            (*log)<< *NewRule << "\n";  
            (*log)<< NewRule->Command << "\n";
        } //End Debug        */
        CollRules.push_back(NewRule);
    }
    fclose(fr);
    for(int z=0;z<Elements.size();z++)
       delete Elements[z];
    for(int a=0;a<MarkElements.size();a++)
       delete MarkElements[a];   
    delete[] buffer;
    delete strMark;
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CRulesAnalyzer::OpenFromFile\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
	return 0;
}

int CRulesAnalyzer::CodeStringChar(char* sRet, int size)
{
	for(int i=0;i<size;i++)
	{
		char Ret=sRet[i];
		Ret=Ret^255;
		sRet[i]=Ret;
	}
	return 0;
}


int CRulesAnalyzer::OpenFromCodeFile(const char* sPath)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CRulesAnalyzer::OpenFromCodeFile\n";
(*log)<< "sPath :" << sPath << "\n";  
(*log)<< "---------------------------------------------\n";
} //End Debug

    FILE *fr;
    if ((fr=fopen(sPath,"rb"))== NULL)
    {
       g_Error="Rules file not open. ";
       return 1;
    }        
	//_setmode( _fileno(fr), _O_BINARY );
    int nBytesRead=0;
    char* buffer = new char[MAXBUF];

	while((nBytesRead = fread(buffer, 1, 4*sizeof(int),fr)))
	{	
	   char *ActualText=buffer;
		
	   int SizeAll=*((int *) buffer);  

	   ActualText+=sizeof(int);
	   int SizeComm=*((int *) ActualText);	

	   ActualText+=sizeof(int);
	   int SizeFrom=*((int *) ActualText);	

       ActualText+=sizeof(int);
	   int SizeTo=*((int *) ActualText);	

	   ActualText+=sizeof(int);

if (LogAll) { //Debug
(*log)<< "\n";
(*log)<< "SizeAll:" << SizeAll << "\n";
(*log)<< "SizeComm:" << SizeComm << "\n";
(*log)<< "SizeFrom:" << SizeFrom << "\n";
(*log)<< "SizeTo:" << SizeTo << "\n";
} //End Debug 

	   char* ThreeStrings = new char[SizeAll];
	   nBytesRead = fread(ThreeStrings, 1, SizeAll,fr);
	   CodeStringChar(ThreeStrings,SizeAll);

	   CRule *NewRule = new CRule();
	   NewRule->Command.append(ThreeStrings,SizeComm);
	   NewRule->From.append(ThreeStrings+SizeComm,SizeFrom);
	   NewRule->To.append(ThreeStrings+SizeComm+SizeFrom,SizeTo);  
if (LogAll) { //Debug
(*log)<< "NewRule->Command:" << NewRule->Command << "\n";
(*log)<< "NewRule->From:" << NewRule->From << "\n";
(*log)<< "NewRule->To:" << NewRule->To << "\n";
} //End Debug    
	   CollRules.push_back(NewRule); 
	}       

    delete buffer;
    fclose(fr);    
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CRulesAnalyzer::OpenFromCodeFile\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
	return 0;
}

int CRulesAnalyzer::Update()
{

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CRulesAnalyzer::Update\n";
(*log)<< "---------------------------------------------\n";
} //End Debug

    int i;
    int w;
   //pro prenos
    string LAB;
    string IDF;
    string ILF;
    string IDS;
    int INDIC;
    int Start;
    int Stop;
    long IF;
    long IS;

    vector<string *> Splited;
    vector<string *> SplitedTo;
    long Num=CollRules.size();
    for(i=0;i<Num;i++)
    {
        CollRules[i]->CollRulesItem.clear();
    }
    
    for(i=0;i<Num;i++)
    {
       for(w=0;w<Splited.size();w++)
          delete Splited[w];
       for(w=0;w<SplitedTo.size();w++)
          delete SplitedTo[w];          
       Splited.clear();   
       SplitedTo.clear();   
       SplitString(&(CollRules[i]->From),&Splited,",");
       SplitString(&(CollRules[i]->To),&SplitedTo,",");
       if (Splited.size()!=SplitedTo.size())
       {
           g_Error="Error From->To :" + CollRules[i]->From;
           return 1;
       }
       for(int j=0;j<Splited.size();j++)
       {
           LAB="";IDF="";ILF="";IDS="";INDIC=0;Start=0;Stop=0;
           //Provedu recognize a vlozim vysledek - Add do CollRuleItems
           if (CollRules[i]->From!="Before" && CollRules[i]->From!="After" && CollRules[i]->From!="BeforeAll" && CollRules[i]->From!="AfterAll")
           {
               if (RecognizeCommand(Splited[j],&LAB,&IDF,&ILF,&IDS,&IF,&IS,&INDIC,&Start,&Stop))
               {
                   g_Error="Invalid Command " + CollRules[i]->From + "->" + CollRules[i]->To;
                   return 1;
               }
           }
           CRuleItem* NewRuleItem=new CRuleItem();
           NewRuleItem->LAB=LAB;
           NewRuleItem->IDF=IDF;
           NewRuleItem->ILF=ILF;
           NewRuleItem->IDS=IDS;
           NewRuleItem->From=*(Splited[j]);
           NewRuleItem->To=*(SplitedTo[j]);
           NewRuleItem->INDIC=INDIC;
           NewRuleItem->Start=Start;
           NewRuleItem->Stop=Stop;
           NewRuleItem->Stop=Stop;
           CollRules[i]->CollRulesItem.push_back(NewRuleItem);
           /*if (LogAll) { //Debug
           (*log)<< "NewRuleItem: FROM: " + NewRuleItem->From + " To: " + NewRuleItem->To + "\n";
           } //End Debug   */        
       } 
    }
    for(w=0;w<Splited.size();w++)
          delete Splited[w];
    for(w=0;w<SplitedTo.size();w++)
          delete SplitedTo[w];
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CRulesAnalyzer::Update\n";
(*log)<< "---------------------------------------------\n";
} //End Debug        
    
	return 0;
}

int CRulesAnalyzer::IsForC(string* Comm)
{
    string IDField;
    for(long i=0;i<CollRules.size();i++)
    {
       for(long j=0;j<CollRules[i]->CollRulesItem.size();j++)
       {
            IDField=CollRules[i]->CollRulesItem[j]->IDF;
            if ((*Comm)==IDField) return 1;
       }
    }   
    return 0;
}

int CRulesAnalyzer::LoadCommand(string* Comm, CCollRulesByC* newCOM)
{

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CRulesAnalyzer::LoadCommand\n";
(*log)<< "Comm: " << *Comm << "\n";
(*log)<< "ptr newCOM\n";
(*log)<< "---------------------------------------------\n";
} //End Debug

    //pro prenos
    string LAB;
    string IDF;
    string ILF;
    string IDS;
    int INDIC;
    int Start;
    int Stop;
    long IF;
    long IS;
    
    string sLab;
    string IDField;
    string IDSubField;
    string IDLinkField;
    string sFrom;
    long locINDIC;

    LAB="";IDF="";ILF="";IDS="";INDIC=0;Start=0;Stop=0;
    //Provedu recognize a vlozim vysledek - Add do CollRuleItems
    if (*(Comm)!="After" && *(Comm)!="Before" && *(Comm)!="BeforeAll" && *(Comm)!="AfterAll")
    {
        if (RecognizeCommand(Comm,&LAB,&IDF,&ILF,&IDS,&IF,&IS,&INDIC,&Start,&Stop))
        {
           g_Error="Invalid Command ";
           return 1;
        }
    }
    
    for(long i=0;i<CollRules.size();i++)
    {
       for(long j=0;j<CollRules[i]->CollRulesItem.size();j++)
       {
            CRule* OneRule=CollRules[i];
            sFrom=CollRules[i]->CollRulesItem[j]->From;
            IDField=CollRules[i]->CollRulesItem[j]->IDF;
            IDSubField=CollRules[i]->CollRulesItem[j]->IDS;
            sLab=CollRules[i]->CollRulesItem[j]->LAB;
            IDLinkField=CollRules[i]->CollRulesItem[j]->ILF;
            locINDIC=CollRules[i]->CollRulesItem[j]->INDIC;
            if (LogAll) { //Debug
            //(*log)<< "FINDING BY: " << sFrom << " " << IDField << " " << IDSubField << " " << sLab << " " << IDLinkField << "\n";
            } //End Debug
            int r=0;
            if ((sFrom=="After")  && (*(Comm)=="After"))
            {
                newCOM->Add(OneRule,i,j);
                r=1;
            }
            if ((sFrom=="Before")  && (*(Comm)=="Before"))
            {
                newCOM->Add(OneRule,i,j);
                r=1;
            }
            if ((sFrom=="BeforeAll")  && (*(Comm)=="BeforeAll"))
            {
                newCOM->Add(OneRule,i,j);
                r=1;
            }
            if ((sFrom=="AfterAll")  && (*(Comm)=="AfterAll"))
            {
                newCOM->Add(OneRule,i,j);
                r=1;
            }
            if (*(Comm)!="After" && *(Comm)!="Before" && *(Comm)!="BeforeAll" && *(Comm)!="AfterAll")
            {
                if (LAB=="OK")
                {
                    if(sLab=="OK")
                        newCOM->Add(OneRule,i,j);
                }
                else
                {
                    if (ILF=="")
                    {
                        if (INDIC>0)
                        {
                            //(*log)<< "COMPARE: " << locINDIC << ":" << INDIC << "-" << IDField << ":" << IDF << "-" << IDLinkField << "" << "\n";
                            if(locINDIC==INDIC && IDField==IDF && IDLinkField=="")
                            {
                                if (LogAll) { //Debug
                                   (*log)<< "ADDING: " << OneRule->From << "->" << OneRule->To << "\n";
                                } //End Debug                                
                                newCOM->Add(OneRule,i,j);
                            }
                        }
                        else
                        {
                            if(IDSubField==IDS && IDField==IDF && IDLinkField=="")
                            {
                                if (LogAll) { //Debug
                                   (*log)<< "ADDING: " << OneRule->From << "->" << OneRule->To << "\n";
                                } //End Debug                               
                                newCOM->Add(OneRule,i,j);
                            }    
                        }
                    }
                    else
                    {
                        if (INDIC>0)
                        {
                            if(locINDIC==INDIC && IDField==IDF && IDLinkField==ILF)
							{
                                if (LogAll) { //Debug
                                   (*log)<< "ADDING: " << OneRule->From << "->" << OneRule->To << "\n";
                                } //End Debug   							
								newCOM->Add(OneRule,i,j);
							}
                        }
                        else
                        {
                            if(IDSubField==IDS && IDField==IDF && IDLinkField==ILF)
                            {
                                if (LogAll) { //Debug
                                   (*log)<< "ADDING: " << OneRule->From << "->" << OneRule->To << "\n";
                                } //End Debug                               
                                newCOM->Add(OneRule,i,j);
                            }    
                        }
                    }
                }
            }
       }
    }
    
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CRulesAnalyzer::LoadCommand\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
    
	return 0;
}

CCollRulesByC* CRulesAnalyzer::GetRulesByC(string* Command)
{

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CRulesAnalyzer::GetRulesByC\n";
(*log)<< "Command: " << * Command<< "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug
    
    //podivam se jestli uz tam je
    //(*log)<< "Je v mape? : " << mapRulByC.count(Command->c_str()) << "\n";
    if(mapRulByC.count(Command->c_str())==0)
    {
        //neni tam musime zalozit komponentu, ulozit ji do mapy a loadnout vzorec
         CCollRulesByC* NewColl=new CCollRulesByC();
         mapRulByC.insert(make_pair(string(*Command),NewColl));
         LoadCommand(Command, NewColl);
         return NewColl;
    }
    else
    {
        return mapRulByC[Command->c_str()];
    }

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CRulesAnalyzer::GetRulesByC\n";
(*log)<< "---------------------------------------------\n";
} //End Debug        
    
}
