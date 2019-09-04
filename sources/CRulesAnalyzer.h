#ifndef CRULESANALYZER_H
#define CRULESANALYZER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

using namespace std;

#define MAXBUF 5000

//#define ADMIN_VERSION

extern int codeVersion;

#define VERSION_LIGHT         0
#define VERSION_STANDARD      1
#define VERSION_ADMIN         2
#define VERSION_TESTING       3

#define MARCMAN_VERSION       "2.3"

extern char const * CUTTRIMED;

class CRuleItem 
{
public:
    string LAB;
    string IDF;
    string ILF;
    string IDS;
    string From;
    string To;
    long INDIC;
    long Start;
    long Stop;
    long g_Yes;
};


class CRule
{
public:
	string Command;
    string From;
    string To;
    vector<CRuleItem *> CollRulesItem;
    CRule()
	{
	}
    ~CRule()
    {
       for(int i=0;i<CollRulesItem.size();i++)
          delete CollRulesItem[i];
    }
};

class CCollRulesByC 
{
public:
	vector<int> IndexColl;
	vector<int> IndexInRuleColl;
    vector<CRule *> CCollection;
    void Add(CRule *ptrRule, int Index, int IndexInItem)
    {
       CCollection.push_back(ptrRule);
       IndexColl.push_back(Index);
       IndexInRuleColl.push_back(IndexInItem);
    }
};

class CRulesAnalyzer
{
public:
    string g_Error;
    vector<CRule*> CollRules;
    vector<CRule*> CollRulesByC;
    map<string,CCollRulesByC*> mapRulByC;
    int SetLog(ofstream* lg, int LogAllMes);
	ofstream* log;
	int LogAll;

	CRulesAnalyzer()
	{
		LogAll=0;
	}
    ~CRulesAnalyzer()
    {

       for(int i=0;i<CollRules.size();i++)
          delete CollRules[i];
       map<string,CCollRulesByC*>::iterator g;   
       for(g=mapRulByC.begin();g!=mapRulByC.end();g++)
       {
          delete g->second;
          //mapRulByC.erase(g);   
       }   
	   mapRulByC.clear();
    }

public:
	CCollRulesByC* GetRulesByC(string* Command);
	int Update();
	int OpenFromFile(const char* sPath);
    int IsForC(string* Comm);	   
    int OpenFromCodeFile(const char* sPath);	    
private:
    int CodeStringChar(char* sRet, int size);
    int MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd);
    int FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd);
    void CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements);    
    int MarkSections2(string* strAnalyzeText, string* strMarks, char const * chBegin, char const * chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd);
    int RecognizeCommand(string* Comm, string *LAB, string *IDF, string *ILF, string *IDS, long* IF, long* IS, int* Indic, int* Sta, int* Sto);    
    int SplitString(string* strSplit, vector<string*> * outArray, char const * Separator);
    void TrimRight(string* sStr, char const * ArraySep);
    void TrimLeft(string* sStr, char const * ArraySep);
    string OrdinalForm(int number, int size);    
    int LoadCommand(string* Comm, CCollRulesByC* newCOM);
    };

#endif
