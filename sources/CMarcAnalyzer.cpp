#include <stdio.h>

#include "CMarcAnalyzer.h"

char ISO_SUBFIELD_SEPARATOR[]         = "\x1F";
char ISO_FIELD_SEPARATOR[]            = "\x1E";
char ISO_RECORD_SEPARATOR[]           = "\x1D";
char TEXT_RECORD_SEPARATOR[]          = "@\n"; 
char TEXT_SUBFIELD_SEPARATOR[]        = "$";
char TEXT_FIELD_SEPARATOR[]           = "\n"; 
char ALEPH_RECORD_SEPARATOR[]         = "\n";
char ALEPH_SUBFIELD_SEPARATOR[]       = "$$";
char ALEPH_FIELD_SEPARATOR[]          = "\n";
char ALEPH_LAB_IDENTIFIKATOR[]          = "LDR";

char LINKING_SUBFIELD_IDENTIFICATOR[] = "1";
char LINKING_SUBFIELD_SEP_ISO[]       = "\x1F\x31";
char LINKING_SUBFIELD_SEP_TXT[]       = "$1";
char LINKING_SUBFIELD_SEP_ALEPH[]     = "$$1";


int CMarcAnalyzer::SetLog(ofstream* lg, int LogAllMes)
{
	log=lg;
	LogAll=LogAllMes;
	return 0;
}

CSubfield::CSubfield()
{
    ID=new string("");
    Value=new string("");
    LinkingField=new CField();
}

CSubfield::~CSubfield()
	{
       delete Value;
       delete ID;   
       delete LinkingField;
    }


void CMarcAnalyzer::CloneField(CField* Dest, CField* Sour)
{
	*(Dest->ID) = *(Sour->ID);
	*(Dest->ID1) = *(Sour->ID1);
	*(Dest->ID2) = *(Sour->ID2);
	for(int i=0;i<Sour->ptrCollSubf.size();i++)
	{
		CSubfield* NewSub=new CSubfield();
		*(NewSub->ID)=*(Sour->ptrCollSubf[i]->ID);
        *(NewSub->Value)=*(Sour->ptrCollSubf[i]->Value);   
		if(*(Sour->ptrCollSubf[i]->LinkingField->ID)!="")
		{
		    *(NewSub->LinkingField->ID)=*(Sour->ptrCollSubf[i]->LinkingField->ID);
			*(NewSub->LinkingField->ID1)=*(Sour->ptrCollSubf[i]->LinkingField->ID1);
			*(NewSub->LinkingField->ID2)=*(Sour->ptrCollSubf[i]->LinkingField->ID2);
			for(int j=0;j<Sour->ptrCollSubf[i]->LinkingField->ptrCollSubf.size();j++)
			{
				CSubfield* NewLinkSub=new CSubfield(); 
				*(NewLinkSub->ID)=*(Sour->ptrCollSubf[i]->LinkingField->ptrCollSubf[j]->ID);
			    *(NewLinkSub->Value)=*(Sour->ptrCollSubf[i]->LinkingField->ptrCollSubf[j]->Value);
				NewSub->LinkingField->ptrCollSubf.push_back(NewLinkSub);  
			}
		}
		Dest->ptrCollSubf.push_back(NewSub);  
	}
}

string CMarcAnalyzer::OrdinalForm(int number, int size)
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

void CMarcAnalyzer::TrimLeft(string* sStr, char const * ArraySep)
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

void CMarcAnalyzer::TrimRight(string* sStr, char const * ArraySep)
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


int CMarcAnalyzer::SplitString(string* strSplit, vector<string*>* outArray, char const * Separator)
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

void CMarcAnalyzer::Replace(string * strSplit, char * findStr, char const * repStr)
{
     int Poss;
     while((Poss=strSplit->find(findStr))!=-1)
     {
        strSplit->replace(Poss,strlen(findStr),repStr);
     }
}


int CMarcAnalyzer::OpenRecordFromString(string* strRecord, int OpenMode)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CMarcAnalyzer::OpenRecordFromString\n";
(*log)<< "strRecord: " << *strRecord << "\n";
(*log)<< "OpenMode: " << OpenMode << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug
    string outString;
    string empty="";

    for(int g=0;g<Fields.size();g++)
        delete Fields[g];
    Fields.clear();

    switch(OpenMode)
    {
    case LOAD_ISO_FORMAT:
        {
        if (strRecord->size()<26)
        {
            g_Error="ISO Format involved 1";
            return 1;
        }
		*g_Label=strRecord->substr(0,24);
		int FirstSep=strRecord->find("\x1E");
		if (FirstSep<25) 
		{
            g_Error="ISO Format involved 2";
            return 1;
        }
        string SizesString=strRecord->substr(24,FirstSep-24); //delky fieldu na zacatku ISO zaznamu
        string BodyString=strRecord->substr(FirstSep+1); //samotna data bez identifikatoru poli
        if (SizesString.size()%12!=0)
		{
            g_Error="ISO Format involved 3";
            return 1;
        }
		for(int i=0;i<(SizesString.size()/12);i++) //projdu vsechny delky
           {
            int Begin;
            int Length;

            string sTag=SizesString.substr(i*12,3);
            string sLength=SizesString.substr(i*12+3,4);
            string sStart=SizesString.substr(i*12+7,5);
            Begin=atoi(sStart.c_str()); //velikosti prevedu na string
            Length=atoi(sLength.c_str());
            if (Length==0)  
            {
                g_Error="ISO Format involved - Invalid Length " + OrdinalForm(i,0); 
                return 1;
            }
			if (Begin+Length>BodyString.size())
			{
				g_Error="ISO Format involved 4";
                return 1;
			}
            outString=BodyString.substr(Begin,Length);
            string ID1="";
            string ID2="";

            //zjistim zda ma pole indikatory
            int NoIndicator=0;
            for(int i=0;i<aNoIdenField.size();i++)
            {
                if ((*aNoIdenField[i])==sTag)
                    NoIndicator=1;
            }
            if(!NoIndicator)
            {
                ID1=outString.substr(0,1);
                ID2=outString.substr(1,1);
                outString=outString.substr(2);
            }
            
            //Pridam pole
            CField* NewField=new CField();
            *(NewField->ID)=sTag;
            *(NewField->ID1)=ID1;
            *(NewField->ID2)=ID2;
            if (LogAll) { //Debug
               (*log)<< "NEW FIELD: " << sTag << " ID1:" << ID1 << " ID2:" << ID2 << "\n";
            } //End Debug
            Fields.push_back(NewField);

            //Ziskam ukazatel na prave pridany Field
            Replace(&outString, ISO_FIELD_SEPARATOR,"");
            
            //Tato funkce rozemere string bez Indikatoru od kazdeho pole
            if ( AddOneField(&outString,NewField,ISO_SUBFIELD_SEPARATOR,LINKING_SUBFIELD_SEP_ISO) ) {
                return 1;
            };
                //error

            }
        }
        break;
    case LOAD_ALEPH_FORMAT:
        {
        int i;
        vector<string *> aLines;

        TrimLeft(strRecord,"\n\r\t");
        TrimRight(strRecord,"\n\r\t");        
        
        //Splitovani stringu
        SplitString(strRecord, &aLines, ALEPH_FIELD_SEPARATOR);
        
        //IDCKO do FieldPrefix
        if (aLines[0]->length()<LENGTH_ID_ALEPH) {
            g_Error="ALEPH Format involved - Invalid size line";
            return 1;    
        }
        *g_FieldPrefix=aLines[0]->substr(0,LENGTH_ID_ALEPH);

        //Pole
        for(i=0;i<aLines.size();i++)
        {
            if (aLines[i]->length()<LENGTH_ID_ALEPH+9) {
                  g_Error="ALEPH Format involved - Invalid size line";
                  return 1;    
            }   
            string sTag=aLines[i]->substr(LENGTH_ID_ALEPH+1,3);
            string ID1=aLines[i]->substr(LENGTH_ID_ALEPH+4,1);
            string ID2=aLines[i]->substr(LENGTH_ID_ALEPH+5,1);
            string Line=aLines[i]->substr(LENGTH_ID_ALEPH+9);
            //Pridam pole

            if(sTag==ALEPH_LAB_IDENTIFIKATOR)
            {
                *g_Label=Line;

            }
            else
                {
                CField* NewField=new CField();
                *(NewField->ID)=sTag;
                *(NewField->ID1)=ID1;
                *(NewField->ID2)=ID2;
                Fields.push_back(NewField);
             

                //Ziskam ukazatel na prave pridany Field
  
                //outString.Replace(TEXT_FIELD_SEPARATOR,"");
               TrimLeft(&Line,"\n\r\t");
               TrimRight(&Line,"\n\r\t");
                //Tato funkce rozebere string bez Indikatoru od kazdeho pole
                if ( AddOneField(&Line,NewField,ALEPH_SUBFIELD_SEPARATOR,LINKING_SUBFIELD_SEP_ALEPH) ) {
                    return 1;
                };
            }
        }
        //odstranit aLines
        for(i=0;i<aLines.size();i++)
            delete aLines[i];    
        
        }
        break;
    case LOAD_TEXT_FORMAT:
        {
        int i;
        vector<string *> aLines;
        
        TrimLeft(strRecord,"\n\r\t");
        TrimRight(strRecord,"\n\r\t");

        //Splitovani stringu
        SplitString(strRecord, &aLines, TEXT_FIELD_SEPARATOR);
        //odstran�n� praznych radek
        for (i=aLines.size()-1;i>(-1);i--)
        {
            TrimLeft(aLines[i]," ");
            TrimRight(aLines[i]," ");
            if (*(aLines[i])=="")
            {
                delete aLines[i];
                aLines.erase(aLines.begin()+i);
            }
        }

        if (aLines[0]->size()<24)
        {
            g_Error="Text Format involved - Invalid Size Label"; 
            //(*log)<< "SIZE:" << *(aLines[0]) << "\n";
            return 0;
        }
        *g_Label=aLines[0]->substr(0,24);
        //Pole
        for(i=1;i<aLines.size();i++)
            {
			if (aLines[i]->size()<4)
			{
				g_Error="Text Format involved - Invalid Size Field"; 
				return 0;
			}
            string sTag=aLines[i]->substr(0,3);
            string Line=aLines[i]->substr(3);
            string ID1="";
            string ID2="";
            int NoIndicator=0;
            for(int j=0;j<aNoIdenField.size();j++)
            {
                if (*(aNoIdenField[j])==sTag)
                    NoIndicator=1;
            }
            if(!NoIndicator)
            {
                ID1=Line.substr(0,1);
                ID2=Line.substr(1,1);
                Line=Line.substr(2);
            }

            CField* NewField=new CField();
            *(NewField->ID)=sTag;
            *(NewField->ID1)=ID1;
            *(NewField->ID2)=ID2;
            Fields.push_back(NewField);

            TrimLeft(&Line,"\n\r\t");
            TrimRight(&Line,"\n\r\t");

            //Tato funkce rozebere string bez Indikatoru od kazdeho pole
            
            if ( AddOneField(&Line,NewField,TEXT_SUBFIELD_SEPARATOR,LINKING_SUBFIELD_SEP_TXT) ) {
                return 1;
            };
            }    
           //odstranit aLines
           for(i=0;i<aLines.size();i++)
              delete aLines[i];    
           break;
        }
    }
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::OpenRecordFromString\n";
(*log)<< "---------------------------------------------\n";
} //End Debug        
    return 0;
}

bool CMarcAnalyzer::MayBeLinking(string Field) 
{
    for(int b=0;b<aLinkingField.size();b++) {
	   if (*(aLinkingField[b])==Field)
          return true;
    }
    return false;
}


int CMarcAnalyzer::AddOneField(string* sField, CField* pField, char* SubSeparator, char* LinkingSeparator)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CMarcAnalyzer::AddOneField\n";
(*log)<< "sField: \"" << *sField << "\"\n";
(*log)<< "SubSeparator: " << SubSeparator << "\n";
(*log)<< "LinkingSeparator: " << LinkingSeparator << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug
    int i;
    string Subfield_Ident=LINKING_SUBFIELD_IDENTIFICATOR;
    CSubfield* NewSubfield;
    vector<string *> SplitArray;
    
    int LengthSep=strlen(SubSeparator);

    //Prazdny string
    if (sField->size() ==0)
        return 0;

    //Je prvn�m znakem $? pak by se jednalo o ruzna podpole
    if (sField->substr(0,LengthSep)==SubSeparator)
    {
        if (sField->substr(LengthSep,1)==LINKING_SUBFIELD_IDENTIFICATOR && MayBeLinking(*(pField->ID))) //jde o vlozena pole? za $1
        {
            SplitString(sField,&SplitArray,LinkingSeparator); //rozdelim string oddelovacem $1
            for (i=1;i<SplitArray.size();i++) //prvn� je praznej
            {
                if (SplitArray[i]->size()<4) //chybi identifikator vlozeneho pole a alespon neco za tim
                {
                    g_Error="File involved - Invalid Command " + (*sField);
                    return 1;
                }

                
                //P�id�me podpole a z�sk�me ukazatel na linking pole
                NewSubfield=new CSubfield();
                *(NewSubfield->ID)=LINKING_SUBFIELD_IDENTIFICATOR;
                *(NewSubfield->Value)="";
                if (LogAll) { //Debug
                     (*log)<< "NewSubfield: Link$" << *(NewSubfield->ID) << "\"" << *(NewSubfield->Value) << "\"" << "\n";
                } //End Debug                 
                pField->ptrCollSubf.push_back(NewSubfield);

                //Prvn� tri je ID pole a zbytek je samotne pole
                string sTag=SplitArray[i]->substr(0,3);
                string outString = SplitArray[i]->substr(3);

                //je vlozen� pole NoIdenField?
                string ID1="";
                string ID2="";

                int NoIndicator=0;
                for(int i=0;i<aNoIdenField.size();i++)
                {
                    if (*(aNoIdenField[i])==sTag)
                        NoIndicator=1;
                }
                
                if(!NoIndicator && (outString.size() >= 2) )
                {
                    ID1=outString.substr(0,1);
                    ID2=outString.substr(1,1);
                    outString=outString.substr(2);
                }

                //Vyplnime informace o vlozenem poli
                *(NewSubfield->LinkingField->ID)=sTag;
                *(NewSubfield->LinkingField->ID1)=ID1;
                *(NewSubfield->LinkingField->ID2)=ID2;

                //ted teprve rekurzivne pridame vlozene pole LinkedField ukazuje na vlozene pole subfieldu
                AddOneField(&outString,NewSubfield->LinkingField,SubSeparator,LinkingSeparator);
            }
            for(i=0;i<SplitArray.size();i++)
              delete SplitArray[i];             
                
        }
        else //nejedna se o vlozene podpole nezacina $1
        {
            //Rozsekam na podpole a zapisu
            SplitString(sField,&SplitArray,SubSeparator);
            if (*(SplitArray[0])!="")  //To by bylo fakt divny
            {
                g_Error="File involved - Invalid Command " + *sField;
                return 1;
            }
            for (i=1;i<SplitArray.size();i++)
            {
                Replace(SplitArray[i],SubSeparator,"");
                if (SplitArray[i]->size()==0) //p��pad $$
                {
                    g_Error="File involved - Invalid Command " + *sField;
                    return 1;
                }
                if (SplitArray[i]->size()>1) //p��pad $$
                {
                    NewSubfield=new CSubfield();
                    *(NewSubfield->ID)=SplitArray[i]->substr(0,1);
                    *(NewSubfield->Value)=SplitArray[i]->substr(1);
                    if (LogAll) { //Debug
                     (*log)<< "NewSubfield: $" << *(NewSubfield->ID) << "\"" << *(NewSubfield->Value) << "\"" << "\n";
                    } //End Debug 
                    pField->ptrCollSubf.push_back(NewSubfield);
                }
                else
                {
                    NewSubfield=new CSubfield();
                    *(NewSubfield->ID)=SplitArray[i]->substr(0,1);
                    *(NewSubfield->Value)="";
                    if (LogAll) { //Debug
                     (*log)<< "NewSubfield: $" << *(NewSubfield->ID) << "\"" << *(NewSubfield->Value) << "\"" << "\n";
                    } //End Debug                     
                    pField->ptrCollSubf.push_back(NewSubfield);
                }
            }
            for(i=0;i<SplitArray.size();i++)
              delete SplitArray[i];                         
        }
    }
    else
    {
        //prvn�m znakem nen� dolar. Jde pouze o text
        NewSubfield=new CSubfield();
        *(NewSubfield->ID)="E";
        *(NewSubfield->Value)=*sField;
        if (LogAll) { //Debug
           (*log)<< "NewSubfield: $" << *(NewSubfield->ID) << "\"" << *(NewSubfield->Value) << "\"" << "\n";
        } //End Debug                 

        pField->ptrCollSubf.push_back(NewSubfield);
    }
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::AddOneField\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
    return 0;
} 

int CMarcAnalyzer:: random_range(int lo, int hi) { 
    int range = (int) ((((float)rand()) / RAND_MAX) * (hi - lo + 1) + lo); 
    //cout  << "random_range(" << lo << "," << hi << ")="<< range << "\n";
    return range;
}

int CMarcAnalyzer::SerializeRecord(string *outString, string *Sizes, char* SubfieldSeparator, char* FieldSeparator, int Mode)
{
    long Size;
    long SubSize;
    long SubSubSize;
    string outOne;

    int PreviousPoss=0;
    Size=Fields.size();
       
    
    int randField = 0; 
    int randSubField = 0;
    
    if ( codeVersion == VERSION_TESTING_E ) {
       randField = random_range(0, Size - 1); 
      // cout  << "RR" << randField << "\n";
    }   
       
    for(int i=0;i<Size;i++)
    {
        outOne="";
        if (Mode==LOAD_TEXT_FORMAT)
            outOne+=*(Fields[i]->ID);
        if (Mode==LOAD_ALEPH_FORMAT)
        {
            outOne+=*g_FieldPrefix+" "+*(Fields[i]->ID);
        }
        int NoIndicator=0;
        for(int b=0;b<aNoIdenField.size();b++)
        {
            //cout   <<  *(aNoIdenField[b]);  
			if (*(aNoIdenField[b])==*(Fields[i]->ID))
                NoIndicator=1;
        }
        if (!NoIndicator || Mode==LOAD_ALEPH_FORMAT)
        {
            outOne+=*(Fields[i]->ID1);
            outOne+=*(Fields[i]->ID2);
        }
        if (Mode==LOAD_ALEPH_FORMAT)
        {
            outOne+=" "+ *(g_Format) + " ";
        }

        SubSize=Fields[i]->ptrCollSubf.size();
        if ( codeVersion == VERSION_TESTING_E && i == randField ) {
            randSubField = random_range(0, SubSize - 1); 
        }
        
        for(int j=0;j<SubSize;j++)
        {
            if ( codeVersion == VERSION_TESTING_E ) {
                if ( i == randField  && j == randSubField ) {
                    outOne+=SubfieldSeparator;
                    outOne += (char) random_range(97, 122);  //character a-z
                    int countChars = random_range(1, 12);
                    for ( int g = 0 ; g <  countChars ; g++ ) {
                        outOne += (char) random_range(97, 122);  //character a-z
                    }               
                }
            }
                
            if(*(Fields[i]->ptrCollSubf[j]->ID)=="E")
            {
                outOne+=*(Fields[i]->ptrCollSubf[j]->Value);
            }
            else
            {
               if (*(Fields[i]->ptrCollSubf[j]->ID)==LINKING_SUBFIELD_IDENTIFICATOR && MayBeLinking(*(Fields[i]->ID)))
                {
                    outOne+=SubfieldSeparator;
                    outOne+=*(Fields[i]->ptrCollSubf[j]->ID);
                    outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ID);

                    NoIndicator=0;
                    for(int b=0;b<aNoIdenField.size();b++)
                    {						                    
                        if (*(aNoIdenField[b])==*(Fields[i]->ptrCollSubf[j]->LinkingField->ID))
                            NoIndicator=1;
                    }
                    if (!NoIndicator)
                    {
						outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ID1);
                        outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ID2);
                    }

                    SubSubSize=Fields[i]->ptrCollSubf[j]->LinkingField->ptrCollSubf.size();
        
                    for(int k=0;k<SubSubSize;k++)
                    {
                        if(*(Fields[i]->ptrCollSubf[j]->LinkingField->ptrCollSubf[k]->ID)=="")
                        {
                            outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ptrCollSubf[k]->Value);
                        }
                        else
                        {
                            outOne+=SubfieldSeparator;
                            outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ptrCollSubf[k]->ID);
                            outOne+=*(Fields[i]->ptrCollSubf[j]->LinkingField->ptrCollSubf[k]->Value);
                        }
                    }
                }
                else
                {
                    outOne+=SubfieldSeparator;
                    outOne+=*(Fields[i]->ptrCollSubf[j]->ID);
                    outOne+=*(Fields[i]->ptrCollSubf[j]->Value);
                }
            }
        }         
	   if (i<(Size-1) || Mode==LOAD_ISO_FORMAT) {
           outOne+=FieldSeparator;
       }
       *outString+=outOne;
       if (Mode==LOAD_ISO_FORMAT)
       {
            *Sizes+=*(Fields[i]->ID);
            *Sizes+=OrdinalForm(outOne.size(),4);
            *Sizes+=OrdinalForm(PreviousPoss,5);
            PreviousPoss+=outOne.size();
       }

    }
	return 0;
}



int CMarcAnalyzer::SortFields()
{
    long Size;
    Size=Fields.size();
    int iID;
    
    for(int i=0;i<Size-1;i++)
    {
        int minimum = 9999;
        int possitionOfMin = 0;
        for(int j=i;j<Size;j++)
        {
            iID=atoi(Fields[j]->ID->c_str());
            if (iID < minimum) 
            {
                possitionOfMin = j;
                minimum = iID;
            }
        }
        CField* MinimumItem = Fields[possitionOfMin];
        for(int k = possitionOfMin ; k > i ; k--) {
            Fields[k]=Fields[k-1];
        }      
        Fields[i] = MinimumItem;
                 
    }
    return 0;    
}



int CMarcAnalyzer::FillFieldsDepth()
{
    long Counter;

    for(int i=0;i<Fields.size();i++)
    {
        Counter=1;
        for(int j=0;j<i;j++)
        {
            if (*(Fields[i]->ID)==*(Fields[j]->ID))
                Counter++;
        }
        Fields[i]->Index=Counter;
        FillSubfieldsDepth(&(Fields[i]->ptrCollSubf),1);
    }
    return 0;
}

int CMarcAnalyzer::FillSubfieldsDepth(vector<CSubfield*>* pcollS, int Link)
{
    long Size=pcollS->size();
    long Counter;

    for(int i=0;i<Size;i++)
    {
        Counter=1;
        for(int j=0;j<i;j++)
        {
          if (*((*pcollS)[i]->ID)==*((*pcollS)[j]->ID))
                Counter++;
        }
        (*pcollS)[i]->Index=Counter;
        if(Link)
            FillSubfieldsDepth(&((*pcollS)[i]->LinkingField->ptrCollSubf),0);
    }
    return 0;
}


string* CMarcAnalyzer::ValueByCommand(string* Command)
{
    string IDF;      //Field
    string IDS;          //Subfile
    string LAB;
    long IF;      //pozice fieldu
    long IS;   //pozicesubfieldu
    string ILF;      //Cislo linkovaciho fieldu
    int Ind;
    int Start;
    int Stop;

    long PointerField=0;
    long PointerSubField=0;
    long PointerLinkingField=0;

    if (RecognizeCommand(Command, &LAB, &IDF, &ILF, &IDS, &IF, &IS, &Ind, &Start, &Stop))
    {
        g_Error="RecognizeCommand failed";
        return new string("");
    }
    if (LAB!="")
    {
        //tady musim pripsat od Start do Stop
        if (Start>0)
        {
            if (Start>Stop)
            {
                return new string("");
            }
            if( Start>24) 
            {
                return new string("");
            }
            return new string(g_Label->substr(Start-1,Stop-Start+1)); 
        }
        else
        {
            return new string(*g_Label);
        }
    }
    return GetValueFromC(IDF,ILF,IDS,IF,IS,Ind,Start,Stop,&PointerField,&PointerSubField,&PointerLinkingField);
}                      


int CMarcAnalyzer::RecognizeCommand(string* Comm, string *LAB, string *IDF, string *ILF, string *IDS, long* IF, long* IS, int* Indic, int* Sta, int* Sto)
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
            if (Start-1<Stop && Stop<g_Label->length()+1) 
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
   

string* CMarcAnalyzer::GetValueFromC(string IDField,string LinkField, string IDSub,long IndexField,long IndexSubField, int Indikator, int Start, int Stop, long* PointerField, long* PointerSubField, long* PointerLinkingField)
{
    string sID;
    string outString="";
    
    CField *NowF = 0;
    CSubfield *NowS = 0;
    long Size;
    //long Index;

    int i;
    Size=Fields.size();
    int Serched=0;

    for(i=0;i<Size;i++)
    {
       // (*log)<< IDField + " x " + *(Fields[i]->ID) + "\n";
       // (*log)<< IndexField << " x " << Fields[i]->Index << "\n";
        if (IDField==*(Fields[i]->ID) && IndexField==Fields[i]->Index)
        {
            *PointerField=i;
            NowF=Fields[i];
            Serched=1;
            break;
        }
    }
    if (Serched==0) return new string(outString);

    
    if (LinkField!="")
    {
        Serched=0;
        Size=NowF->ptrCollSubf.size();
        for(i=0;i<Size;i++)
        {
            NowF=NowF->ptrCollSubf[i]->LinkingField;
            if (LinkField==*(NowF->ID))
            {
                *PointerLinkingField=i;
                Serched=1;
                break;
            }
        }
        if (Serched==0) return new string(outString);
    }
    
    
    //V NowF je ted ukazatel na pole bud norm nebo linkovane - k oboum pristupuju stejne
    if (Indikator!=0)
    {
        if (Indikator==1)
        {
          return new string(*(NowF->ID1));
        }
        if (Indikator==2)
        {
          return new string(*(NowF->ID2));
        }
        return new string("");
    }

    if (IDSub=="") return new string("");

    if (IDSub==" ") IDSub="";
    Size=NowF->ptrCollSubf.size();
    Serched=0;
    for(i=0;i<Size;i++)
    {
        NowS = NowF->ptrCollSubf[i]; 
        if (IDSub==*(NowS->ID) && NowS->Index==IndexSubField)
        {
            *PointerSubField=i;
            Serched=1;
            break;
        }
    }

    if (Serched==0) return new string("");

    if (Start==(-1)) return new string(*(NowS->Value));

    if (Start-1<Stop && Stop<outString.length()+1) 
    {
        return new string(NowS->Value->substr(Start-1,Stop-Start+1));
    }
    
    return new string("");
}
     

string* CMarcAnalyzer::GetSerializeRecord(long Mode)
{
	string OutString;
    string SizesString;

    SortFields();
        
    if (Mode==LOAD_ISO_FORMAT)
    {
        SerializeRecord(&OutString,&SizesString, ISO_SUBFIELD_SEPARATOR, ISO_FIELD_SEPARATOR, LOAD_ISO_FORMAT);     
        //zapsani delky
        string sL=OrdinalForm(SizesString.length()+OutString.length()+24+2,5);
        sL+=g_Label->substr(5,7);
        
        sL+=OrdinalForm(SizesString.length()+24+1,5);
        
        sL+=g_Label->substr(17);
        sL+=SizesString;     
        sL+=ISO_FIELD_SEPARATOR; 
        OutString=sL+OutString + ISO_RECORD_SEPARATOR;
    }
    
    if (Mode==LOAD_TEXT_FORMAT)
    {
        SerializeRecord(&OutString,&SizesString, TEXT_SUBFIELD_SEPARATOR, TEXT_FIELD_SEPARATOR, LOAD_TEXT_FORMAT);     
        OutString=TEXT_RECORD_SEPARATOR+*(g_Label)+TEXT_FIELD_SEPARATOR+OutString;  
    }    
	if (Mode==LOAD_ALEPH_FORMAT)
    {
        SerializeRecord(&OutString,&SizesString, ALEPH_SUBFIELD_SEPARATOR, ALEPH_FIELD_SEPARATOR, LOAD_ALEPH_FORMAT);     
        OutString=*(g_FieldPrefix) + " LDR   "+ *(g_Format) + " "+*(g_Label)+ ALEPH_FIELD_SEPARATOR + OutString + ALEPH_RECORD_SEPARATOR;  
    }
	return new string(OutString);
}



void CMarcAnalyzer::SetValueByCommand(string* Command, string* Value, long IField, long ISubfield, long Add)
{
    string IDF;      //Field
    string IDS;          //Subfile
    string LAB;
    long IF;      //pozice fieldu
    long IS;   //pozicesubfieldu
    string ILF;      //Cislo linkovaciho fieldu
    int Ind;
    int Start;
    int Stop;
  //  (*log)<< *Command <<"\n";
    if (RecognizeCommand(Command, &LAB, &IDF, &ILF, &IDS, &IF, &IS, &Ind, &Start, &Stop))
    {
        g_Error="RecognizeCommand failed";
        return;
    }
    if (LAB!="")
    {
      //  (*log)<< "LABB\n";
        if (Start==(-1))
        {
            if (Value->length()!=24)
            {
                g_Error="Invalid Label Value Length";
                return;   
            }
            *g_Label=*Value;
        }
        else
        {
            if (Start-1<Stop && Stop<25) 
                {
                    string Before=g_Label->substr(0,Start-1);
                    string After=g_Label->substr(Stop);
                    if ((Stop-Start+1)>Value->length())
                    {
                        *g_Label=Before;
                        *g_Label+=*Value;
                        for (int i=0;i<(Stop-Start+1)-Value->length();i++)
                            *g_Label+=" ";
                        *g_Label+=After;
                    }
                    else
                    {
                        *g_Label=Before;
                        *g_Label+=Value->substr(0,Stop-Start+1);
                        *g_Label+=After;
                    }

                }
        }
    } 
    else
    {
      if (SetValueFromC(Value,&IDF,&ILF,&IDS,IField,ISubfield,Ind,Start,Stop,Add))
      {
        g_Error="Setting failed";
        return;
      }
    }  
    FillFieldsDepth();
}



int CMarcAnalyzer::SetValueFromC(string* Value, string *IDField, string* LinkField, string* IDSub,long IndexField,long IndexSubField, int Indikator, int Start, int Stop, int Add)
{
    long Size;
    CField* NewField;
    CSubfield* NewSubfield;
    int i;
    Size=Fields.size();
    int Serched=0;

    for(i=0;i<Size;i++)
    {
        
        if (*(IDField)==*(Fields[i]->ID) && IndexField==Fields[i]->Index)
        {
            Serched=1;
            NewField=Fields[i];
            break;
        }
    }
    if (Serched==0)
    {
        //Tak ho musim zalo�it
        NewField=new CField(); 
        *(NewField->ID)=*IDField;
        *(NewField->ID1)=" ";
        *(NewField->ID2)=" ";
        Fields.push_back(NewField);
    }

    if (*LinkField!="")
    {
        Serched=0;
        Size=NewField->ptrCollSubf.size();
        for(i=0;i<Size;i++)
        {
            if (*LinkField==*(NewField->ptrCollSubf[i]->LinkingField->ID))
            {
                Serched=1;
                NewField=NewField->ptrCollSubf[i]->LinkingField;
                break;
            }
        }
        if (Serched==0)
        {
            //Neni tam a ja ho musim pridat a priradit mu ID
            NewSubfield=new CSubfield();
            *(NewSubfield->ID)=LINKING_SUBFIELD_IDENTIFICATOR;
            *(NewSubfield->Value)="";
            NewField->ptrCollSubf.push_back(NewSubfield);
            *(NewSubfield->LinkingField->ID)=*LinkField;
            NewField=NewSubfield->LinkingField;
        }
    }
    
    
    //V NewField je ted ukazatel na pole bud norm nebo linkovane - k oboum pristupuju stejne
    if (Indikator!=0)
    {
        if (Indikator==1)
        {
          *(NewField->ID1)=Value->substr(0,1);
          return 0;
        }
        if (Indikator==2)
        {
          *(NewField->ID2)=Value->substr(0,1);
          return 0;
          return 0;
        }
        return 1;
    }
    if (*IDSub=="") 
    {
        return 1;
    }

    if (*IDSub==" ") *IDSub="";

    Size = NewField->ptrCollSubf.size();
    Serched=0;
    for(i=0;i<Size;i++)
    {
        if (*IDSub==*(NewField->ptrCollSubf[i]->ID) && NewField->ptrCollSubf[i]->Index==IndexSubField)
        {
            Serched=1;
            NewSubfield=NewField->ptrCollSubf[i];
            break;
        }
    }

    if (Serched==0) 
    {
        NewSubfield=new CSubfield();
        *(NewSubfield->ID)=*IDSub;
        *(NewSubfield->Value)="";
        NewField->ptrCollSubf.push_back(NewSubfield);
    }
    
    if (Start==(-1)) 
    {    
        if (Add)
            *(NewSubfield->Value)=*(NewSubfield->Value) + *Value;
        else
            *(NewSubfield->Value)= *Value;
        return 0;
    }
    
    if (Start-1<Stop) 
    {
        //Zjist�me st�vaj�c� hodnotu
        string mValueStr=*(NewSubfield->Value);
        string outStr="";
        //Dopln�me tuto hodnotu na d�lku do stop
        for(i=mValueStr.length();i<Stop;i++)
        {
            mValueStr=mValueStr+" ";
        }
        string Before=mValueStr.substr(0,Start-1);
        string After=mValueStr.substr(Stop);
        //S m��e b�t men�� ne� je po�adovan� d�lka
        if ((Stop-Start+1)>Value->length())
        {
            outStr=Before;
            outStr+=*Value;
            for (int i=0;i<(Stop-Start+1)-Value->length();i++)
                outStr+=" ";
            outStr+=After;
        }
        else
        {
            outStr=Before;
            outStr+=Value->substr(0,Stop-Start+1);
            outStr+=After;
        }
        *(NewSubfield->Value)=outStr;
        return 0;
    }

    return 1;

} 



long CMarcAnalyzer::MaxFieldIndex(string* sIDF)
{
	long Size;
    Size=Fields.size();

    long MaxIndex=0;

    for(int i=0;i<Size;i++)
    {
        if (*(Fields[i]->ID)==*(sIDF))
        {
            if (Fields[i]->Index>MaxIndex) MaxIndex=Fields[i]->Index;
        }
    }
	return MaxIndex;
} 

long CMarcAnalyzer::MaxLinkingFieldIndex(string* sIDF, long IndexField, string* sIDL)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CMarcAnalyzer::MaxLinkingFieldIndex\n";
(*log)<< "sIDF: " << *sIDF << "\n";
(*log)<< "IndexField: " << IndexField << "\n";
(*log)<< "sIDL: " << *sIDL << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug
	long Size;
    long Size2;
    Size=Fields.size();

    long MaxIndex=0;

    for(int i=0;i<Size;i++)
    {
        if (*(Fields[i]->ID)==*sIDF && Fields[i]->Index==IndexField)
        {
            Size2=Fields[i]->ptrCollSubf.size();
            for(int j=0;j<Size2;j++)
            {
                if (*(Fields[i]->ptrCollSubf[j]->LinkingField->ID)==*sIDL)
                {
                    MaxIndex=1;
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxLinkingFieldIndex\n";
(*log)<< "Return: " << MaxIndex << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug                     
                    return MaxIndex;
                }
            }
        }
    }
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxLinkingFieldIndex\n";
(*log)<< "Return: " << MaxIndex << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
	return MaxIndex;
}




long CMarcAnalyzer::MaxSubfieldIndex(string* sIDF, string* sSubID, long FIndex)
{
    long Size;	
	Size=Fields.size();
    long MaxIndex=0;
    CField *MF = 0;
    int Searched=0;
    int i;
    for(i=0;i<Size;i++)
    {
        if (*(Fields[i]->ID)==*sIDF && Fields[i]->Index==FIndex)
        {
            Searched=1;
            MF=Fields[i];
            break;
        }
    }
    if (Searched==0) return 0;

    Size=MF->ptrCollSubf.size();

    for(i=0;i<Size;i++)
    {
    if (*(MF->ptrCollSubf[i]->ID)==*sSubID)
        {
            if (MF->ptrCollSubf[i]->Index>MaxIndex) MaxIndex=MF->ptrCollSubf[i]->Index;
        }
    }   
    return MaxIndex;
}



long CMarcAnalyzer::MaxLinkingSubfieldIndex(string* sIDF, string* sIDL, string* sSubID, long FIndex)
{
    int i;
	long Size;	
	Size=Fields.size();

    long MaxIndex=0;
    int Searched=0;
    CField *MF = 0;
    
    for(i=0;i<Size;i++)
    {
        if (*(Fields[i]->ID)==*sIDF && Fields[i]->Index==FIndex)
        {
            Searched=1;
            MF=Fields[i];            
            break;
        }
    }
    if (Searched==0) return 0;
    
    Size=MF->ptrCollSubf.size();
    
    Searched=0;
    for(i=0;i<Size;i++)
    {
        if (*(MF->ptrCollSubf[i]->LinkingField->ID)==*sIDL)
        {
            Searched=1;
            MF=MF->ptrCollSubf[i]->LinkingField;
            break;
        }
    }   
    if (Searched==0) return 0;

    Size=MF->ptrCollSubf.size();

    for(i=0;i<Size;i++)
    {
        if (*(MF->ptrCollSubf[i]->ID)==*sSubID)
        {
            if (MF->ptrCollSubf[i]->Index>MaxIndex) MaxIndex=MF->ptrCollSubf[i]->Index;
        }
    }   
	return MaxIndex;
} 



long CMarcAnalyzer::MaxIndexByCommand(string* Command, long FIndex)
{
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "BEGIN: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Command: " << *Command << "\n";
(*log)<< "FIndex: " << FIndex << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug
    string IDF;      //Field
    string IDS;          //Subfiles
    string LAB;
    long IF;      //pozice fieldu
    long IS;   //pozicesubfieldu
    string ILF;      //Cislo linkovaciho fieldu
    int Ind;
    int Start;
    int Stop;
    RecognizeCommand(Command, &LAB, &IDF, &ILF, &IDS, &IF, &IS, &Ind, &Start, &Stop);   
    if (LAB!="")
    {
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Return1: " << "0" << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
        return 0;
    } 
    if (IDS=="")
    {
        if (ILF!="") 
        {
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Return(MaxLinkingFieldIndex): " << MaxLinkingFieldIndex(&IDF,IF,&ILF) << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
            return MaxLinkingFieldIndex(&IDF,IF,&ILF);
        }
        else
        {
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Return(MaxFieldIndex): " << MaxFieldIndex(&IDF) << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug    
            return MaxFieldIndex(&IDF);
        }
    }
    if (ILF!="")
    {
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Return(MaxLinkingSubfieldIndex): " << MaxLinkingSubfieldIndex(&IDF,&ILF,&IDS,IF) << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug        
        return MaxLinkingSubfieldIndex(&IDF,&ILF,&IDS,IF);
    }
if (LogAll) { //Debug
(*log)<< "---------------------------------------------\n";
(*log)<< "END: CMarcAnalyzer::MaxIndexByCommand\n";
(*log)<< "Return(MaxSubfieldIndex): " << MaxSubfieldIndex(&IDF,&IDS,IF) << "\n";
(*log)<< "---------------------------------------------\n";
} //End Debug      
    return MaxSubfieldIndex(&IDF,&IDS,IF);
}



string* CMarcAnalyzer::ValueByCommandSetPoss(string* Command, long loIF, long loIS)
{
    string IDF;      //Field
    string IDS;          //Subfile
    string LAB;
    long IF;      //pozice fieldu
    long IS;   //pozicesubfieldu
    string ILF;      //Cislo linkovaciho fieldu
    int Ind;
    int Start;
    int Stop;

    long PointerField=0;
    long PointerSubField=0;
    long PointerLinkingField=0;

    if (RecognizeCommand(Command, &LAB, &IDF, &ILF, &IDS, &IF, &IS, &Ind, &Start, &Stop))
    {
        g_Error="RecognizeCommand failed";
        return new string("");
    }
    if (LAB!="")
    {
        //tady musim pripsat od Start do Stop
        if (Start>0)
        {
            if (Start>Stop)
            {
                return new string("");
            }
            if( Start>24) 
            {
                return new string("");
            }
            return new string(g_Label->substr(Start-1,Stop-Start+1));
        }
        else
        {
            return new string(*g_Label);
        }
    }
    return GetValueFromC(IDF,ILF,IDS,loIF,loIS,Ind,Start,Stop,&PointerField,&PointerSubField,&PointerLinkingField);
}





string* CMarcAnalyzer::ValueByCSetPossWithPointers(string* Command, long loIF, long loIS, long PointerField, long PointerLinkingField, long PointerSubField)
{
   string IDF;      //Field
    string IDS;          //Subfile
    string LAB;
    long IF;      //pozice fieldu
    long IS;   //pozicesubfieldu
    string ILF;      //Cislo linkovaciho fieldu
    int Ind;
    int Start;
    int Stop;

    if (RecognizeCommand(Command, &LAB, &IDF, &ILF, &IDS, &IF, &IS, &Ind, &Start, &Stop))
    {
        g_Error="RecognizeCommand failed";
        return new string("");
    }
    if (LAB!="")
    {
        //tady musim pripsat od Start do Stop
        if (Start>0)
        {
            if (Start>Stop)
            {
                return new string("");
            }
            if( Start>24) 
            {
                return new string("");
            }
            return new string(g_Label->substr(Start-1,Stop-Start+1));
        }
        else
        {
            return new string(*g_Label);
        }
    }
    return GetValueFromC(IDF,ILF,IDS,loIF,loIS,Ind,Start,Stop,&PointerField,&PointerSubField,&PointerLinkingField);
}


void CMarcAnalyzer::LoadCheck(const char* sPath)
{   
    vector<string *> Elements;
    vector<string *> MarkElements;
    string strAnalyzeText;    
    FILE *fr;
    
    if ((fr=fopen(sPath,"r"))== NULL)
    {
       g_Error="Not open ControlFile...";  
       return;
    }          
    int nBytesRead=0;
    char* buffer = new char[MAXBUF];

    while ((nBytesRead = fread(buffer, 1, MAXBUF - 1,fr))!=0) {
		buffer[nBytesRead] = '\0';
	    strAnalyzeText+=buffer;	
	};       
    string* strMark=new string(strAnalyzeText.length(), 'x');
 
	MarkSections2(&strAnalyzeText,strMark,"/*","*/",'C','R',0);
	FillMarkSection(strMark,'C','R');
	
	//Ostranime prazdny radky a komentare
    for(int i=strMark->length()-1;i>(-1);i--) {
        if ((*strMark)[i]=='C')  {
            strAnalyzeText.erase(i,1);
            strMark->erase(i,1);
        }
    }
    
	MarkSections2(&strAnalyzeText,strMark,"{","}",'O','P',0);   
    MarkSections2(&strAnalyzeText,strMark,"(",")",'B','E',0);
    MarkSections2(&strAnalyzeText,strMark,"[","]",'W','N',0);    
    MarkSections2(&strAnalyzeText,strMark,"<",">",'Q','F',0);    
    FillMarkSection(strMark,'B','E');
    FillMarkSection(strMark,'W','N');
    FillMarkSection(strMark,'Q','F');
    
    //cout << *strMark << "\n";

    CutCommandByType(&strAnalyzeText,strMark,&Elements,&MarkElements);   
    
    
    //Odstranime zavorky a tak
    for(int i=Elements.size()-1;i>(-1);i--)
    {
        if(*(MarkElements)[i]=="x") {
         delete Elements[i];
         delete MarkElements[i];
         Elements.erase(Elements.begin()+i);
         MarkElements.erase(MarkElements.begin()+i);
        } else {
         TrimLeft(Elements[i],"(");
         TrimRight(Elements[i],")");
         TrimLeft(Elements[i],"[");
         TrimRight(Elements[i],"]");
         TrimLeft(Elements[i],"<");
         TrimRight(Elements[i],">");
         TrimRight(Elements[i],"{");
         TrimLeft(Elements[i],"}");
         //cout << *(MarkElements)[i] << ":" << *(Elements)[i] << "\n";
        }
    }
    
    for(int i=0;i<Elements.size();i++)
    {
        if(*(MarkElements[i])=="Q") {
           if(Elements[i]->length()!=2 && Elements[i]->length()!=4) {
               g_Error="ControlFile Invalid ID Field: " + *(Elements[i]);  
               return;
           }
           if(Elements[i]->length()==4) {
              //pole
              *(MarkElements[i])="M";
           }
        }
    }
    /*
    for(int i=0;i<Elements.size();i++)
    {
        cout << *(MarkElements)[i] << ":" << *(Elements)[i] << "\n";
    }*/
        
    CField* ActualField = 0;
    CField* BeforeLinkingActualField = 0;
    CSubfield* ActualSubField = 0;
    string Before = "";
    string BeforeMark = "";

    for(int i=0;i<Elements.size();i++)
    {
        string ActMark = *(MarkElements[i]);
        string ActElem = *(Elements[i]);
        //nove pole
        if(ActMark=="M") {
           if(BeforeMark=="O") { //Vlozene pole
              BeforeLinkingActualField=ActualField; //zapamatuju si to stare
              ActualField=ActualSubField->LinkingField;
           } else {
              ActualField=new CField();
              ControlFields.push_back(ActualField);
           }
           *(ActualField->ID)=ActElem;
        }
        
        if(ActMark=="P") {
           //zpet stare pole
           ActualField=BeforeLinkingActualField;
        }
        
        //nove podpole
        if(ActMark=="Q") {
           ActualSubField=new CSubfield();
           *(ActualSubField->ID)=ActElem;
           ActualSubField->Index=0;
           ActualField->ptrCollSubf.push_back(ActualSubField);
        }
        
        //vycet hodnot
        if(ActMark=="B") {
           if(BeforeMark=="M") { //POLE
              *(ActualField->ID1) = ActElem;
           }
           if(BeforeMark=="B") { //ZAVORKY
              *(ActualField->ID2) = ActElem;
           }
           if(BeforeMark=="W") { //DELKA
              *(ActualSubField->Value) = ActElem;
           }
        }
        
        //delka
        if(ActMark=="W") {
           if(BeforeMark!="Q") {
              g_Error="Invalid definition size";  
              return;
           }
           int tNumber=atoi(ActElem.c_str());
           if (tNumber!=0) {
            ActualSubField->Index= tNumber;
           }
        }
        
        if(ActMark=="W" || ActMark=="B" || ActMark=="M" || ActMark=="Q" || ActMark=="O" ) {
          BeforeMark = ActMark;
          Before = ActElem;
        }
    }
    /*
    for(int i=0;i<ControlFields.size();i++)
    {
        cout << "Field" << *(ControlFields[i]->ID) << "\n";
        cout << "---------------------------------" << "\n";
        cout << "ID1" << *(ControlFields[i]->ID1) << "\n";
        cout << "ID2" << *(ControlFields[i]->ID2) << "\n";

        for(int j=0;j<ControlFields[i]->ptrCollSubf.size();j++)
        {
            cout << "Subfield: " << *(ControlFields[i]->ptrCollSubf[j]->ID) << "\n";
            cout << "Value: " << *(ControlFields[i]->ptrCollSubf[j]->Value) << "\n";
            cout << "Index: " << ControlFields[i]->ptrCollSubf[j]->Index << "\n";
            CField* LinkField = ControlFields[i]->ptrCollSubf[j]->LinkingField;
            if(*(LinkField->ID)!="") {
               cout << "Linking: " << *(LinkField->ID) << "\n";
               for(int k=0;k<LinkField->ptrCollSubf.size();k++) {
                  cout << "LINKING_Subfield: " << *(LinkField->ptrCollSubf[k]->ID) << "\n";
                  cout << "LINKING_Value: " << *(LinkField->ptrCollSubf[k]->Value) << "\n";
                  cout << "LINKING_Index: " << LinkField->ptrCollSubf[k]->Index << "\n";
               }
            }
        }
     }*/
	return;
}

/*
	+ - prvkek je povinn� a opakovateln�
	_ - prvek je pot�ebn� a neopakovateln�
	* - prvek je nepot�ebn� a opakovateln�
	? - prvek je nepot�ebn� a neopakovateln�
*/

string CMarcAnalyzer::CheckRecord(string Lang)
{
    string Errors; //vystup
	string ControlID; //ID aktu�ln�ho pravidla
	string ControlChar; //Znak vyskytu u aktualniho pravidla pole (?_*+)
	string ControlID1;
    string ControlID2;
    
    //kontrola po polich pro ktere jsou definovana pravidla
    for(int i=0;i<ControlFields.size();i++)
    {
        ControlID=ControlFields[i]->ID->substr(0,3);
		ControlChar=ControlFields[i]->ID->substr(3,1);
		ControlID1=*(ControlFields[i]->ID1);
		ControlID2=*(ControlFields[i]->ID2);

		if(ControlID=="LAB") {
		   for(int j=0;j<ControlFields[i]->ptrCollSubf.size();j++)
		   {
              ControlID=ControlFields[i]->ptrCollSubf[j]->ID->substr(0,1);
		      ControlChar=ControlFields[i]->ptrCollSubf[j]->ID->substr(1,1);
		      string Value = *(ControlFields[i]->ptrCollSubf[j]->Value);
		      int IndexS = ControlFields[i]->ptrCollSubf[j]->Index;
		      if(ControlID=="E") {
		         if (IndexS!=0) {
        		    if (g_Label->length()!=IndexS) {
        		        if (Lang=="CZE")  
                             Errors+="#LAB#: �patn� d�lka n�v�t� (d�lka: '" + OrdinalForm(Value.length(),0) + "' po�adovan� d�lka: '" + OrdinalForm(IndexS, 0) + "')\r\n"; 	   
                        else     
                             Errors+="#LAB#: Invalid size label(actual size: '" + OrdinalForm(Value.length(),0) + "' required size: '" + OrdinalForm(IndexS, 0) + "')\r\n"; 	   
                    } else {
                        if(Value!="") {
                           vector<string*> outArray;        
                           SplitString(&Value,&outArray,",");
                           if(outArray.size()!=IndexS) {
                              if (Lang=="CZE")  
                                 Errors+="#LAB#:D�lka mo�n�ch hodnot(d�lka='" + OrdinalForm(outArray.size(),0) + "') pro pozice v n�v�t� neodpov�d� d�lce n�v�t� (d�lka: '" + OrdinalForm(IndexS, 0) + "')\r\n"; 
                              else   
                                 Errors+="#LAB#:Size of possible values(size='" + OrdinalForm(outArray.size(),0) + "') for possitions in label does not equals size of label(size: '" + OrdinalForm(IndexS, 0) + "')\r\n"; 
                           } else {
                              for(int k=0;k<outArray.size();k++) {
                                 string controlOnePoss = *(outArray[k]);
                                 string oneRecChar = g_Label->substr(k,1);
                                 if (CheckChar(oneRecChar,controlOnePoss)) {
                                   if (Lang=="CZE")
                                      Errors+="#LAB#:N�v�t� obsahuje �patn� znak na pozici '" + OrdinalForm(k,0) + "' (hodnota: '" + oneRecChar + "' mo�n� hodnoty: '" + controlOnePoss + "')\r\n"; 
                                   else
                                      Errors+="#LAB#:Label contains invalid character in possition '" + OrdinalForm(k,0) + "' (actual value: '" + oneRecChar + "' possible value: '" + controlOnePoss + "')\r\n"; 
                                 }
                              }
                           }
                           for(int i=0;i<outArray.size();i++)
                               delete outArray[i];
                       }    
		           }
		         }  
		      }     
		   }
		   
		   
		} else {

		 int NumArr=0; //po��t� v�skyty tohoto pole 
		 for(int j=0;j<Fields.size();j++) //proch�z�m vsechny pole
		 {
		    if (*(Fields[j]->ID)==ControlID) { 
                //ID1
				if (CheckChar(*(Fields[j]->ID1),ControlID1)) {
				    if (Lang=="CZE")
                       Errors+="#" + ControlID + "#: " + "Prvn� indik�tor pole '" + ControlID + "' obsahuje �patnou hodnotu (hodnota: '" + *(Fields[j]->ID1) + "' mo�n� hodnoty: '" + ControlID1 + "')\r\n";  
                    else 
		               Errors+="#" + ControlID + "#: " + "First indicator from field '" + ControlID + "' contains invalid character (actual value: '" + *(Fields[j]->ID1) + "' possible values: '" + ControlID1 + "')\r\n"; 
				}

				//ID2
				if (CheckChar(*(Fields[j]->ID2),ControlID2)) {
				   if (Lang=="CZE")
				    Errors+="#" + ControlID + "#: " + "Druh� indik�tor pole '" + ControlID + "' obsahuje �patnou hodnotu (hodnota: '" + *(Fields[j]->ID2) + "' mo�n� hodnoty: '" + ControlID2 + "')\r\n"; 
				   else
				    Errors+="#" + ControlID + "#: " + "Second indicator from field '" + ControlID + "' contains invalid character (actual value: '" + *(Fields[j]->ID2) + "' possible values: '" + ControlID2 + "')\r\n"; 
				}


                Errors+=CheckSubfields(ControlFields[i],Fields[j],1, Lang);
			    NumArr++;
            }	
         }    
         if (ControlChar=="+" && NumArr<1) {
            if (Lang=="CZE")
              Errors+="#" + ControlID + "#: " + "Pole '" + ControlID + "' je povinn�" + "\r\n";
            else
			  Errors+="#" + ControlID + "#: " + "Field '" + ControlID + "' is mandatory" + "\r\n";
         }
		 if (ControlChar=="_" && NumArr==0) {
		    if (Lang=="CZE")
		      Errors+="#" + ControlID + "#: " + "Pole '" + ControlID + "' je povinn� " + "\r\n";
		    else
			  Errors+="#" + ControlID + "#: " + "Field '" + ControlID + "' is mandatory " + "\r\n";
		 }
		 if (ControlChar=="_" && NumArr>1) {
		 	if (Lang=="CZE")
		 	  Errors+="#" + ControlID + "#: " + "Pole '" + ControlID + "' nen� opakovateln� " + "\r\n";
            else		
   		 	  Errors+="#" + ControlID + "#: " + "Field '" + ControlID + "' is not repeatable " + "\r\n";
		 }
		 if (ControlChar=="?" && NumArr>1) {
			if (Lang=="CZE")
			  Errors+="#" + ControlID + "#: " + "Pole '" + ControlID + "' nen� opakovateln�" + "\r\n"; 	   
			else
              Errors+="#" + ControlID + "#: " + "Field '" + ControlID + "' is not repeatable" + "\r\n";
		 }
			
	    }

     }
     //kontrola po polich zaznamu o kterych se pravidla nezminuji
     for(int j=0;j<Fields.size();j++)  {
		 int found = 0;
         for(int i=0;i<ControlFields.size();i++) {
            ControlID=ControlFields[i]->ID->substr(0,3);
            if(ControlID==*(Fields[j]->ID)) {
               found = 1;
            }
         }
         if(found==0) {
            if (Lang=="CZE")
              Errors+="#" + *(Fields[j]->ID) + "#: " + "Pole '" + *(Fields[j]->ID) + "' nen� deklarov�no v kontroln�m souboru" + "\r\n"; 
            else
              Errors+="#" + *(Fields[j]->ID) + "#: " + "Field '" + *(Fields[j]->ID) + "' is not declared in control file" + "\r\n"; 
         }
     }    
     
     
     return Errors;
}

string CMarcAnalyzer::CheckSubfields( CField* ControlF,  CField* RecF, int WithLinking, string Lang)
{
    string Errors=""; //vystup
	string ControlID; //ID aktu�ln�ho pravidla
	string ControlChar; //Znak vyskytu u aktualniho pravidla pole (?_*+)
    string ControlFieldID=ControlF->ID->substr(0,3);  
    int isall = 0;  
    for(int i=0;i<ControlF->ptrCollSubf.size();i++)
    {
        ControlID=ControlF->ptrCollSubf[i]->ID->substr(0,1);
		ControlChar=ControlF->ptrCollSubf[i]->ID->substr(1,1);
		if (ControlID=="&")
          isall = 1;
         
        
		//if (WithLinking==0) cout << "L:" <<ControlID << "\n";
		int NumArr=0; //po��t� v�skyty tohoto pole 
		for(int j=0;j<RecF->ptrCollSubf.size();j++) //proch�z�m vsechny pole
		{
		    if (*(RecF->ptrCollSubf[j]->ID)==ControlID)
			{
			   Errors+=CheckOneSubfield(ControlFieldID, ControlF->ptrCollSubf[i],RecF->ptrCollSubf[j],WithLinking, Lang);
			   NumArr++;
            }	
            
        }    
        if (ControlChar=="+" && NumArr<1) {
            if (WithLinking==0) Errors+="LINKING";   
            if (Lang=="CZE")
               Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Podpole '" + ControlID + "' je povinn� v poli '" + ControlFieldID + "'\r\n";
            else
			   Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Subfield '" + ControlID + "' is mandatory in field '" + ControlFieldID + "'\r\n";
		}
		if (ControlChar=="_" && NumArr==0) {
		    if (WithLinking==0) Errors+="LINKING";   
		    if (Lang=="CZE")
		       Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Podpole '" + ControlID + "' je povinn� v poli '" + ControlFieldID + "'\r\n";
		    else
			   Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Subfield '" + ControlID + "' is mandatory in field '" + ControlFieldID + "'\r\n";
		}
		if (ControlChar=="_" && NumArr>1)  {
		    if (WithLinking==0) Errors+="LINKING";   
		    if (Lang=="CZE")
			   Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Podpole '" + ControlID + "' nen� opakovateln� v poli '" + ControlFieldID + "'\r\n";	
			else
               Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Subfield '" + ControlID + "' is not repeatable in field '" + ControlFieldID + "'\r\n";	    
		}
		if (ControlChar=="?" && NumArr>1) {
		    if (WithLinking==0) Errors+="LINKING"; 
		    if (Lang=="CZE")
		       Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Podpole '" + ControlID + "' nen� opakovateln� v poli '" + ControlFieldID + "'\r\n"; 	   
		    else
			   Errors+="#" + ControlFieldID + "$" + ControlID + "#: " + "Subfield '" + ControlID + "' is not repeatable in field '" + ControlFieldID + "'\r\n"; 	   
		}	
     }
     //kontrola podpoli nevyskytujicich se v kontrolnim souboru
     if(isall==0) {
      for(int j=0;j<RecF->ptrCollSubf.size();j++) {
        int found = 0;
        for(int i=0;i<ControlF->ptrCollSubf.size();i++) {
            ControlID=ControlF->ptrCollSubf[i]->ID->substr(0,1);
            if (*(RecF->ptrCollSubf[j]->ID)==ControlID) {
			   found=1;
            }    
        }
        if(found==0) {
            if (WithLinking==0) Errors+="LINKING"; 
            if (Lang=="CZE")
               Errors+="#" + ControlFieldID + "$" + *(RecF->ptrCollSubf[j]->ID) + "#: " + "Podpole '" + *(RecF->ptrCollSubf[j]->ID) + "' v poli '" + ControlFieldID + "' nen� deklarov�no v kontroln�m souboru" + "\r\n"; 
            else
               Errors+="#" + ControlFieldID + "$" + *(RecF->ptrCollSubf[j]->ID) + "#: " + "Subfield '" + *(RecF->ptrCollSubf[j]->ID) + "' in field '" + ControlFieldID + "' is not declared in control file" + "\r\n"; 
        }
      }
      if (WithLinking) {
      //proch�z�m vsechny pole kvuli vlozenym polim
       for(int j=0;j<RecF->ptrCollSubf.size();j++) {
		    if (*(RecF->ptrCollSubf[j]->ID)=="1") {
			   //nasel jsem vlozene podpole
			   CField * LinF = RecF->ptrCollSubf[j]->LinkingField;
			   if(*(LinF->ID)!="") {
			       for(int i=0;i<ControlF->ptrCollSubf.size();i++)
    			   {
            		  ControlID=ControlF->ptrCollSubf[i]->ID->substr(0,1);
            		  ControlChar=ControlF->ptrCollSubf[i]->ID->substr(1,1);
            		  if(ControlID=="1") {
            		     CField * ControlLinF = ControlF->ptrCollSubf[i]->LinkingField;
            		     string ContolLinkID = ControlLinF->ID->substr(0,3);  
            		     if(*(LinF->ID)==ContolLinkID) {
                              //ID1
			 	              if (CheckChar(*(LinF->ID1),*(ControlLinF->ID1))) {
			 	                if (Lang=="CZE")
			 	                   Errors+="LINKING#" + ContolLinkID + "#: " + " Prvn� indik�tor pole '" + ContolLinkID + "' obsahuje �patnou hodnotu (hodnota: '" + *(LinF->ID1) + "' mo�n� hodnoty: '" + *(ControlLinF->ID1) + "')\r\n";                   			 	                   
			 	                else
		                           Errors+="LINKING#" + ContolLinkID + "#: " + " First indicator from field '" + ContolLinkID + "' contains invalid character (actual value: '" + *(LinF->ID1) + "' possible values: '" + *(ControlLinF->ID1) + "')\r\n"; 
				              }
				              //ID2
			 	              if (CheckChar(*(LinF->ID2),*(ControlLinF->ID2))) {
			 	                if (Lang=="CZE")
			 	                  Errors+="LINKING#" + ContolLinkID + "#: " + " Druh� indik�tor pole '" + ContolLinkID + "' obsahuje �patnou hodnotu (hodnota: '" + *(LinF->ID2) + "' mo�n� hodnoty: '" + *(ControlLinF->ID2) + "')\r\n"; 
			 	                else
		                          Errors+="LINKING#" + ContolLinkID + "#: " + " Second indicator from field '" + ContolLinkID + "' contains invalid character (actual value: '" + *(LinF->ID2) + "' possible values: '" + *(ControlLinF->ID2) + "')\r\n"; 
				              }
            		        Errors+=CheckSubfields(ControlLinF,LinF,0, Lang);
            		     }
                      }
            	   }
			   }
            }	
            
       }
      }   
     }
     return Errors;
}

string CMarcAnalyzer::CheckOneSubfield( string sField, CSubfield* ControlSF,  CSubfield* RecSF, int WithLinking, string Lang)
{
     string Errors=""; 
     string ControlID=ControlSF->ID->substr(0,1);
     
     if (ControlSF->Index!=0) {
        if (RecSF->Value->length()!=ControlSF->Index) {
           if (WithLinking==0) Errors+="LINKING"; 
           if (Lang=="CZE")
              Errors+="#" + sField + "$" + ControlID + "#: " + "Podpole '" + ControlID + "' m� nespr�vnou d�lku(d�lka: '" + OrdinalForm(RecSF->Value->length(),0) + "' pot�ebn� d�lka: '" + OrdinalForm(ControlSF->Index, 0) + "') v poli '" + sField + "'\r\n"; 	   
           else
              Errors+="#" + sField + "$" + ControlID + "#: " + "Subfield '" + ControlID + "' contains invalid size(actual size: '" + OrdinalForm(RecSF->Value->length(),0) + "' required size: '" + OrdinalForm(ControlSF->Index, 0) + "') in field '" + sField + "'\r\n"; 	   
        } else {
           if(*(ControlSF->Value)!="") {
              vector<string*> outArray;        
              SplitString(ControlSF->Value,&outArray,",");
              if(outArray.size()!=ControlSF->Index) {
                 if (WithLinking==0) Errors+="LINKING "; 
                 if (Lang=="CZE")
                    Errors+="D�lka mo�n�ch hodnot(d�lka='" + OrdinalForm(outArray.size(),0) + "') pro pozice v podpoli '" + ControlID + "' neodpov�d� d�lce podpole(d�lka: '" + OrdinalForm(ControlSF->Index, 0) + "') v poli '" + sField + "'\r\n"; 
                 else
                    Errors+="Size of possible values(size='" + OrdinalForm(outArray.size(),0) + "') for possitions in subfields '" + ControlID + "' does not equals size of subfield(size: '" + OrdinalForm(ControlSF->Index, 0) + "') in field '" + sField + "'\r\n"; 
              } else {
                 for(int i=0;i<outArray.size();i++) {
                    string controlOnePoss = *(outArray[i]);
                    string oneRecChar = RecSF->Value->substr(i,1);
                    if (CheckChar(oneRecChar,controlOnePoss)) {
                       if (WithLinking==0) Errors+="LINKING "; 
                       if (Lang=="CZE")
                          Errors+="#" + sField + "$" + ControlID + "#: " + " Podpole '" + ControlID + "' obsahuje �patn� znak na pozici '" + OrdinalForm(i,0) + "' (hodnota: '" + oneRecChar + "' mo�n� hodnoty: '" + controlOnePoss + "') v poli '" + sField + "'\r\n"; 
                       else
                          Errors+="#" + sField + "$" + ControlID + "#: " + " Subfield '" + ControlID + "' contains invalid character in possition '" + OrdinalForm(i,0) + "' (actual value: '" + oneRecChar + "' possible value: '" + controlOnePoss + "') in field '" + sField + "'\r\n";    
                    }
                 }
              }
              for(int i=0;i<outArray.size();i++)
                 delete outArray[i];    
           }
        }
     }
     return Errors;
}

int CMarcAnalyzer::CheckChar( string sOneChar, string sCheckString) {
    int found=0;
    for(int h=0;h<sCheckString.length();h++) {
        string onePoss=sCheckString.substr(h,1);
        if(onePoss==sOneChar) {
           found=1;
        } else {
           if(onePoss=="&") found=1;
           if(onePoss=="S" && sOneChar==" ") found=1;
           if(onePoss=="S" && sOneChar=="^") found=1;
           if(onePoss=="S" && sOneChar=="-") found=1;
           if(onePoss=="S" && sOneChar=="") found=1;
         }
    }
    if (found==0)
        return 1;
    else
        return 0;
}

/***********************************************************************
 *
 * FUNCTION:     MarkSections
 *
 * DESCRIPTION:  funkce vezme analyzovany text a vyhleda v něm části začínajíci chBegin a končíci chEnd a označí
 *               je ve stejně dlouhým stringu strMark identifikatory chMarkBegin a chMarkEnd. Je-li promenna iInsertField
 *               1 označí se vcetne konecneho chEnd
 *
 * PARAMETERS:   (IN)   string* strAnalyzeText - analyzovany text
 *               (OUT)	string* strMarks - označkovaný text
 *               (IN)	char chBegin - hledaný začátek
 *               (IN)	char chEnd - hledaný konec
 *               (IN)	char chMarkBegin - znacka zacatku
 *               (IN)	char chMarkEnd - znacka konce
 *               (IN)	int iInsertEnd - měl se vložit i konecny znak?
 *
 * AUTOR:        Jindřich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CMarcAnalyzer::MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd)
{
    int iCounter=0;
    int iPossitionStart;
    int iPossitionStop;
    
    while (iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(chBegin,iCounter);
        
        if (iPossitionStart==(-1)) //��dn� �et�zec nenalezen
            return 0;
      
        (*strMarks)[iPossitionStart]=chMarkBegin; //Zacatek retezce

        if (iPossitionStart+1<=strAnalyzeText->length())
            iPossitionStop=strAnalyzeText->find(chEnd,iPossitionStart+1);
        else
            return 0;

        if (iPossitionStop==(-1)) //Konec �et�zce nenalezen
            return 0;
        
        if (iInsertEnd)
            (*strMarks)[iPossitionStop]=chMarkEnd;
        else
            (*strMarks)[iPossitionStop-1]=chMarkEnd;
        iCounter=iPossitionStop+1;
    }
    return 0;
}


int CMarcAnalyzer::MarkSections2(string * strAnalyzeText, string * strMarks, char const * chBegin, char const * chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd)
{
    int iCounter=0;
    //int h;
    int iPossitionStart;
    int iPossitionStop;
    string chB=chBegin;
    string chE=chEnd;
    while (iCounter<strAnalyzeText->length())
    {
        iPossitionStart=strAnalyzeText->find(chB,iCounter);
        
        if (iPossitionStart==(-1)) //žádný řetězec nenalezen
            return 0;
      
        if (iPossitionStart>52522)
        {
           //h=0;
        }

        (*strMarks)[iPossitionStart]=chMarkBegin; //Zacatek retezce

        if (iPossitionStart+1<=strAnalyzeText->length()) 
            iPossitionStop=strAnalyzeText->find(chEnd,iPossitionStart+1);
        else
            return 0;

        if (iPossitionStop==(-1)) //Konec �et�zce nenalezen
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
 * DESCRIPTION:  funkce vezme omarkovaný text tvaru XXXXXXXCXXXMXXXXEXXXXNXX a vyplní v něm
 *               sekce začínající chMarkBegin a končící chMarkEnd charem chMarkBegin.Při C,N by
 *               vysledek byl XXXXXXXCCCCCCCCCCXXXXNXX
 *
 * PARAMETERS:   (OUT)	string* strMarks - označkovaný text
 *               (IN)	char chMarkBegin - hledaný začátek
 *               (IN)	char chMarkEnd - hledaný konec
 *
 * AUTOR:        Jindřich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

int CMarcAnalyzer::FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd)
{
    int iPossitionStart;
    int iPossitionStop;    
    int iCounter=0;
    while (iCounter<strMark->length())
    {
        iPossitionStart=strMark->find(chMarkBegin,iCounter);
    
        if (iPossitionStart==(-1)) //žádny řetězec nenalezen
            return 0;

        if (iPossitionStart+1<=strMark->length())
            iPossitionStop=strMark->find(chMarkEnd,iPossitionStart+1);
        else
            iPossitionStop=strMark->length()-1;

        if (iPossitionStop==(-1)) //Konec �et�zce nenalezen
            iPossitionStop=strMark->length()-1;
    
        for (int i=iPossitionStart;i<iPossitionStop;i++)
            (*strMark)[i]=chMarkBegin; //Zacatek retezce
        (*strMark)[iPossitionStop]='*';
                
        iCounter=iPossitionStop+1;
    }

    return 0;
} 

/***********************************************************************
 *
 * FUNCTION:     CutCommandByType
 *
 * DESCRIPTION:  funkce vezme analyzovan� a omarkovan� text a vytvo�� pole analyzovan�ch
 *               string� a markovac�ch string� d�lky jedna. Funkce analyzovan� text d�l� podle 
 *               identifikovan�ch sekc� vznikl�ch vypln�n�m markovac�ho stringu
 *
 * PARAMETERS:   (IN)    string* strCommand - analyzovan� text
 *               (IN)	 string* strMarkCommand -omarkovan� text
 *               (OUT)	 stringArray *ptrElements - jednotliv� ��sti analyzovan�ho textu podle sekc�
 *               (IN)	 stringArray *ptrMarkElements - markovac� stringy d�lky jedna ozna�uj�c� typ sekce
 *
 * AUTOR:        Jind�ich Stejskal (Jindrich.Stejskal@seznam.cz)
 * 
 * DATE:         1.11.2002
 *
 ***********************************************************************/

void CMarcAnalyzer::CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements)
{
    char chLastChar='J';
    char chNowChar;
    int iElementCounter=-1;
    string newChar;

    for (int i=0;i<strMarkCommand->length();i++)
    {
        chNowChar=(*strMarkCommand)[i];
        //cout << chNowChar << "\n";
        if ((chNowChar!=chLastChar && chNowChar!='*') || i==0)
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
            if(chNowChar=='*') chLastChar='*';
            string* ptrStr;
            ptrStr =(*ptrElements)[iElementCounter];
            *ptrStr+=(*strCommand)[i];
            (*ptrElements)[iElementCounter]= ptrStr;
        }
    }
} 
