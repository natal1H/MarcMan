#ifndef CMARCANALYZER_H
#define CMARCANALYZER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

extern int codeVersion;

using namespace std;

#define LOAD_ISO_FORMAT   1
#define LOAD_TEXT_FORMAT  2
#define LOAD_ALEPH_FORMAT 3
#define NEW_ISO_FORMAT    4
#define NEW_TEXT_FORMAT   5

#define MAXBUF           5000
#define LENGTH_ID_ALEPH  9

#define VERSION_TESTING_E     3
 
extern char ISO_SUBFIELD_SEPARATOR[];
extern char ISO_FIELD_SEPARATOR[];
extern char ISO_RECORD_SEPARATOR[];
extern char TEXT_RECORD_SEPARATOR[];
extern char TEXT_SUBFIELD_SEPARATOR[];
extern char TEXT_FIELD_SEPARATOR[];
extern char ALEPH_RECORD_SEPARATOR[];
extern char ALEPH_SUBFIELD_SEPARATOR[];
extern char ALEPH_FIELD_SEPARATOR[];
extern char ALEPH_LAB_IDENTIFIKATOR[];

extern char LINKING_SUBFIELD_IDENTIFICATOR[];
extern char LINKING_SUBFIELD_SEP_ISO[];
extern char LINKING_SUBFIELD_SEP_TXT[];
extern char LINKING_SUBFIELD_SEP_ALEPH[];


class CField;

class CSubfield
{
public:
    int Index;
    string* Value;
    string* ID;
    CField* LinkingField;
	CSubfield();
    ~CSubfield();
};

class CField {
public:
    string* ID;
    string* ID1;
    string* ID2;
    int Index;
    vector<CSubfield*> ptrCollSubf;
	CField()
	{
	    ID=new string();
        ID1=new string(" ");
        ID2=new string(" ");
    }
    ~CField()
    {
        delete ID;
        delete ID1;
        delete ID2;
        for(int i=0;i<ptrCollSubf.size();i++)
        {
          delete ptrCollSubf[i];
        }  
    }
};




class CMarcAnalyzer {
public:
    vector<CField*> Fields;
    vector<CField*> ControlFields;
    vector<string*> aNoIdenField;
    vector<string*> aLinkingField;

    string g_Error;    
    string* g_Label;
    string* g_FieldPrefix;
    string* g_Format;
    
    int SetLog(ofstream* lg, int LogAllMes);
	ofstream* log;
	int LogAll;
    
	CMarcAnalyzer()
	{
       LogAll=0; 
	   g_Label=new string("                        ");
       g_FieldPrefix=new string("");
	   g_Format=new string("");
	}
	
    ~CMarcAnalyzer()
    {
       int i;
       for(i=0;i<aNoIdenField.size();i++)
             delete aNoIdenField[i];
       for(i=0;i<Fields.size();i++)
             delete Fields[i];  
       delete g_Label;
       delete g_FieldPrefix;
       delete g_Format;                
    }

public:
	string CheckRecord(string Lang);
	void LoadCheck(const char* sPath);
    int SortFields();
    int OpenRecordFromString(string* strRecord, int OpenMode);
	string* ValueByCSetPossWithPointers(string* Command, long loIF, long loIS, long PointerField, long PointerLinkingField, long PointerSubField);
	string* ValueByCommandSetPoss(string* Command, long loIF, long loIS);
	string* ValueByCommand(string* Command);
	void SetValueByCommand(string* Command, string* Value, long IField, long ISubfield, long Add);
	string* GetSerializeRecord(long Mode);
	long MaxIndexByCommand(string* Command, long FIndex);
    int FillFieldsDepth();
	void EmptyRecord();
    void CloneField(CField* Dest, CField* Sour);

private:
    string OrdinalForm(int number, int size);
    int SplitString(string* strSplit, vector<string*> * outArray, char* Separator);
    void Replace(string* strSplit, char* findStr, char* repStr);
    int AddOneField(string* sField, CField* pField, char* SubSeparator, char* LinkingSeparator);
    bool MayBeLinking(string Field); 
    int SerializeRecord(string *outString, string *Sizes, char* SubfieldSeparator, char* FieldSeparator, int Mode);
    int random_range(int lo, int hi);
    int FillSubfieldsDepth(vector<CSubfield*>* pcollS, int Link);
    string* GetValueFromC(string IDField,string LinkField, string IDSub,long IndexField,long IndexSubField, int Indikator, int Start, int Stop, long* PointerField, long* PointerSubField, long* PointerLinkingField);
    int RecognizeCommand(string* Comm, string *LAB, string *IDF, string *ILF, string *IDS, long* IF, long* IS, int* Indic, int* Sta, int* Sto);
    int SetValueFromC(string* Value, string *IDField,string* LinkField, string* IDSub,long IndexField,long IndexSubField, int Indikator, int Start, int Stop, int Add);
    long MaxLinkingSubfieldIndex(string* sIDF, string* sIDL, string* sSubID, long FIndex);
    long MaxSubfieldIndex(string* sIDF, string* sSubID, long FIndex);
    long MaxFieldIndex(string* sIDF);
    long MaxLinkingFieldIndex(string* sIDF, long IndexField, string* sIDL);
    void TrimRight(string* sStr, char* ArraySep);
    void TrimLeft(string* sStr, char* ArraySep);
    int MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd);
    int FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd);
    void CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements);    
    int MarkSections2(string* strAnalyzeText, string* strMarks, char* chBegin, char* chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd);
    string CheckSubfields( CField* ControlF,  CField* RedF, int WithLinking, string Lang);
    string CheckOneSubfield( string sField, CSubfield* ControlSF,  CSubfield* RecSF, int WithLinking, string Lang);
    int CheckChar( string sOneChar, string sCheckString);
    };

#endif
