#ifndef CCONVERTOR_H
#define CCONVERTOR_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

#include "CUseScript.h"
#include "CMarcAnalyzer.h"
#include "CRulesAnalyzer.h"
#include "CPluginManager.h"

class CConfigFileReader
{
public:
    int ReadFile(char * sPath, map<string, M_EXPRESSION_ELEMENT*>* VIt);
	int SetLog(ofstream* lg, int LogAllMes);
	ofstream* log;
	int LogAll;
    map<string, M_EXPRESSION_ELEMENT*> VItems;
    ~CConfigFileReader()
    {
      map<string,M_EXPRESSION_ELEMENT*>::iterator j;
      for(j=VItems.begin();j!=VItems.end();j++)
      {
           // cout << "delete: " << j->first << "-" << *(j->second->sName)  << "\n";
            delete j->second;
            //VItems.erase(j);
      }  
	  VItems.clear();
    }
};

class CConfigArray
{
public:
    vector<M_EXPRESSION_ELEMENT *> VItems;
    ~CConfigArray()
    {
      for(int j=0;j!=VItems.size();j++)
      {
            delete VItems[j];
      }   
	  VItems.clear();
    }
};

class CConvertor;

class CConvertor
{
public:
    map<string,M_EXPRESSION_ELEMENT*> Variables;
    map<string,CConfigFileReader*> Tables;
    map<string,CConfigArray*> Arrays;
    int LoadAll(int Code);
    int DoneAll();
    int InsertRecord(string* Record, int Format);
    int Convert();
	int SetLog(ofstream* lg, int LogAllMes);
	ofstream* log;
	int LogAll;
    string sError;
    string* GetOut(int Format);
    int SaveCodeRulesFile(char *sPath);
    CConvertor()
    {
      LogAll=0; 
      MarcIn=new CMarcAnalyzer();
      MarcOut=new CMarcAnalyzer();
      RulesAnalyzer=new CRulesAnalyzer();
      PluginManager=new CPluginManager();
    }
    ~CConvertor()
    {
      delete MarcIn;
      delete MarcOut;
      //delete RulesAnalyzer;
      delete PluginManager;
      map<string,M_EXPRESSION_ELEMENT*>::iterator j;
      for(j=Variables.begin();j!=Variables.end();j++)
      {
           // cout << "delete: " << j->first << "\n"; 
           // cout << "uf: " << *(j->second->sName) << "\n";
            delete j->second;
      }      
	  Variables.clear();
      map<string,CConfigFileReader*>::iterator k;
      for(k=Tables.begin();k!=Tables.end();k++)
      {
         delete k->second;   
       //  Tables.erase(k);
      }   
	  Tables.clear();
      map<string,CConfigArray*>::iterator d;
      for(d=Arrays.begin();d!=Arrays.end();d++)
      {
         delete d->second;      
       //  Arrays.erase(d);
      }   
	  Arrays.clear();
      /*map<string,CUseScript*>::iterator w;
      for(w=Codes.begin();w!=Codes.end();w++)
      {
         delete w->second;
      //   Codes.erase(w);  
      }*/   
	  Codes.clear();
    }
//private:
    map<string,CUseScript*> Codes;
    CMarcAnalyzer* MarcIn;
    CMarcAnalyzer* MarcOut;
    CRulesAnalyzer* RulesAnalyzer;   
    CPluginManager* PluginManager;
    int ActualRule;
    
    int lPointerField;
    int lPointerSubField;
    int lPointerLinkingField;
    int DoingLinking;

    string* GroupExpand(string* sText);
    int DoElement(string* Value, string* Command, long iField, long iSubfield);
    int ClearErrors();
    int DoOneRecord();
    int DoRule(CUseScript* UseScript, string* sValue, string* sFrom, string* sTo, string* sCommand, long IndexRule, long iField, long iSubfield);
    void TrimRight(string* sStr, char* ArraySep);
    void TrimLeft(string* sStr, char* ArraySep);    
    int SplitString(string* strSplit, vector<string*> * outArray, char* Separator);
    int CreateVariable(string Key, int iType);
    int ChangePossition(vector<CSubfield*>* Coll, int Poss1, int Poss2); 
    int SortSubfields(vector<CSubfield*>* Coll, string* Command);    
    int IsInS(vector<string*>* St, CSubfield* SubF);   
};

#endif
