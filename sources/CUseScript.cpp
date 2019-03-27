#include <stdio.h>

// US_UseScript.cpp : Implementation of CUS_UseScript
#include "CUseScript.h"

//MARKS
const char BEGIN_OF_STRING=       'S';
const char END_OF_STRING=         'E';
const char BEGIN_OF_COMMENT=      'C';
const char END_OF_COMMENT=        'W';
const char BEGIN_OF_DIRECT=       'D';
const char END_OF_DIRECT=         'X';
const char BEGIN_OF_METHOD=       'F';
const char END_OF_METHOD=         'Y';

const char UNRECOGNIZED=          'x';
const char EXPRESSION=            'H';

const char MARK_LEFT_PARENTHESIS= 'L';   
const char MARK_RIGHT_PARENTHESIS='R';   
const char MARK_COMMA=            'V';  
const char MARK_EQUAL=            'T';  
const char MARK_GREATER=          'G';  
const char MARK_LESS=             'I';  
const char MARK_NOEQUALS=         'N';  
const char MARK_AND=              'A';
const char MARK_OR=               '0';
const char MARK_PLUS=             'P';
const char MARK_MINUS=            'M';
const char MARK_MULTIPLICATION=   'K';
const char MARK_DIVISION=         'Z';
const char MARK_ASSIGNING=        'B';

//GROUPS CHARACTERS
char* DIRECT_NEIGHBOURHOOD=     "\r\n\t ;'";
char* METHOD_NEIGHBOURHOOD=     "\r\n\t ()*/+-=;";
char* LINE_SEPARATORS=          "\r\n;";
char* TRIMED=                   "\t ";
char SPACE=                    ' ';
char TABULATOR=                '\t';
char ESCAPE=                   '\\';

//EXPRESSION
char* E_LEFT_PARENTHESIS=         "(";
char* E_RIGHT_PARENTHESIS=        ")";
char* E_COMMA=                    ",";
char* E_EQUAL=                    "==";
char* E_GREATER=                  ">";
char* E_LESS=                     "<";
char* E_NOEQUALS=                 "!=";
char* E_AND=                      "&";
char* E_OR=                       "|";
char* E_PLUS=                     "+";
char* E_MINUS=                    "-";
char* E_MULTIPLICATION=           "*";
char* E_DIVISION=                 "/";
char* E_ASSIGNING=                "=";

//KEYWORDS
const string KEYWORD_IF=       "If";
const string KEYWORD_THEN=     "Then";
const string KEYWORD_ELSE=     "Else";
const string KEYWORD_ENDIF=    "Endif";
const string KEYWORD_FOR=      "For";
const string KEYWORD_TO=       "To";
const string KEYWORD_NEXT=     "Next";
const string KEYWORD_WHILE=    "While";
const string KEYWORD_WEND=     "Wend";
const string KEYWORD_EXIT=     "Exit";



/***********************************************************************
 *
 * FUNCTION:     OrdinalForm
 *
 * DESCRIPTION:  Tato funkce pøevede èíslo na string doplnene nulami na delku size v ordinalni podobe napr 005
 *               Je-li size nula cislo dopleno nulami nebude.
 *
 * PARAMETERS:   (IN)	int number - cislo k prevedeni
 *               (IN)	cint size - delka ordinalni podoby
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::SetLog(ofstream* lg, int LogAllMes)
{
	log=lg;
	LogAll=LogAllMes;
	return 0;
}

int CUseScript::SplitString(string* strSplit, vector<string*>* outArray, char* Separator)
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

string CUseScript::OrdinalForm(int number, int size)
{
    string myNumber;
    char buffer[20];
    
    sprintf(buffer,"%d",number);
    myNumber=buffer;
    if (size!=0) {
        for (int j=myNumber.length();j<size;j++)
            myNumber="0"+myNumber;      
    }
    return myNumber;
}

void CUseScript::TrimLeft(string* sStr, char* ArraySep)
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

void CUseScript::TrimRight(string* sStr, char* ArraySep)
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

/***********************************************************************
 *
 * FUNCTION:     CopyConstArrayExpression
 *
 * DESCRIPTION:  Naklonuje Pole trid M_EXPRESSION_ELEMENT
 *
 * PARAMETERS:   (IN)	CPtrArray* SourceArray - zdrojové pole
 *               (OUT)	CPtrArray* NewArray - naklonované pole
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::CopyConstArrayExpression(vector<M_EXPRESSION_ELEMENT *>* SourceArray, vector<M_EXPRESSION_ELEMENT *>* NewArray)
{
    int i;
    M_EXPRESSION_ELEMENT *mysouEl;
    M_EXPRESSION_ELEMENT *mynewEl;

    for (i=0;i<NewArray->size();i++)
        delete (*NewArray)[i];
    NewArray->clear();
    for (i=0;i<SourceArray->size();i++)
    {
        mysouEl= (*SourceArray)[i];
        mynewEl=new M_EXPRESSION_ELEMENT; 
        if (mysouEl->iType==EX_INTEGER)
        {
            mynewEl->iIntValue=mysouEl->iIntValue;
            mynewEl->sName=new string("");
        }
        else
        {
            mynewEl->sName=new string(*(mysouEl->sName));
        }
        mynewEl->iDepth=mysouEl->iDepth;
        mynewEl->iType=mysouEl->iType;
        NewArray->insert(NewArray->end(),mynewEl);
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     DebugProgNodes
 *
 * DESCRIPTION:  Serializuje pole pole trid M_PROG_NODE(uzlù programu) do citelné podoby do souboru
 *
 * PARAMETERS:   (IN)   CPtrArray * aProgNodes - pole trid M_PROG_NODE(uzlu programu)
 *               (IN)	string sFileName - Cesta k souboru do které se má pole serializovat
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

void CUseScript::DebugProgNodes(vector<M_PROG_NODE *>* aProgNodes, char* sFileName)
{
        FILE *fw;
        if ((fw=fopen(sFileName,"w"))== NULL);
           //(*log)<< "Soubor se nepodarilo otevrit\n";
        string myNumber;
        for(int i=0;i<aProgNodes->size();i++)
        {
            M_PROG_NODE* mNode = (*aProgNodes)[i];

            myNumber=OrdinalForm(i,5);
            fwrite(myNumber.c_str(),1,myNumber.length(),fw);
            fwrite(" - ",1,3,fw);
            
            switch(mNode->shType)
            {
            case D_IF_WITH_THEN:  
                fwrite("D_IF_WITH_THEN         ",1,23,fw);
                break;
            case D_IF_WITH_THEN_AND_ELSE: 
                fwrite("D_IF_WITH_THEN_AND_ELSE",1,23,fw);
                break;
            case D_FOR:
                fwrite("D_FOR                  ",1,23,fw);
                break;
            case D_WHILE:
                fwrite("D_WHILE                ",1,23,fw);
                break;
            case D_IF:
                fwrite("D_IF                   ",1,23,fw);
                break;
            case D_NEXT:
                fwrite("D_NEXT                 ",1,23,fw);
                break;
            case D_WEND:
                fwrite("D_WEND                 ",1,23,fw);
                break;
            case D_ELSE:
                fwrite("D_ELSE                 ",1,23,fw);
                break;
            case D_ENDIF:
                fwrite("D_ENDIF                ",1,23,fw);
                break;
            case D_COMMAND:
                fwrite("D_COMMAND              ",1,23,fw);
                break;    
            }
            fwrite(" - ",1,3,fw);

            myNumber=OrdinalForm(mNode->iTrueJump,5);
            fwrite(myNumber.c_str(),1,myNumber.length(),fw);
            fwrite(" - ",1,3,fw);

            myNumber=OrdinalForm(mNode->iFalseJump,5);
            fwrite(myNumber.c_str(),1,myNumber.length(),fw);
            fwrite(" - ",1,3,fw);

            //(*log)<< *(mNode->sCommand);
            fwrite(mNode->sCommand->c_str(),1,mNode->sCommand->length(),fw);
            fwrite(" - ",1,3,fw);
            fwrite(mNode->sQuestion->c_str(),1,mNode->sQuestion->length(),fw);
            

            fwrite("\n",1,1,fw);
        }

        fclose(fw);
}

/***********************************************************************
 *
 * FUNCTION:     SerializeProgNodes
 *
 ***********************************************************************/

string* CUseScript::SerializeProgNodes()
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CUseScript::SerializeProgNodes\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    

        string* outString =new string();
		int i;
		int j;
        for(i=0;i<Code->ProgNodeArray.size();i++)
        {
            M_PROG_NODE* mNode = Code->ProgNodeArray[i];
            //Length
			int Size= 6 * 6 + mNode->sQuestion->length() + mNode->sCommand->length();
			*(outString) += OrdinalForm(Size,6);
			if (mNode->shType >D_EXIT)
              *(outString) += OrdinalForm(D_COMMAND,6);
            else
              *(outString) += OrdinalForm(mNode->shType,6);
            if (mNode->iTrueJump > 999999)
              *(outString) += OrdinalForm(0,6);
            else
              *(outString) += OrdinalForm(mNode->iTrueJump,6);  
              
            if (mNode->iFalseJump > 999999)
              *(outString) += OrdinalForm(0,6);
            else
              *(outString) += OrdinalForm(mNode->iFalseJump,6);
            
            *(outString) += OrdinalForm(mNode->sCommand->length(),6);
            *(outString) += OrdinalForm(mNode->sQuestion->length(),6);
            *(outString) += *(mNode->sCommand);
            *(outString) += *(mNode->sQuestion);
            
            if (LogAll) { //Debug
              (*log)<< "outString:" << *(outString) <<" \n";
            } //End Debug 
        }
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CUseScript::SerializeProgNodes\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
        return outString;
}

int CUseScript::SetSerializeProgNodes(string* SerString)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CUseScript::SetSerializeProgNodes\n";
(*log)<< "SerString:" << *SerString << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug            
    int ptrSize=0;       
	
	while( ptrSize < SerString->length() )
	{	
	       //prectu casti
	       string length =	SerString->substr(ptrSize,6);	       
           int ActualLength = atoi(length.c_str());
	       if (LogAll) { (*log)<< "Size :" << ActualLength <<" \n"; }
	       if (ActualLength == 0) {
	          if (LogAll) { (*log)<< "NULA" <<" \n"; }
	          return 1;
	       }
	       int ptr = ptrSize + 6;
	       
	       M_PROG_NODE * NewNode = new M_PROG_NODE();
	       //TYPE
	       string type = SerString->substr(ptr,6);
           NewNode->shType = atoi(type.c_str());

           //TRUE JUMP
           ptr+=6;
	       string truejump = SerString->substr(ptr,6);
           NewNode->iTrueJump = atoi(truejump.c_str());
	       
	       //FALSE JUMP
           ptr+=6;
	       string falsejump = SerString->substr(ptr,6);
           NewNode->iFalseJump = atoi(falsejump.c_str());
	       
	       //LENGHT COMMAND
	       ptr+=6;
	       string lengthCommand = SerString->substr(ptr,6);
           int CommandLength = atoi(lengthCommand.c_str());
	       
	       //LENGHT QUESTION
	       ptr+=6;
	       string lengthQuestion = SerString->substr(ptr,6);
           int QuestionLength = atoi(lengthQuestion.c_str());
	       
	       //COMMAND
	       ptr+=6;
	       NewNode->sCommand = new string(SerString->substr(ptr,CommandLength));
	       
	       //QUESTION
	       ptr+=CommandLength;
	       NewNode->sQuestion = new string(SerString->substr(ptr,QuestionLength));
	       
	       Code->ProgNodeArray.push_back(NewNode);
	       
	       if (LogAll) { //Debug
(*log)<< "\n";
(*log)<< "NewNode->shType: " << NewNode->shType << "\n";
(*log)<< "NewNode->iTrueJump: " << NewNode->iTrueJump << "\n";
(*log)<< "NewNode->iFalseJump: " << NewNode->iFalseJump << "\n";
(*log)<< "NewNode->sCommand: " << *(NewNode->sCommand) << "\n";
(*log)<< "NewNode->sQuestion: " << *(NewNode->sQuestion) << "\n";
} //End Debug   
	       
           ptrSize += ActualLength;
        }

	if (PrepareNodes(&(Code->ProgNodeArray)))
       return intError;
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CUseScript::SetSerializeProgNodes\n";
(*log)<< "---------------------------------------------\n";
} //End Debug             
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     DebugExpression
 *
 * DESCRIPTION:  Serializuje pole trid M_EXPRESSION_ELEMENT(entit výrazu) do citelné podoby do souboru
 *
 * PARAMETERS:   (IN)   CPtrArray * outarrElements - pole trid 
 *               (IN)	string sFileName - Cesta k souboru do které se má pole serializovat
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/


void CUseScript::DebugExpression(vector<M_EXPRESSION_ELEMENT *>* outarrElements, char* sFileName)
{
    
       FILE *fw;
        if ((fw=fopen(sFileName,"w"))== NULL);
           //(*log)<< "Soubor se nepodarilo otevrit\n";
        string myNumber;

        for(int i=0;i<outarrElements->size();i++)
        {
            M_EXPRESSION_ELEMENT* myElement= (*outarrElements)[i];
            
            switch(myElement->iType)
            {
            case EX_OPERATOR:  
                fwrite("EX_OPERATOR    ",1,15,fw);
                break;
            case EX_VARIABLE: 
                fwrite("EX_VARIABLE    ",1,15,fw);
                break;
            case EX_ASSIGNING:
                fwrite("EX_ASSIGNING   ",1,15,fw);
                break;
            case EX_METHOD:
                fwrite("EX_METHOD      ",1,15,fw);
                break;
            case EX_STRING:
                fwrite("EX_STRING      ",1,15,fw);
                break;
            case EX_INTEGER:
                fwrite("EX_INTEGER     ",1,15,fw);
                break;  
            case EX_COMMA:
                fwrite("EX_COMMA       ",1,15,fw);
                break;    
            case EX_EMPTYMETHOD:
                fwrite("EX_EMPTYMETHOD ",1,15,fw);
                break;    
            }

            fwrite(" - ",1,3,fw);

            myNumber=OrdinalForm(myElement->iDepth,5);
            fwrite(myNumber.c_str(),1,myNumber.length(),fw);
            fwrite(" - ",1,3,fw);
            
            if (myElement->iType==EX_INTEGER) 
                {
                myNumber=OrdinalForm(myElement->iIntValue,7);
                fwrite(myNumber.c_str(),1,myNumber.length(),fw);
                }
            else
                fwrite(myElement->sName->c_str(),1,myElement->sName->length(),fw);

            fwrite("\n",1,1,fw);
        }

        fclose(fw);
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

int CUseScript::MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd)
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


/***********************************************************************
 *
 * FUNCTION:     MarkKeyword
 *
 * DESCRIPTION:  funkce vezme analyzovany text a vyhleda v nìm slovo sSearchString a oznaèí
 *               je ve stejnì dlouhém stringu strMark identifikatory chMarkBegin a chMarkEnd.
 *               Klíèové slovo oznaèí pouze v pøípadì, že se v jeho bezprostøedním okolí nachází
 *               pouze znaky z množiny chNeib.
 *
 * PARAMETERS:   (IN)   string* strAnalyzeText - analyzovany text
 *               (OUT)	string* strMarks - oznaèkovaný text
 *               (IN)	string sSearchString - hledaný zaèátek
 *               (IN)	char chMarkBegin - znacka zacatku
 *               (IN)	char chMarkEnd - znacka konce
 *               (IN)	char* chNeib - množina znakù, které se mohou kolem klíèového slova nacházet
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::MarkKeyword(string* strAnalyzeText, string* strMarks, string sSearchString, char chMarkBegin,  char chMarkEnd, char* chNeib)
{
    int iCounter=0;
    int iPossitionStart;
    int iPossitionStop;
    int iPossitionStartSpace;
    int iPossitionStopSpace;
    string strWithSpaces;
    string BeforeS;
    while (iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(sSearchString,iCounter);
        if (iPossitionStart==(-1)) //Žádný øetìzec nenalezen
            return 0;
        
        if (iPossitionStart!=0)
            iPossitionStartSpace=iPossitionStart-1;
        else
            iPossitionStartSpace=0;
        
        iPossitionStop=iPossitionStart+sSearchString.length();
        if (iPossitionStop!=strAnalyzeText->length())
            iPossitionStopSpace=iPossitionStop+1;
        else
            iPossitionStopSpace=iPossitionStop;

        strWithSpaces=strAnalyzeText->substr(iPossitionStartSpace,iPossitionStopSpace-iPossitionStartSpace);
        TrimLeft(&strWithSpaces,chNeib);
        TrimRight(&strWithSpaces,chNeib);
        BeforeS =strAnalyzeText->substr(iPossitionStart,iPossitionStop-iPossitionStart);
        if (strWithSpaces==BeforeS)
            {
            (*strMarks)[iPossitionStart]=chMarkBegin; //Zacatek retezce
            (*strMarks)[iPossitionStart+sSearchString.length()-1]=chMarkEnd;
            }
        iCounter=iPossitionStart+sSearchString.length()+1;
    }
    return 0;
}


/***********************************************************************
 *
 * FUNCTION:     MarkSigns
 *
 * DESCRIPTION:  funkce vezme analyzovany text a vyhleda v nìm retezec *sign(tøeba +,-,&&) a oznaèí
 *               je ve stejnì dlouhém stringu strMark identifikatory chMarkSign.
 *
 * PARAMETERS:   (IN)   string* strAnalyzeText - analyzovany text
 *               (OUT)	string* strMarks - oznaèkovaný text
 *               (IN)	char *sign- hledané znamínko
 *               (IN)	char chMarkSign - znacka retezce
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/


int CUseScript::MarkSigns(string* strAnalyzeText, string* strMark, char *sign, char chMarkSign)
{
    string strSigns=sign;
    int iCounter=0;
    int iPossitionStart;
    int i;
    while(iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(strSigns,iCounter);
        if (iPossitionStart==(-1))
            break;
        
        for (i=0;i<strSigns.length();i++)
            (*strMark)[iPossitionStart+i]=chMarkSign;
        iCounter=iPossitionStart+i;
    }       
    return 0;
}

int EscapeToInt(string Text)
{
    int Sor=8;
    int Rad=2;
    int Returned=0;
    for(int i=0;i<Text.length();i++)
    {
        int Add=(-1);
        switch(Text[i])
        {
            case 'x':
                Sor=16;
                Rad=1;
                break;
            case '0':
                Add=0;
                break;
            case '1':
                Add=1;
                break;
            case '2':
                Add=2;
                break;
            case '3':
                Add=3;
                break;
            case '4':
                Add=4;
                break;
            case '5':
                Add=5;
                break;
            case '6':
                Add=6;
                break;
            case '7':
                Add=7;
                break;
            case '8':
                Add=8;
                break;
            case '9':
                Add=9;
                break;
            case 'A':
                Add=10;
                break;
            case 'B':
                Add=11;
                break;
            case 'C':
                Add=12;
                break;
            case 'D':
                Add=13;
                break;
            case 'E':
                Add=14;
                break;
            case 'F':
                Add=15;
                break;
        }
        if (Add!=(-1))
        {
            int Moc=1;
            for(int j=0;j<Rad;j++)
                Moc*=Sor;
            Returned+=Moc*Add;
            Rad--;
        }
        else
        {
            if (Text[0]!='x')
            {
                return -1;
            }
        }
    }
    return Returned;
}

int CUseScript::EcsapeString(string *strText,char Escape)
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

/***********************************************************************
 *
 * FUNCTION:     EscapeSequention
 *
 * DESCRIPTION:  funkce nahradi escape sekvence jejioch hodnotami
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::EscapeSequention(string* strAnalyzeText, string* strMark, char MarkString, char Escape)
{
    for(int i=strMark->length()-1;i>(-1);i--)
        if ((*strMark)[i]==MarkString)
        {
            if ((*strAnalyzeText)[i]==Escape)
            {
                if (i<strMark->length()-1)
                { 
                 switch((*strAnalyzeText)[i+1])
                    {
                    case 'n':
                        (*strAnalyzeText)[i]='\n';
                        strAnalyzeText->erase(i+1,1);
                        strMark->erase(i+1,1);
                        break;
                    case 'r':
                        (*strAnalyzeText)[i]='\r';
                        strAnalyzeText->erase(i+1,1);
                        strMark->erase(i+1,1);
                        break;
                    case '\\':
                        (*strAnalyzeText)[i]='\\';
                        strAnalyzeText->erase(i+1,1);
                        strMark->erase(i+1,1);
                        break;
                    case 't':
                        (*strAnalyzeText)[i]='\t';
                        strAnalyzeText->erase(i+1,1);
                        strMark->erase(i+1,1);
                        break;
                    }
                }
                if (i<strMark->length()-3)
                {
                    int NumS=EscapeToInt(strAnalyzeText->substr(i+1,3));
                    if (NumS>0)
                    {
                        (*strAnalyzeText)[i]=NumS;
                        strAnalyzeText->erase(i+1,3);
                        strMark->erase(i+1,3);
                    }
                }
            }
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

int CUseScript::FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd)
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
 * FUNCTION:     DeleteSection
 *
 * DESCRIPTION:  funkce vezme analyzovaný text a tam kde jsou v markovaném stringu 
 *               znamínka rovna chMarkSection smaže jak v analyzovaném tak markovacím stringu 
 *
 * PARAMETERS:   (IN)   string* strAnalyzeText - analyzovaný text
 *               (OUT)	string* strMarks - omarkovaný string
 *               (IN)	char chMarkSection - hledaná znaèka
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::DeleteSection(string* strAnalyzeText, string* strMarks, char chMarkSection)
{
    for(int i=strMarks->length()-1;i>(-1);i--)
        if ((*strMarks)[i]==chMarkSection)
            {
            strAnalyzeText->erase(i,1);
            strMarks->erase(i,1);
            }
    return 0;
}


/***********************************************************************
 *
 * FUNCTION:     SeparateLines
 *
 * DESCRIPTION:  funkce vezme analyzovaný a omarkovaný text a vytvoøí pole analyzovaných
 *               a omarkovaných vzájemnì si odpovídajících stringù splitovaných podle jednoho ze znakù v
 *               chLineSeparators
 *
 * PARAMETERS:   (OUT)   stringArray* arroutLines - vysledne pole
 *               (OUT)	 stringArray* arroutMarkLines - vysledne pole
 *               (IN)	 string* strAnalyzeText - analyzovaný text
 *               (IN)	 string* strMark - omarkovaný text
 *               (IN)    char * chLineSeparators - separatory podle kterých se bude splitovat
 *               (IN)    char chEmpty - znak niceho v markovacím stringu
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

void CUseScript::SeparateLines(vector<string *> *arroutLines, vector<string *> *arroutMarkLines,  string* strAnalyzeText, string* strMark, char * chLineSeparators, char chEmpty)
{
    int iPossitionStart;
    int iDefinitivePossitionStart;
    int iLastPossition=0;
    int iCounter=0;
    string strSeparators=chLineSeparators;
    while (iCounter<strAnalyzeText->length())
    {
        iDefinitivePossitionStart=-1;
        for (int i=0;i<strSeparators.length();i++)
        {
            iPossitionStart=strAnalyzeText->find(strSeparators[i],iCounter);
            if (iPossitionStart!=(-1)) 
            {
                if ((iPossitionStart<iDefinitivePossitionStart) ||  iDefinitivePossitionStart==(-1))
                    iDefinitivePossitionStart=iPossitionStart;
            }
        }
        if (iDefinitivePossitionStart==(-1)) //Žádný øetìzec nenalezen
            break;

        if ((*strMark)[iDefinitivePossitionStart]==chEmpty)
        {
            string* NewStr=new string(strAnalyzeText->substr(iLastPossition,iDefinitivePossitionStart-iLastPossition));
            arroutLines->push_back(NewStr);
            string* NewStr2= new string(strMark->substr(iLastPossition,iDefinitivePossitionStart-iLastPossition));
            arroutMarkLines->push_back(NewStr2);
            iLastPossition=iDefinitivePossitionStart+1;
        }
       
        iCounter=iDefinitivePossitionStart+1;
        
    }

    string* NewH=new string(strAnalyzeText->substr(iLastPossition,strAnalyzeText->length()-iLastPossition));
    arroutLines->push_back(NewH);
    string* NewH2=new string(strMark->substr(iLastPossition,strMark->length()-iLastPossition));
    arroutMarkLines->push_back(NewH2);
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

void CUseScript::CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements)
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

/***********************************************************************
 *
 * FUNCTION:     NextCommand
 *
 * DESCRIPTION:  funkce vrátí index dalšího výskytu sekce typu FindMark v poli ptrMarkElements
 *
 * PARAMETERS:   (IN)    stringArray *ptrMarkElements - markovací stringy delky jedna
 *               (IN)	 int Index - index od kterého se má zaèít
 *               (IN)	 char FindMark - hledaný typ sekce
 *
 * RETURNED:     -1 - retezec nenalezen
 *               jinak index pole
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/


int CUseScript::NextCommand(vector<string *> *ptrMarkElements, int Index, char FindMark)
{
    string sFindMark;
    for(int i=Index;i<ptrMarkElements->size();i++)
    {
    /*    (*log)<< "<NextCommand>\n";
        (*log)<< *(*ptrMarkElements)[i] + "\n" ;
        (*log)<< FindMark;
        (*log)<< "\n<NextCommand>\n";         */
        string* my=(*ptrMarkElements)[i];
        if ((*my)[0]==FindMark)
        {
            return i;
        }
    }
    return -1;
}
/***********************************************************************
 *
 * FUNCTION:     JoinSections
 *
 * DESCRIPTION:  funkce spojí sekce od indexu IndexBegin do indexu IndexEnd a vráti je jako retezec
 *
 * PARAMETERS:   (IN)    stringArray *ptrElements - pole sekcí
 *               (IN)	 int IndexBegin - index od kterého se má zaèít
 *               (IN)	 int IndexEnd - index konce
 *
 * RETURNED:     string - slouèený øetìzec
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

string* CUseScript::JoinSections(vector<string *> *ptrElements, int IndexBegin, int IndexEnd)
{
    string* OutString=new string("");
    int i;
    for(i=IndexBegin;i<IndexEnd;i++)
    {
        (*OutString)+=*((*ptrElements)[i]);
    }
    return OutString;
}

/***********************************************************************
 *
 * FUNCTION:     NodesFromLines
 *
 * DESCRIPTION:  funkce pøidá do pole aProgNodes uzly programu z øádky programu
 *
 * PARAMETERS:   (OUT)  CPtrArray *aProgNodes - pole sekcí
 *               (IN)   string* strCommand - jedna øádka kódu
 *               (IN)   string* strMarkCommand - omarkovaná jedna øádka kódu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::NodesFromLines(vector<M_PROG_NODE *> *aProgNodes, string* strCommand, string* strMarkCommand)
{
    vector<string *> aElements;
    vector<string *> aMarkElements;
    
    string* sTrim;
    string* TempString;
    //rozsekam podle typu
    CutCommandByType(strCommand,strMarkCommand,&aElements,&aMarkElements);

    //odstranim prazdne stringy
    int i;

    for (i=aElements.size()-1; i > (-1);i--)
    {
        sTrim = aElements[i];
        TrimLeft(sTrim,TRIMED);
        TrimRight(sTrim,TRIMED);
        if (*sTrim=="")
        {
            delete aElements[i];
            delete aMarkElements[i];
            aElements.erase(aElements.begin()+i);
            aMarkElements.erase(aMarkElements.begin()+i);
        }
    }
  /*
    for(int i=0;i<aElements.size();i++)
    {
       (*log)<< "D" + *(aElements[i]) + "-" + *(aMarkElements[i])+ "\n";
    }
    (*log)<<"\n";
  */

    if ((*(aMarkElements[0]))[0]==BEGIN_OF_DIRECT)
    {
        M_PROG_NODE* newNode = new M_PROG_NODE;
        aProgNodes->push_back(newNode);
        
        //IF
        if (*(aElements[0])==KEYWORD_IF)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);

            if (NextC==(-1)) //Není Then
            {
                intError=ERR_IF_WITHOUT_THEN;
                intLineError=aProgNodes->size();
                return 1;
            }
            if (*(aElements[NextC])!=KEYWORD_THEN) //Nejaky command je ale není to Then
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            newNode->sQuestion=JoinSections(&aElements,1,NextC); //Mezi if a then
            newNode->sCommand=new string("");
            if (*(newNode->sQuestion)=="") //Prázná otázka
            {
                intError=ERR_EMPTY_QUESTION;
                intLineError=aProgNodes->size();
                return 1;
            }

            int NextC2=NextCommand(&aMarkElements,NextC+1,BEGIN_OF_DIRECT); //Hledám další direct

            if (NextC2!=(-1)) //Nìjaký nenalezen 
            {
                if (*(aElements[NextC2])!=KEYWORD_ELSE) //Když to neni else tak blbý
                {    
                    intError=ERR_INVALID_COMBINATION_KEYWORDS;
                    intLineError=aProgNodes->size();
                    return 1;
                }

                TempString=JoinSections(&aElements,NextC+1,NextC2); //Toto je mezi Then a Else

                if (*(TempString)=="") //Neco by tam byt melo
                {
                    intError=ERR_EMPTY_COMMAND;
                    intLineError=aProgNodes->size();
                    return 1;
                }

                M_PROG_NODE* newNodeNext = new M_PROG_NODE; //Neco tam je vytvorime novy radek programu
                aProgNodes->push_back(newNodeNext);
                newNodeNext->sCommand = TempString;
                newNodeNext->sQuestion=new string("");
                newNodeNext->shType = D_COMMAND;
                newNode->shType=D_IF_WITH_THEN_AND_ELSE ;
                
                //jeste kdyby tam byl dalsi direct tak blby
                int NextC3=NextCommand(&aMarkElements,NextC2+1,BEGIN_OF_DIRECT);

                if (NextC3!=(-1))
                {    
                    intError=ERR_INVALID_COMBINATION_KEYWORDS;
                    intLineError=aProgNodes->size();
                    return 1;
                }

                TempString=JoinSections(&aElements,NextC2+1,aMarkElements.size()); //Mezi Else a koncem

                M_PROG_NODE* newNodeNextNext = new M_PROG_NODE; //Neco tam je vytvorime novy radek programu
                aProgNodes->push_back(newNodeNextNext);
                newNodeNextNext->sCommand = TempString;
                newNodeNextNext->sQuestion=new string("");
                newNodeNextNext->shType = D_COMMAND;
            }
            else
            {
                TempString=JoinSections(&aElements,NextC+1,aMarkElements.size());
                if (*(TempString)!="")
                {       
                    M_PROG_NODE* newNodeNext = new M_PROG_NODE;
                    aProgNodes->push_back(newNodeNext);
                    newNodeNext->sCommand = TempString;
                    newNodeNext->sQuestion = new string("");
                    newNode->shType=D_IF_WITH_THEN;
                }
                else
                {
                    newNode->shType=D_IF;
                    delete TempString;
                }
            }
        }

        //ELSE
        if (*(aElements[0])==KEYWORD_ELSE)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (NextC!=(-1)) 
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }
            string *JS=JoinSections(&aElements,1,aElements.size());
            if (*JS!="")
            {
               delete JS;
               intError=ERR_KEYWORD_NOT_ALONE;
               intLineError=aProgNodes->size();
               return 1;
            }
            delete JS;
            newNode->shType=D_ELSE;
            newNode->sCommand=new string("");
            newNode->sQuestion=new string("");
        }

        //END IF
        if (*(aElements[0])==KEYWORD_ENDIF)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (NextC!=(-1)) 
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }
            if (aElements.size()>1)
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }
            string *JS=JoinSections(&aElements,1,aElements.size());
            if (*JS!="")
            {
               delete JS;
               intError=ERR_KEYWORD_NOT_ALONE;
               intLineError=aProgNodes->size();
               return 1;
            }
            delete JS;
            newNode->shType=D_ENDIF;
            newNode->sCommand=new string("");
            newNode->sQuestion=new string("");
        }

        //FOR
        if (*(aElements[0])==KEYWORD_FOR)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (NextC==(-1)) //Není To
            {
                intError=ERR_FOR_WITHOUT_TO;
                intLineError=aProgNodes->size();
                return 1;
            }

            if (*(aElements[NextC])!=KEYWORD_TO) //Nejaky command je ale není to To
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            newNode->sCommand=JoinSections(&aElements,1,NextC); //Mezi For a To Pred samotnym forem jeste inicializacni radek
            newNode->shType=D_COMMAND;
            newNode->sQuestion=new string("");

            if (*(newNode->sCommand)=="") //Prázná otázka
            {
                intError=ERR_EMPTY_INICIALIZE_FOR;
                intLineError=aProgNodes->size();
                return 1;
            }

            int NextC2=NextCommand(&aMarkElements,NextC+1,BEGIN_OF_DIRECT); //Hledám další direct

            if (NextC2!=(-1)) //Nakej dalsi keyword na radce
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            TempString=JoinSections(&aElements,NextC+1,aElements.size());

            if (*TempString=="")
            {       
                delete TempString;
                intError=ERR_EMPTY_QUESTION_FOR;
                intLineError=aProgNodes->size();
                return 1;
            }

            M_PROG_NODE* newNodeNext = new M_PROG_NODE;
            aProgNodes->push_back(newNodeNext);
            newNodeNext->shType=D_FOR;
            newNodeNext->sQuestion=TempString; 
            newNodeNext->sCommand=new string("");
        }

        //NEXT
        if (*(aElements[0])==KEYWORD_NEXT)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);

            if (NextC!=(-1)) 
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            if (aElements.size()>1)
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }
            string* JS=JoinSections(&aElements,1,aElements.size());
            if (*JS!="")
            {
               delete JS;
               intError=ERR_KEYWORD_NOT_ALONE;
               intLineError=aProgNodes->size();
               return 1;
            }
            delete JS;
            newNode->shType=D_NEXT;
            newNode->sCommand=new string("");
            newNode->sQuestion=new string("");
        }


        //WHILE
        if (*(aElements[0])==KEYWORD_WHILE)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (NextC!=(-1)) 
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            TempString=JoinSections(&aElements,1,aElements.size());

            if (*TempString=="")
            {
                intError=ERR_EMPTY_QUESTION_WHILE;
                intLineError=aProgNodes->size();
                return 1;
            }

            newNode->sQuestion=TempString;
            newNode->shType=D_WHILE;
            newNode->sCommand=new string("");
        }

        //WEND
        if (*(aElements[0])==KEYWORD_WEND)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (aElements.size()>1)
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }


            if (NextC!=(-1)) 
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }
            string * JS= JoinSections(&aElements,1,aElements.size());
            if (*JS!="")
            {
               delete JS;
               intError=ERR_KEYWORD_NOT_ALONE;
               intLineError=aProgNodes->size();
               return 1;
            }
            delete JS;
            newNode->shType=D_WEND;
            newNode->sCommand=new string("");
            newNode->sQuestion=new string("");
        }
        //EXIT
        if (*(aElements[0])==KEYWORD_EXIT)
        {
            int NextC=NextCommand(&aMarkElements,1,BEGIN_OF_DIRECT);
            
            if (aElements.size()>1)
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            if (NextC!=(-1))
            {
                intError=ERR_INVALID_COMBINATION_KEYWORDS;
                intLineError=aProgNodes->size();
                return 1;
            }

            string * JS=JoinSections(&aElements,1,aElements.size());
            if (*JS!="")
            {
               delete JS;
               intError=ERR_KEYWORD_NOT_ALONE;
               intLineError=aProgNodes->size();
               return 1;
            }
            delete JS;
            newNode->shType=D_EXIT;
            newNode->sCommand=new string("");
            newNode->sQuestion=new string("");
        }

    }
    else
    {
        TempString=JoinSections(&aElements,0,aElements.size());
        if (*TempString!="")
            {
            M_PROG_NODE* newNode = new M_PROG_NODE;
            aProgNodes->push_back(newNode);
            newNode->shType=D_COMMAND; 
            newNode->sCommand =TempString; 
            newNode->sQuestion=new string("");
            }
       else
            {
              delete TempString;
            }
    }

    for(i=0;i<aElements.size();i++)
    {
       delete aElements[i];
       delete aMarkElements[i];
    }

    return 0;
}


/***********************************************************************
 *
 * FUNCTION:     MakeNodes
 *
 * DESCRIPTION:  funkce poslupne volá funkci NodesFromLines pro každý øádek kódu a vyhazuje prázné øádky
 *
 * PARAMETERS:   (OUT)  CPtrArray *aProgNodes - pole uzlù programu
 *               (IN)   stringArray* aLines - pole øádkù kódu
 *               (IN)   stringArray *aMarkLines - pole omarkovaných øádkù kódu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::MakeNodes(vector<M_PROG_NODE *> *aProgNodes, vector<string *>* aLines, vector<string *> *aMarkLines)
{
   for (int i=0;i<aLines->size();i++)
    {
        string sLine=*((*aLines)[i]);
        TrimLeft(&sLine,TRIMED);
        TrimRight(&sLine,TRIMED);
        if (sLine!="")
        {                
            if (NodesFromLines(aProgNodes, (*aLines)[i], (*aMarkLines)[i])==1)
                return 1;
        }
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     GetPtrNode - ukazatel z pole tøíd M_PROG_NODE na tøídu o indexu iIndex
 *
 ***********************************************************************/

M_PROG_NODE* CUseScript::GetPtrNode(vector<M_PROG_NODE *>* aProgNodes, int iIndex)
{
    M_PROG_NODE* pActualNode = (M_PROG_NODE*) (*aProgNodes)[iIndex];
    return pActualNode;
}

/***********************************************************************
 *
 * FUNCTION:     LinkNodes
 *
 * DESCRIPTION:  funkce vezne pole Uzlù programu a propojí je skoky - tj. vypní iTrueJump a iFalseJump
 *
 * PARAMETERS:   (OUT)  CPtrArray *aProgNodes - pole uzlù programu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::LinkNodes(vector<M_PROG_NODE *> *aProgNodes)
{
    vector<int> aStack;
    vector<int> aStackTypes;
    int iNode;
    int i;

    for(i=0;i<aProgNodes->size();i++)
    {
        M_PROG_NODE* pActualNode = GetPtrNode(aProgNodes,i);
        M_PROG_NODE* pNextNode;
        M_PROG_NODE* pForNode;
        M_PROG_NODE* pWhileNode;
        M_PROG_NODE* pIfNode;
        M_PROG_NODE* pElseNode;
        
        pActualNode->iFalseJump=0;
        pActualNode->iTrueJump=0;

        switch(pActualNode->shType)
        {
            case D_IF_WITH_THEN:         
                pActualNode->iTrueJump=i+1;
                pActualNode->iFalseJump=i+2;
                break;
            case D_IF_WITH_THEN_AND_ELSE:
                pActualNode->iTrueJump=i+1;
                pActualNode->iFalseJump=i+2;
                pNextNode = GetPtrNode(aProgNodes,i+1);
                delete pNextNode->sQuestion;
                pNextNode->sQuestion=new string("1");
                pNextNode->iTrueJump=i+3; 
				pNextNode->iFalseJump=0; 
				i++;
                break;
            case D_FOR:
                aStack.push_back(i);
                aStackTypes.push_back(D_FOR);
                break;
            case D_WHILE:
                aStack.push_back(i);
                aStackTypes.push_back(D_WHILE);
                break;
            case D_IF:
                aStack.push_back(i);
                aStackTypes.push_back(D_IF);
                break;
            case D_NEXT:
                if (aStackTypes.size()==0)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                if (aStackTypes[aStackTypes.size()-1]!=D_FOR)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                iNode=aStack[aStackTypes.size()-1];
                pForNode = GetPtrNode(aProgNodes,iNode);
                delete pActualNode->sQuestion;
                pActualNode->sQuestion=new string("1");
                pActualNode->iTrueJump=iNode; 
                pForNode->iTrueJump=iNode+1;
                pForNode->iFalseJump=i+1;
                aStack.pop_back();
                aStackTypes.pop_back();
                break;
            case D_WEND:
                if (aStackTypes.size()==0)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                if (aStackTypes[aStackTypes.size()-1]!=D_WHILE)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                iNode=aStack[aStackTypes.size()-1];
                pWhileNode = GetPtrNode(aProgNodes,iNode);
                delete pActualNode->sQuestion;
                pActualNode->sQuestion=new string("1");
                pActualNode->iTrueJump=iNode; 
                pWhileNode->iTrueJump=iNode+1;
                pWhileNode->iFalseJump=i+1;
                aStack.pop_back();
                aStackTypes.pop_back();
                break;
            case D_ELSE:
                if (aStackTypes.size()==0)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                if (aStackTypes[aStackTypes.size()-1]!=D_IF)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                aStack.push_back(i);
                aStackTypes.push_back(D_ELSE);
                break;
            case D_ENDIF:
                {
                if (aStackTypes.size()==0)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                int UpperStack=aStackTypes[aStackTypes.size()-1];
                if (UpperStack!=D_IF && UpperStack!=D_ELSE)
                {
                   intError=ERR_KEYWORDS_CROSS;
                   intLineError=i;
                   return 1;
                }
                if (UpperStack==D_IF)
                {
                    iNode=aStack[aStackTypes.size()-1];
                    pIfNode = GetPtrNode(aProgNodes,iNode);
                    pIfNode->iTrueJump=iNode+1;
                    pIfNode->iFalseJump=i+1; 
                    aStack.pop_back();
                    aStackTypes.pop_back();
                }
                if (UpperStack==D_ELSE)
                {
                    if (aStackTypes.size()==1)
                    {
                       intError=ERR_KEYWORDS_CROSS;
                       intLineError=i;
                       return 1;
                    }
                    int iElseNode=aStack[aStackTypes.size()-1];
                    int iIfNode=aStack[aStackTypes.size()-2];
                    pIfNode = GetPtrNode(aProgNodes, iIfNode);
                    pElseNode = GetPtrNode(aProgNodes, iElseNode);
                    pIfNode->iTrueJump=iIfNode+1;
                    pIfNode->iFalseJump=iElseNode+1;
                    delete pElseNode->sQuestion;
                    pElseNode->sQuestion=new string("1");
                    pElseNode->iTrueJump=i+1; 
                    aStack.pop_back();
                    aStackTypes.pop_back();
                    aStack.pop_back();
                    aStackTypes.pop_back();
                }
                }
                break;
            case D_EXIT:
                pActualNode->iTrueJump=100000;
                delete pActualNode->sQuestion;
                pActualNode->sQuestion=new string("1");
                break;
        }
    }
    if (aStackTypes.size()>0)
    {
       intError=ERR_KEYWORDS_NOT_CLOSED;
       intLineError=i;
       return 1;
    }

    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     AddNextExpresionElement - pøidá další element do pole výrazu
 *
 ***********************************************************************/

M_EXPRESSION_ELEMENT* CUseScript::AddNextExpresionElement(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int Depth, int Type, string* Name)
{
    M_EXPRESSION_ELEMENT* myElement=new M_EXPRESSION_ELEMENT;
    myElement->iDepth =Depth;
    myElement->iType = Type;
    myElement->sName = Name;
    outarrElements->push_back(myElement);
    return myElement;
}

/***********************************************************************
 *
 * FUNCTION:     AddNextExpresionIntElement - pøidá další element do pole výrazu
 *
 ***********************************************************************/

M_EXPRESSION_ELEMENT* CUseScript::AddNextExpresionIntElement(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int Depth, int Type, int intValue)
{
    M_EXPRESSION_ELEMENT* myElement=new M_EXPRESSION_ELEMENT;
    myElement->iDepth =Depth;
    myElement->iType = Type;
    myElement->iIntValue = intValue;
    myElement->sName = new string("");
    outarrElements->push_back(myElement);
    return myElement;
}

/***********************************************************************
 *
 * FUNCTION:     AnalyzeExpression
 *
 * DESCRIPTION:  funkce vytvoøí pole elementù výrazu sExpression
 *
 * PARAMETERS:   (OUT)  CPtrArray * outarrElements - pole elementù(M_EXPRESSION_ELEMENT) výrazu
 *               (OUT)  int* MaxDepth - maximální nalazené závorkové vnoøení napø. ((sd)) by bylo 2            
 *               (IN)  string* sExpression - analyzovaný výraz
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::AnalyzeExpression(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int* MaxDepth, string* sExpression)
{
    string strMarkExp;
    string* m;
    vector<string *> arrMarkElements;
    vector<string *> arrElements;
    int i;
    //Vyrobit markovaci string
    for(i=0;i<sExpression->length();i++)
            strMarkExp+="x";
    //Oznacit stringy
    MarkSections(sExpression,&strMarkExp,'"','"',BEGIN_OF_STRING,END_OF_STRING,1);
    
    //Odstarnit mezery ale ne ve stringach
    int CutOn=1;

    for(i=sExpression->length()-1;i>(-1);i--)
    {
        if (strMarkExp[i]==END_OF_STRING) CutOn=0;
        if (strMarkExp[i]==BEGIN_OF_STRING) CutOn=1;
        if (CutOn)
            if ((*sExpression)[i]==SPACE ||  (*sExpression)[i]==TABULATOR)
            {
                sExpression->erase(i,1);
                strMarkExp.erase(i,1);
            }
    }


    //Oznacit operátory,rovnítka,závorky a carky...
    MarkSigns(sExpression,&strMarkExp,E_LEFT_PARENTHESIS,MARK_LEFT_PARENTHESIS);
    MarkSigns(sExpression,&strMarkExp,E_RIGHT_PARENTHESIS,MARK_RIGHT_PARENTHESIS);
    MarkSigns(sExpression,&strMarkExp,E_ASSIGNING,MARK_ASSIGNING);
    MarkSigns(sExpression,&strMarkExp,E_COMMA,MARK_COMMA);
    MarkSigns(sExpression,&strMarkExp,E_EQUAL,MARK_EQUAL);
    MarkSigns(sExpression,&strMarkExp,E_GREATER,MARK_GREATER);
    MarkSigns(sExpression,&strMarkExp,E_LESS,MARK_LESS);
    MarkSigns(sExpression,&strMarkExp,E_NOEQUALS,MARK_NOEQUALS);
    MarkSigns(sExpression,&strMarkExp,E_AND,MARK_AND);
    MarkSigns(sExpression,&strMarkExp,E_OR,MARK_OR);
    MarkSigns(sExpression,&strMarkExp,E_PLUS,MARK_PLUS);
    MarkSigns(sExpression,&strMarkExp,E_MINUS,MARK_MINUS);
    MarkSigns(sExpression,&strMarkExp,E_MULTIPLICATION,MARK_MULTIPLICATION);
    MarkSigns(sExpression,&strMarkExp,E_DIVISION,MARK_DIVISION);

    //vyplnit stringy
    FillMarkSection(&strMarkExp,BEGIN_OF_STRING,END_OF_STRING);
    //rozssekani
    CutCommandByType(sExpression,&strMarkExp,&arrElements,&arrMarkElements);
    
    //Vytvoreni Pole elementù výrazu
    int ActualDepth=0;
    *MaxDepth=0;
    for (i=0;i<arrElements.size();i++)
    {
        switch ((*(arrMarkElements[i]))[0])
        {
        case BEGIN_OF_STRING:
            m=new string(arrElements[i]->substr(1,arrElements[i]->length()-2));
            AddNextExpresionElement(outarrElements,ActualDepth,EX_STRING,m);
            break;
        case MARK_LEFT_PARENTHESIS:   
            ActualDepth+=arrElements[i]->length();
            if ((*MaxDepth)<ActualDepth) *MaxDepth=ActualDepth;
            break;
        case MARK_RIGHT_PARENTHESIS:
            ActualDepth-=arrElements[i]->length();
            if (ActualDepth<0)
            {   
                intError=ERR_MORE_CLOSE_PARENTHESIS;
                intLineError=i;
                return 1;
            }
            break;
        case MARK_ASSIGNING:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_ASSIGNING,new string(""));
            break;
        case MARK_COMMA:
            if (ActualDepth-1<0)
            {
               intError=ERR_MORE_CLOSE_PARENTHESIS;
               intLineError=i;
               return 1;
            }
            AddNextExpresionElement(outarrElements,ActualDepth-1,EX_COMMA,new string(""));
            break;
        case MARK_EQUAL:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_EQUAL));
            break;
        case MARK_GREATER:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_GREATER));
            break;
        case MARK_LESS:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_LESS));
            break;
        case MARK_NOEQUALS:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_NOEQUALS));
            break;
        case MARK_AND:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_AND));
            break;
        case MARK_OR:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_OR));
            break;
        case MARK_PLUS:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_PLUS));
            break;
        case MARK_MINUS:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_MINUS));
            break;
        case MARK_MULTIPLICATION:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_MULTIPLICATION));
            break;
        case MARK_DIVISION:
            AddNextExpresionElement(outarrElements,ActualDepth,EX_OPERATOR,new string(E_DIVISION));
            break;
        case UNRECOGNIZED:
            int tNumber=atoi(arrElements[i]->c_str());
            if (tNumber==0)
            {
                if (*(arrElements[i])=="0") //je to nula
                   AddNextExpresionIntElement(outarrElements,ActualDepth,EX_INTEGER,0);
                else    //je to promenna nebo nazev funkce
                {
                if (i==arrElements.size()-1) //je to promenna
                   AddNextExpresionElement(outarrElements,ActualDepth,EX_VARIABLE,new string(*(arrElements[i])));
                else
                   if ((*(arrMarkElements[i+1]))[0]==MARK_LEFT_PARENTHESIS) //je to funkce
                      if (i==arrElements.size()-2)
                        {
                            intError=ERR_MORE_CLOSE_PARENTHESIS;
                            intLineError=i;
                            return 1;
                        }
                      else
                        if ((*(arrMarkElements[i+2]))[0]==MARK_RIGHT_PARENTHESIS)
                            AddNextExpresionElement(outarrElements,ActualDepth,EX_EMPTYMETHOD,new string(*(arrElements[i])));
                        else
                            AddNextExpresionElement(outarrElements,ActualDepth,EX_METHOD,new string(*(arrElements[i])));
                         
                   else //je to promenna
                      {
                      AddNextExpresionElement(outarrElements,ActualDepth,EX_VARIABLE,new string(*(arrElements[i])));
                       }
                }
            }
            else //je to cislo
                AddNextExpresionIntElement(outarrElements,ActualDepth,EX_INTEGER,tNumber);
            break;
        }
    }
    for(i=0;i<arrElements.size();i++)
    {
       delete arrElements[i];
       delete arrMarkElements[i];
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     GetVariableValue
 *
 * DESCRIPTION:  funkce vrátí hodnotu promenné uložených v asociativním poli gVariables
 *
 * PARAMETERS:   M_EXPRESSION_ELEMENT* Variable - element v jehož sName je nazev promenne
 *                                               a v jehož iIntValue nebo sName se objevi hodnota promenne
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::GetVariableValue(M_EXPRESSION_ELEMENT* Variable)
{
if (LogAll) { //Debug
(*log)<< "GetVariable:" << *(Variable->sName) << " - ";
} //End Debug

    M_EXPRESSION_ELEMENT* Varb;
    if (gVariables->count(*(Variable->sName))==0)
    {
        //intError=ERR_VARIABLE_NOT_EXIST;
        //return 1;        
        Variable->iType=EX_STRING;
        *(Variable->sName)="";
    }
    else
    {
        Varb=(*gVariables)[*(Variable->sName)];
        Variable->iType=Varb->iType;
        if (Varb->iType==EX_INTEGER)
            Variable->iIntValue=Varb->iIntValue;  
        if (Varb->iType==EX_STRING)
        {
            *(Variable->sName)=*(Varb->sName);  
        }
        if (LogAll) { //Debug
//        if (Varb->iType==EX_INTEGER)
//           (*log)<< "Return:(INT)" << Variable->iIntValue << "\n";
//        if (Varb->iType==EX_STRING)
//           (*log)<< "Return:(STR)" << *(Variable->sName) << "\n";    
        } //End Debug
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     SetVariableValue
 *
 * DESCRIPTION:  funkce nastaví hodnotu promenné uložených v asociativním poli gVariables
 *               v pøípadì že neexistuje, založí ji a dá ji defaultní hosnotu.
 *
 * PARAMETERS:   (IN)  string ValueName - element v jehož sName je nazev promenne
 *               (IN)  M_EXPRESSION_ELEMENT* pValue - element v nemž je uložena nová hodnota
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::SetVariableValue(string* ValueName, M_EXPRESSION_ELEMENT* pValue)
{
    M_EXPRESSION_ELEMENT* Varb;

    if (LogAll) { //Debug
    (*log)<< "SetVariable:" << *ValueName << " - ";    
    if (pValue->iType==EX_INTEGER)
           (*log)<< "(INT)" << pValue->iIntValue << "\n";
    if (pValue->iType==EX_STRING)
           (*log)<< "(STR)" << *(pValue->sName) << "\n";    
    } //End Debug

    //je v hash mape tato promenna? - tak ji smaz
    if (gVariables->count(*ValueName)>0)
    {
        Varb = (*gVariables)[*ValueName];        
    }
    else
    {
        Varb=new M_EXPRESSION_ELEMENT();
        gVariables->insert(make_pair(string(*ValueName),Varb));
        Varb->sName=new string("");
    }
    Varb->iType=pValue->iType;
    if (pValue->iType==EX_INTEGER)
    {
        Varb->iIntValue=pValue->iIntValue;
        *(Varb->sName)="";
    }
    if (pValue->iType==EX_STRING)
    {
        *(Varb->sName)=*(pValue->sName);
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     CalculateOperation
 *
 * DESCRIPTION:  funncke provede operaci mezi One Two a vysledek uloží do Two
 *
 * PARAMETERS:   (IN)  M_EXPRESSION_ELEMENT* One - element 1
 *               (IN)  string Operator - operator
 *               (IN/OUT)  M_EXPRESSION_ELEMENT* Two - element 2
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/


int CUseScript::CalculateOperation(M_EXPRESSION_ELEMENT* One, string Operator, M_EXPRESSION_ELEMENT* Two)
{
    //Result
    //Prelozeni promennych na hodnoty
    if (One->iType==EX_VARIABLE)
        if (GetVariableValue(One))
        {
            intError=ERR_VARIABLE_NOT_EXIST;
            return 1;
        }
    if (Two->iType==EX_VARIABLE)
        if (GetVariableValue(Two))
        {
            intError=ERR_VARIABLE_NOT_EXIST;
            return 1;
        }
    //Operace samotná
    if (Operator==E_EQUAL)
    {
        if (One->iType!=Two->iType)
        {
            Two->iIntValue=0; 
        }      
        if (One->iType==EX_STRING)
        {
            if ((*One->sName) == (*Two->sName))
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }       
        if (One->iType==EX_INTEGER)
        {
            if (One->iIntValue == Two->iIntValue)
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }
        Two->iType=EX_INTEGER;
    }
    if (Operator==E_GREATER)
    { 
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
        {
            if (One->iIntValue > Two->iIntValue)
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }
        else
        {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
        }
    }
    if (Operator==E_LESS)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
        {
            if (One->iIntValue < Two->iIntValue)
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }
        else
        {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
        }
    }
    if (Operator==E_NOEQUALS)
    {
       if (One->iType!=Two->iType)
        {
            Two->iIntValue=0;
        }      
        if (One->iType==EX_STRING)
        {
            if (*(One->sName) == *(Two->sName))
                Two->iIntValue=0; 
            else
                Two->iIntValue=1; 
        }       
        if (One->iType==EX_INTEGER)
        {
            if (One->iIntValue == Two->iIntValue)
                Two->iIntValue=0; 
            else
                Two->iIntValue=1; 
        }
        Two->iType=EX_INTEGER;
    }
    if (Operator==E_AND)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
        {
            if (One->iIntValue>0 && Two->iIntValue>0)
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }
        else
        {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
        }
    }
    if (Operator==E_OR)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
        {
            if (One->iIntValue>0 || Two->iIntValue>0)
                Two->iIntValue=1; 
            else
                Two->iIntValue=0; 
        }
        else
        {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
        }
    }
    if (Operator==E_PLUS)
    {
        if (One->iType==EX_STRING && Two->iType==EX_STRING)
            *(Two->sName)=*(One->sName)+*(Two->sName);
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
            Two->iIntValue=One->iIntValue+Two->iIntValue;  //!!!!!Pozor na preteceni Integeru
        if (One->iType==EX_INTEGER && Two->iType==EX_STRING)
            *(Two->sName)=OrdinalForm(One->iIntValue,0)+*(Two->sName);
        if (One->iType==EX_STRING && Two->iType==EX_INTEGER)
        {
            *(Two->sName)=*(One->sName)+OrdinalForm(Two->iIntValue,0);
            Two->iType=EX_STRING; 
        }
    }
    if (Operator==E_MINUS)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
            Two->iIntValue=One->iIntValue-Two->iIntValue;  //!!!!!Pozor na preteceni Integeru
        else
            {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
            }
    }
    if (Operator==E_MULTIPLICATION)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
            Two->iIntValue=One->iIntValue*Two->iIntValue;  //!!!!!Pozor na preteceni Integeru
        else
            {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
            }
    }
    if (Operator==E_DIVISION)
    {
        if (One->iType==EX_INTEGER && Two->iType==EX_INTEGER)
        {    
            if (Two->iIntValue==0)
            {
                intError=ERR_DIVISION_BY_ZERO;
                intLineError=0;
                return 1;
            }
            Two->iIntValue=One->iIntValue/Two->iIntValue;  //!!!!!Pozor na preteceni Integeru a deleni nulou
        }
        else
            {
            intError=ERR_MATHEMATIC_OPERATE_STRINGS;
            intLineError=0;
            return 1;
            }
    }
    return 0;
}

/***********************************************************************
 * pomocné funkce
 ***********************************************************************/

int CUseScript::GetExpElementType(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iIndex)
{
    M_EXPRESSION_ELEMENT* pActualNode = (*outarrElements)[iIndex];
    return pActualNode->iType;
}

string* CUseScript::GetExpElementName(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iIndex)
{
    M_EXPRESSION_ELEMENT* pActualNode = (*outarrElements)[iIndex];
    return pActualNode->sName;
}

int CUseScript::IntExpElementDepth(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iIndex)
{
    M_EXPRESSION_ELEMENT* pActualNode = (*outarrElements)[iIndex];
    return pActualNode->iDepth;
}

int CUseScript::GetExpElementIntValue(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iIndex)
{
    M_EXPRESSION_ELEMENT* pActualNode = (*outarrElements)[iIndex];
    return pActualNode->iIntValue;
}

/***********************************************************************
 *
 * FUNCTION:     SectionCalculateByOperator
 *
 * DESCRIPTION:  funkce provede výpoèet sekcí mezi BeginIndex a EndIndex pomocí páru operátorù napø +,-
 *
 * PARAMETERS:   (IN)  CPtrArray * outarrElements - pole elementù výrazu
 *               (IN)  int BeginIndex 
 *               (IN)  int EndIndex
 *               (IN)  char *Operator1
 *               (IN)  char *Operator2
 *               (OUT) int *Removed - kolik zmizelo elementù výpoètem
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::SectionCalculateByOperator(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int BeginIndex, int EndIndex, char *Operator1, char *Operator2, int *Removed)
{
    M_EXPRESSION_ELEMENT* myElement1;
    M_EXPRESSION_ELEMENT* myElement2;
    M_EXPRESSION_ELEMENT* myElement3;

    //musi byt vse zkontrolovane
    int iRemoved=0;
    int Flour=0;
    for (int i=BeginIndex;i<EndIndex;i=i+2)
    {
        myElement2=(*outarrElements)[BeginIndex+1+Flour];
        if (myElement2->iType==EX_OPERATOR && ((*(myElement2->sName)==Operator1)||(*(myElement2->sName)==Operator2)))
        {
            myElement1=(*outarrElements)[BeginIndex+Flour];
            myElement3=(*outarrElements)[BeginIndex+2+Flour];
            if (CalculateOperation(myElement1,*(myElement2->sName),myElement3))
                return 1;
            //DebugExpression(outarrElements,"D:\\huh1.txt");

            delete (*outarrElements)[BeginIndex+Flour];
            delete  (*outarrElements)[BeginIndex+Flour+1];
            outarrElements->erase(outarrElements->begin() + BeginIndex+Flour,outarrElements->begin() + BeginIndex+Flour+2);
            //DebugExpression(outarrElements,"D:\\huh.txt");
            iRemoved+=2;
        }
        else
            Flour=Flour+2;
    }
    *Removed=EndIndex-iRemoved;
    return 0;
} 

/***********************************************************************
 *
 * FUNCTION:     SectionCalculate
 *
 * DESCRIPTION:  funkce provede výpoèet sekce mezi BeginIndex a EndIndex
 *               podle všech operátoru a volá fukci SectionCalculateByOperator 
 *               podle jejich priority
 *
 * PARAMETERS:   (IN)  CPtrArray * outarrElements - pole elementù výrazu
 *               (IN)  int BeginIndex 
 *               (IN)  int EndIndex
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::SectionCalculate(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int BeginIndex, int EndIndex)
{
    int iRemoved=EndIndex;
    //calcutate by operator priority
    if (iRemoved>BeginIndex) if (SectionCalculateByOperator(outarrElements,BeginIndex,EndIndex,E_MULTIPLICATION,E_DIVISION,&iRemoved)==1) return 1;
    if (iRemoved>BeginIndex) if (SectionCalculateByOperator(outarrElements,BeginIndex,iRemoved,E_PLUS,E_MINUS,&iRemoved)==1) return 1;
    if (iRemoved>BeginIndex) if (SectionCalculateByOperator(outarrElements,BeginIndex,iRemoved,E_EQUAL,E_NOEQUALS,&iRemoved)==1) return 1;
    if (iRemoved>BeginIndex) if (SectionCalculateByOperator(outarrElements,BeginIndex,iRemoved,E_LESS,E_GREATER,&iRemoved)==1) return 1;
    if (iRemoved>BeginIndex) if (SectionCalculateByOperator(outarrElements,BeginIndex,iRemoved,E_AND,E_OR,&iRemoved)==1) return 1;
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     CallMethod
 *
 * DESCRIPTION:  funkce volá událost v klientovi komponenty a pøedává mu kolekci
 *               parametrù. Návratovou hodnotu v prvním prvku kolekce vrací jako
 *               M_EXPRESSION_ELEMENT
 *
 * PARAMETERS:   (IN)  string MethodName - název volané funkce
 *               (IN)  CPtrArray * arguments - pole argumentù funcke
 *
 * RETURNED:     M_EXPRESSION_ELEMENT hodnota vrácená funkcí
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

M_EXPRESSION_ELEMENT* CUseScript::CallMethod(string* MethodName, vector<M_EXPRESSION_ELEMENT *>* arguments)
{
    return ptrFunction(ptrClientClass, MethodName,arguments);
}

/***********************************************************************
 *
 * FUNCTION:     CompileMethod
 *
 * DESCRIPTION:  funkce vyhledá funkce v sekcích mezi BeginIndex a EndIndex, zavolá je
 *               a výsledek zaøadí do pole elementù 
 *
 * PARAMETERS:   (IN/OUT)  CPtrArray * outarrElements - pole elementù výrazu
 *               (IN)      int BeginIndex 
 *               (IN)      int EndIndex
 *               (OUT)     int *Removed - kolik zmizelo elementù výrazu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::CompileMethod(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int BeginSection, int EndSection,int *Removed)
{
    vector<M_EXPRESSION_ELEMENT *> MethodArgArray;
    int i;
    int j;

    for (i=EndSection;i>BeginSection-1;i--)
    {
        MethodArgArray.clear();
        if(GetExpElementType(outarrElements,i)==EX_METHOD)
        {
            //kam az sahaj carky
            int iCounter=0;
            
            if (i==EndSection)
            {
                intError=ERR_UNDEFINED;
                return 1;
            }
            for(j=i+2;j<EndSection+1;j=j+2)
            {
                if (GetExpElementType(outarrElements,j)!=EX_COMMA)
                    break;
                iCounter++;
            }
            
            for(j=i+1;j<(i+iCounter*2+2);j=j+2)
            {
                if (GetExpElementType(outarrElements,j)!=EX_STRING && GetExpElementType(outarrElements,j)!=EX_INTEGER && GetExpElementType(outarrElements,j)!=EX_VARIABLE)
                {
                    intError=ERR_UNDEFINED;
                    return 1;
                }
                else
                {
                    MethodArgArray.push_back((*outarrElements)[j]);
                }
            }
            M_EXPRESSION_ELEMENT* retValue=CallMethod(GetExpElementName(outarrElements,i),&MethodArgArray);
            if (retValue->iType==EX_ERROR) 
            {
                intError=ERR_METHOD_RETURNED_ERROR;
                return 1;
            }
            if (retValue->iType==EX_STRING)
            {
                M_EXPRESSION_ELEMENT* el=(*outarrElements)[i];
                *(el->sName)=*(retValue->sName);//tady
                el->iType=EX_STRING;
            }
            if (retValue->iType==EX_INTEGER)
            {
                M_EXPRESSION_ELEMENT* el=(*outarrElements)[i];
                el->iIntValue=retValue->iIntValue;
                el->iType=EX_INTEGER;
            }
            for (int y=i+1;y<i+iCounter*2+2;y++)
                delete (M_EXPRESSION_ELEMENT*) (*outarrElements)[y];
            outarrElements->erase(outarrElements->begin() + i+1,outarrElements->begin() + i+1 + iCounter*2+1); //???
            *Removed+=iCounter*2+1;
            delete retValue;
        }
        if(GetExpElementType(outarrElements,i)==EX_EMPTYMETHOD)
        {
            M_EXPRESSION_ELEMENT* retValue=CallMethod(GetExpElementName(outarrElements,i),&MethodArgArray);
            if (retValue->iType==EX_ERROR) 
            {
                intError=ERR_METHOD_RETURNED_ERROR;
                return 1;
            }
            if (retValue->iType==EX_STRING)
            {
                M_EXPRESSION_ELEMENT* el=(*outarrElements)[i];
                *(el->sName)=*(retValue->sName); //tady
                el->iType=EX_STRING;
            }
            if (retValue->iType==EX_INTEGER)
            {
                M_EXPRESSION_ELEMENT* el=(*outarrElements)[i];
                el->iIntValue=retValue->iIntValue;
                el->iType=EX_INTEGER;
            }
            delete retValue;
        }
       // DebugExpression(outarrElements,"method.txt");
    }
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     GetNextSection
 *
 * DESCRIPTION:  funkce vyhledá další skupinu sekcí k analýze v aktuální hloubce výrazu. Jesliže
 *               již žádnou sekci s hloubkou  iDepth nenajde vrací nula
 *
 * PARAMETERS:   (IN/OUT)  CPtrArray * outarrElements - pole elementù výrazu
 *               (IN)      int iDepth - hledaná hloubka 
 *               (OUT)     int* BeginSection - zacátek nalezené sekce
 *               (OUT)     int* EndSection- konec nalezené sekce
 *
 * RETURNED:     0 - sekce nalezena
 *               1 - žádné sekce v hloubce iDepth nejsou
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::GetNextSection(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iDepth, int* BeginSection, int* EndSection)
{
    int iDepthLast=-1;

    for (int i=0;i<outarrElements->size();i++)
    {
        if(IntExpElementDepth(outarrElements,i)==iDepth && iDepthLast!=iDepth)
        {
            *BeginSection=i;
            iDepthLast=iDepth;
        }
        if(IntExpElementDepth(outarrElements,i)!=iDepth && iDepthLast==iDepth)
        {
            *EndSection=i-1;
            return 1;
        }
    }
    if (iDepthLast==(-1))
        return 0;
    else
    {
        *EndSection=outarrElements->size()-1; //?????????
        return 1;
    }
    
}

/***********************************************************************
 *
 * FUNCTION:     ProcessExpression
 *
 * DESCRIPTION:  funkce analyzuje celý výraz. Buï je jejím výstupem jeden element (èíslo nebo
 *               žetìzec nebo jsou jejím výstupem tøi elementy pak je to pøiøazovací výraz
 *               (a=2)
 *
 * PARAMETERS:   (IN/OUT)  CPtrArray * outarrElements - pole elementù výrazu
 *               (IN)      int iDepth - nejvetsí hloubka po analýze - uchovávaná v PROG_NODES 
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/


int CUseScript::ProcessExpression(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iDepth)
{
    int IsComma;
    int IsAssign=0;
    int BeginSection;
    int EndSection;
    
    while (iDepth>(-1))
    {
        IsComma=0;
        //DebugExpression(outarrElements,"D:\\kuk.txt");
        if (GetNextSection(outarrElements,iDepth,&BeginSection,&EndSection)==1)
        {         
          //je to prirazovaci vyraz? musi byjt na druhym miste a musi to bejt uroven nula
            if (BeginSection+1<outarrElements->size())
            {
                if (GetExpElementType(outarrElements,BeginSection+1)==EX_ASSIGNING)
                {
                    if (iDepth!=0)
                    {
                        intError=ERR_ASSIGNING_NOT_IN_ROOT;
                        return 1;
                    }
                    if (GetExpElementType(outarrElements,BeginSection)!=EX_VARIABLE)
                    {
                        intError=ERR_BEFORE_ASSIGNING_NOT_VARIABLE;
                        return 1;
                    }
                    IsAssign=1;
                    BeginSection+=2;
                }
            }

            if ((EndSection < BeginSection))
            {
                    intError=ERR_UNDEFINED;
                    return 1;
            }
            //odstranim funkce
            int iRemoved=0;
            if (CompileMethod(outarrElements,BeginSection,EndSection,&iRemoved)==1)
                return 1;
            EndSection=EndSection-iRemoved;

            if ((EndSection < BeginSection))
            {
                    intError=ERR_UNDEFINED;
                    return 1;
            }
            if (((EndSection-BeginSection)%2)==1) //sudy pocet prvku! muze to byt vyraz typu -1 tj na zacatek nulu
            {
                M_EXPRESSION_ELEMENT* myNewEl=new M_EXPRESSION_ELEMENT;
                myNewEl->iDepth=iDepth;
                myNewEl->iIntValue=0;
                myNewEl->sName=new string("");
                myNewEl->iType=EX_INTEGER; 
                outarrElements->insert(outarrElements->begin()+BeginSection,myNewEl);
                EndSection++;
            }
            //DebugExpression(outarrElements,"D:\\kik.txt");
            //ted to zkontroluju musi to jit cislo operator cislo operator ...
            for (int i=BeginSection;i<EndSection+1;i=i+2)
            {
                if (GetExpElementType(outarrElements,i)!=EX_STRING && GetExpElementType(outarrElements,i)!=EX_INTEGER && GetExpElementType(outarrElements,i)!=EX_VARIABLE)
                {
                    intError=ERR_UNDEFINED;
                    return 1;
                }
                if (i<EndSection) //muze se jednat o jednoelementovy vyraz
                {
                    if (GetExpElementType(outarrElements,i+1)!=EX_OPERATOR)
                    {
                        intError=ERR_UNDEFINED;
                        return 1;
                    }
                }
            }   
            //kdyz to bude jedna nemusi se nic volat
            if ((EndSection-BeginSection)>0) //???
            {
                if (SectionCalculate(outarrElements,BeginSection,EndSection)==1)
                    return 1;
                //ted snizit hloubku u zbyleho prvku
                M_EXPRESSION_ELEMENT* myEl=(*outarrElements)[BeginSection];
                myEl->iDepth=iDepth-1;
                if (IsAssign) break;
            }
            else
            {
                M_EXPRESSION_ELEMENT* myCommaEl=(*outarrElements)[BeginSection];
                if (myCommaEl->iType==EX_VARIABLE)
                {
                    GetVariableValue(myCommaEl);
                }
                myCommaEl->iDepth=iDepth-1; 
                if (IsAssign) break;
                //DebugExpression(outarrElements,"debug.txt");
            }
        }
        else
        {
            iDepth--;
        }
    }
    //tak tady uz by to melo bejt vypocitany :-)
    //takze napø. bud a=3 nebo 3
    //DebugExpression(outarrElements,"return.txt");
    return 0;
}

/***********************************************************************
 *
 * FUNCTION:     PrepareNodes
 *
 * DESCRIPTION:  funkce provede analýzu všech výrazù v programu a uloží je do polí CommandExpression
 *               a QuestionExpression
 *
 * PARAMETERS:   (IN/OUT)  CPtrArray *aProgNodes - pole uzlù programu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::PrepareNodes(vector<M_PROG_NODE *> *aProgNodes)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CUseScript::PrepareNodes\n";
(*log)<< "---------------------------------------------\n";
} //End Debug  

    M_PROG_NODE* PNode = 0;
    M_PROG_NODE* PNode2 = 0;
    int NextIsFor=0;
    int i;
    //Projdu vsechny uzly programu a naanalyzuju jim vyrazy jak commandy tak questiony
    for (i=0;i<aProgNodes->size();i++)
    {
  if (LogAll) { //Debug
  (*log)<< "i=" << i;
  } //End Debug      
        NextIsFor=0;
        PNode=GetPtrNode(aProgNodes,i);
        if (i!=aProgNodes->size()-1)
        {
            PNode2=GetPtrNode(aProgNodes,i+1);
            if (PNode2->shType==D_FOR )
            {
                NextIsFor=1;
                if (*(PNode->sCommand)=="")
                {
                   intError=ERR_UNDEFINED;
                   return 1;
                }
                //For je dosud v podobe NODE1: i=0; NODE2: FOR Q:"10" C:""
                PNode->sCommand->append(E_MINUS);
                PNode->sCommand->append("1");
            }
        }
       if (*(PNode->sCommand)!="")
       {
            //(*log)<< "Analyze command" << i;
            if (AnalyzeExpression(&(PNode->CommandExpression),&(PNode->MaxDepthCommand),PNode->sCommand))
                return 1;
            //(*log)<< "End Analyze command" << i;
            //DebugExpression(&(PNode->CommandExpression),"d:\\out.txt");
        }
        if (*(PNode->sQuestion)!="")
        {
            if (AnalyzeExpression(&(PNode->QuestionExpression),&(PNode->MaxDepthQuestion),PNode->sQuestion))
                return 1;
            //DebugExpression(&(PNode->QuestionExpression),"d:\\out2.txt");
        }

        if (NextIsFor) 
        {
            if (PNode->CommandExpression.size()<3)
            {
               intError=ERR_UNDEFINED;
               return 1;
            } 
            if (GetExpElementType(&(PNode->CommandExpression),0)!=EX_VARIABLE || GetExpElementType(&(PNode->CommandExpression),1)!=EX_ASSIGNING)
            {
               intError=ERR_UNDEFINED;
               return 1;
            }
            string* mVariable=GetExpElementName(&(PNode->CommandExpression),0);
            PNode2->sCommand->append(*(mVariable) + E_ASSIGNING + *(mVariable) + E_PLUS + "1");
            (*PNode2->sQuestion)=(*(mVariable) + E_LESS + *(PNode2->sQuestion) + E_PLUS + "1");
        }

    }
    return 0;
} 


/***********************************************************************
 *
 * FUNCTION:     Execute
 *
 * DESCRIPTION:  bìh programu
 *
 * PARAMETERS:   (IN/OUT)  CPtrArray *aProgNodes - pole uzlù programu
 *
 * RETURNED:     0 - OK
 *               1 - FALSE
 *
 * AUTOR:        Jindøich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CUseScript::Execute(vector<M_PROG_NODE *>* aProgNodes)
{
 intError=0;
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: Execute\n";
(*log)<< "---------------------------------------------\n";
} //End Debug

    M_PROG_NODE* PNode;
    vector<M_EXPRESSION_ELEMENT *> ActualExpression;
    int iJump=0;
    
    while(iJump<aProgNodes->size())
    {
        PNode=GetPtrNode(aProgNodes,iJump);
       // (*log)<< *PNode->sCommand + "\n";
       // (*log)<< *PNode->sQuestion + "\n";
        //Command
        if ((*PNode->sCommand)!="")
        {
           CopyConstArrayExpression(&(PNode->CommandExpression),&ActualExpression);
           if (ProcessExpression(&ActualExpression,PNode->MaxDepthCommand))
                return 1;
            if (ActualExpression.size()==3)
                SetVariableValue(GetExpElementName(&ActualExpression,0),(M_EXPRESSION_ELEMENT*) ActualExpression[2]);
            for (int d=0;d<ActualExpression.size();d++)
                delete (M_EXPRESSION_ELEMENT*) ActualExpression[d];
            ActualExpression.clear();
        }
        //Question
        if ((*PNode->sQuestion)!="")
        {
            CopyConstArrayExpression(&(PNode->QuestionExpression),&ActualExpression);
            if (ProcessExpression(&ActualExpression,PNode->MaxDepthQuestion))
                return 1;
            if (ActualExpression.size()==3)
            {
                SetVariableValue(GetExpElementName(&ActualExpression,0),(M_EXPRESSION_ELEMENT*) ActualExpression[2]);
                if (GetExpElementType(&ActualExpression,2)!=EX_INTEGER)
                {
                    intError=ERR_STRING_IN_QUESTION;
                    return 1;
                }
                if (GetExpElementIntValue(&ActualExpression,2)>0)
                    iJump=PNode->iTrueJump;
                else
                    iJump=PNode->iFalseJump;
            }
            else
            {
                if (GetExpElementType(&ActualExpression,0)!=EX_INTEGER)
                {
                    intError=ERR_STRING_IN_QUESTION;
                    return 1;
                }
                if (GetExpElementIntValue(&ActualExpression,0)>0)
                    iJump=PNode->iTrueJump;
                else
                    iJump=PNode->iFalseJump;
            }
            for (int d=0;d<ActualExpression.size();d++)
                delete (M_EXPRESSION_ELEMENT*) ActualExpression[d];
            ActualExpression.clear();
        }
        else
            iJump++;
    }
    for (int d=0;d<ActualExpression.size();d++)
       delete (M_EXPRESSION_ELEMENT*) ActualExpression[d];
    ActualExpression.clear();

if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: Execute\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CUseScript


int CUseScript::MarkCode(string *strAnalyzeText, string *strMark)
{
    int i;
    *strMark="";
    for(i=0;i<strAnalyzeText->length();i++)
            (*strMark)+=UNRECOGNIZED;
    //(*log)<< *strMark;
    //komentáøe, stringy
    MarkSections(strAnalyzeText,strMark,'\'','\n',BEGIN_OF_COMMENT,END_OF_COMMENT,0);
    MarkSections(strAnalyzeText,strMark,'"','"',BEGIN_OF_STRING,END_OF_STRING,1);
    //klíèová slova
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_IF,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_THEN,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_ELSE,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_ENDIF,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_FOR,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_TO,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_NEXT,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_WHILE,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_WEND,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
    MarkKeyword(strAnalyzeText,strMark,KEYWORD_EXIT,BEGIN_OF_DIRECT,END_OF_DIRECT,DIRECT_NEIGHBOURHOOD);
   /* //metody
    for(i=0;i<aMethods.size();i++)
      MarkKeyword(strAnalyzeText,strMark,aMethods[i],BEGIN_OF_METHOD,END_OF_METHOD,METHOD_NEIGHBOURHOOD);
     */
    //znacky
    MarkSigns(strAnalyzeText,strMark,E_EQUAL,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_GREATER,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_LESS,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_NOEQUALS,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_AND,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_OR,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_PLUS,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_MINUS,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_MULTIPLICATION,EXPRESSION);
    MarkSigns(strAnalyzeText,strMark,E_DIVISION,EXPRESSION);

    //vyplnení 
    FillMarkSection(strMark,BEGIN_OF_COMMENT,END_OF_COMMENT);
    FillMarkSection(strMark,BEGIN_OF_STRING,END_OF_STRING);
    FillMarkSection(strMark,BEGIN_OF_DIRECT,END_OF_DIRECT);
    FillMarkSection(strMark,BEGIN_OF_METHOD,END_OF_METHOD);

    EscapeSequention(strAnalyzeText,strMark,BEGIN_OF_STRING,ESCAPE);
    return 0;
}

string CUseScript::GetStringError()
{
    string outStr;
    switch(intError)
    {
    case 0:  outStr="OK"; break;
    case 1:  outStr="ERR_UNCLOSED_SECTION"; break; 
    case 2:  outStr="ERR_IF_WITHOUT_THEN"; break;
    case 3:  outStr="ERR_INVALID_COMBINATION_KEYWORDS"; break;
    case 4:  outStr="ERR_EMPTY_QUESTION"; break;
    case 5:  outStr="ERR_EMPTY_COMMAND"; break;
    case 6:  outStr="ERR_KEYWORD_NOT_ALONE"; break;
    case 7:  outStr="ERR_FOR_WITHOUT_TO";break;  
    case 8:  outStr="ERR_EMPTY_INICIALIZE_FOR" ; break; 
    case 9:  outStr="ERR_EMPTY_QUESTION_FOR" ; break;
    case 10: outStr="ERR_EMPTY_QUESTION_WHILE"; break;
    case 11: outStr="ERR_KEYWORDS_CROSS" ; break;
    case 12: outStr="ERR_MORE_CLOSE_PARENTHESIS"  ;break;
    case 13: outStr="ERR_VARIABLE_NOT_EXIST"  ; break;
    case 14: outStr="ERR_MATHEMATIC_OPERATE_STRINGS" ; break;
    case 15: outStr="ERR_CANNOT_EQUALS_DIFFERENT_TYPE" ; break;
    case 16: outStr="ERR_OPERATORS_NEAR" ; break;
    case 17: outStr="ERR_DIVISION_BY_ZERO" ; break;
    case 18: outStr="ERR_UNDEFINED" ;break;
    case 19: outStr="ERR_METHOD_RETURNED_ERROR";break;
    case 20: outStr="ERR_ASSIGNING_NOT_IN_ROOT" ;break;
    case 21: outStr="ERR_BEFORE_ASSIGNING_NOT_VARIABLE";break;
    case 22: outStr="ERR_STRING_IN_QUESTION"; break;
    case 23: outStr="ERR_EXTERNAL_ERROR"; break; 
    case 24: outStr="ERR_KEYWORDS_NOT_CLOSED"; break;
    }
    return outStr;
}

int CUseScript::SetCode(string* sCode, int NoPrepare)
{
    int i;
    vector<string *> aLines;
    vector<string *> aMarkLines;

    Code->sCode = new string(*sCode);
    Code->sMarkCode = new string("");

    //priprava kodu
    MarkCode(Code->sCode,Code->sMarkCode);
    DeleteSection(Code->sCode,Code->sMarkCode,BEGIN_OF_COMMENT);
    SeparateLines(&aLines, &aMarkLines, Code->sCode,Code->sMarkCode,LINE_SEPARATORS,UNRECOGNIZED);

    //(*log)<<  *Code->sCode + "\n";
    //(*log)<<  *Code->sMarkCode + "\n";

    //vytvareni uzlu
    if ((MakeNodes(&(Code->ProgNodeArray),&aLines,&aMarkLines))==1)
       return intError;

    //destrukce objektu
    for(i=0;i<aLines.size();i++)
    {
       delete aLines[i];
       delete aMarkLines[i];
    }

    if (LinkNodes(&(Code->ProgNodeArray)))
       return intError;
	if (!NoPrepare)
      if (PrepareNodes(&(Code->ProgNodeArray)))
         return intError;

  //  DebugProgNodes(&(Code->ProgNodeArray),"outt.txt");

    return 0;
}

int CUseScript::Execute()
{
    //DebugProgNodes(&(Code->ProgNodeArray),"D:\\tt.txt");
    if (Execute(&(Code->ProgNodeArray)))
      return intError;

    return 0;
}

