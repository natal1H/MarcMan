#ifndef CUSESCRIPT_H
#define CUSESCRIPT_H


#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;



//ERRORS
#define ERR_UNCLOSED_SECTION                1
#define ERR_IF_WITHOUT_THEN                 2
#define ERR_INVALID_COMBINATION_KEYWORDS    3
#define ERR_EMPTY_QUESTION                  4
#define ERR_EMPTY_COMMAND                   5
#define ERR_KEYWORD_NOT_ALONE               6
#define ERR_FOR_WITHOUT_TO                  7
#define ERR_EMPTY_INICIALIZE_FOR            8
#define ERR_EMPTY_QUESTION_FOR              9
#define ERR_EMPTY_QUESTION_WHILE            10
#define ERR_KEYWORDS_CROSS                  11
#define ERR_MORE_CLOSE_PARENTHESIS          12
#define ERR_VARIABLE_NOT_EXIST              13
#define ERR_MATHEMATIC_OPERATE_STRINGS      14
#define ERR_CANNOT_EQUALS_DIFFERENT_TYPE    15
#define ERR_OPERATORS_NEAR                  16
#define ERR_DIVISION_BY_ZERO                17
#define ERR_UNDEFINED                       18
#define ERR_METHOD_RETURNED_ERROR           19
#define ERR_ASSIGNING_NOT_IN_ROOT           20
#define ERR_BEFORE_ASSIGNING_NOT_VARIABLE   21
#define ERR_STRING_IN_QUESTION              22
#define ERR_EXTERNAL_ERROR                  23
#define ERR_KEYWORDS_NOT_CLOSED             24

//NODE TYPES
#define D_COMMAND                           0
#define D_IF_WITH_THEN                      1
#define D_IF_WITH_THEN_AND_ELSE             2
#define D_FOR                               3
#define D_WHILE                             4
#define D_NEXT                              5
#define D_WEND                              6
#define D_ELSE                              7
#define D_ENDIF                             8
#define D_IF                                9
#define D_EXIT                              10

//ELEMENT EXPRESION TYPES
#define EX_OPERATOR                         0
#define EX_VARIABLE                         1
#define EX_METHOD                           2
#define EX_STRING                           3
#define EX_INTEGER                          4
#define EX_COMMA                            5
#define EX_ASSIGNING                        6
#define EX_EMPTYMETHOD                      7
#define EX_ERROR                            8  

//EXTERNAL TYPES
#define EXTERNAL_ERROR                      0
#define EXTERNAL_INTEGER                    1
#define EXTERNAL_STRING                     2

#define MAXBUF 5000

//MARKS
extern const char BEGIN_OF_STRING;
extern const char END_OF_STRING;
extern const char BEGIN_OF_COMMENT;
extern const char END_OF_COMMENT;
extern const char BEGIN_OF_DIRECT;
extern const char END_OF_DIRECT;
extern const char BEGIN_OF_METHOD;
extern const char END_OF_METHOD;

extern const char UNRECOGNIZED;
extern const char EXPRESSION;

extern const char MARK_LEFT_PARENTHESIS;
extern const char MARK_RIGHT_PARENTHESIS;
extern const char MARK_COMMA;
extern const char MARK_EQUAL;
extern const char MARK_GREATER;
extern const char MARK_LESS;
extern const char MARK_NOEQUALS;
extern const char MARK_AND;
extern const char MARK_OR;
extern const char MARK_PLUS;
extern const char MARK_MINUS;
extern const char MARK_MULTIPLICATION;
extern const char MARK_DIVISION;
extern const char MARK_ASSIGNING;

//GROUPS CHARACTERS
extern char* DIRECT_NEIGHBOURHOOD;
extern char* METHOD_NEIGHBOURHOOD;
extern char* LINE_SEPARATORS;
extern char* TRIMED;
extern char SPACE;
extern char TABULATOR;
extern char ESCAPE;

//EXPRESSION
extern char* E_LEFT_PARENTHESIS;
extern char* E_RIGHT_PARENTHESIS;
extern char* E_COMMA;
extern char* E_EQUAL;
extern char* E_GREATER;
extern char* E_LESS;
extern char* E_NOEQUALS;
extern char* E_AND;
extern char* E_OR;
extern char* E_PLUS;
extern char* E_MINUS;
extern char* E_MULTIPLICATION;
extern char* E_DIVISION;
extern char* E_ASSIGNING;

//KEYWORDS
extern const string KEYWORD_IF;
extern const string KEYWORD_THEN;
extern const string KEYWORD_ELSE;
extern const string KEYWORD_ENDIF;
extern const string KEYWORD_FOR;
extern const string KEYWORD_TO;
extern const string KEYWORD_NEXT;
extern const string KEYWORD_WHILE;
extern const string KEYWORD_WEND;
extern const string KEYWORD_EXIT;

int EscapeToInt(string Text);

class M_EXPRESSION_ELEMENT
{
public:
    int iDepth;
    int iType;
    string* sName;
    int iIntValue;
    ~M_EXPRESSION_ELEMENT()
    {
       //cout << "deleteexpression\n";
       delete sName;
    }
};

class M_PROG_NODE
{
public:
	int shType;
    string* sCommand;
    string* sQuestion;
    int iTrueJump;
    int iFalseJump;
    vector<M_EXPRESSION_ELEMENT *> CommandExpression;
    vector<M_EXPRESSION_ELEMENT *> QuestionExpression;
    int MaxDepthCommand;
    int MaxDepthQuestion;
    ~M_PROG_NODE()
    {
        int i;
        for(i=0;i<CommandExpression.size();i++)
        {
            delete CommandExpression[i];
        }
        for(i=0;i<QuestionExpression.size();i++)
        {
            delete QuestionExpression[i];
        }
       // cout << "deleting" + *sCommand + "\n";
       // cout << "deleting" + *sQuestion + "\n";
        delete sCommand;
        delete sQuestion;
    }
};

class M_CODE
{
public:
    string* sCode;
    string* sMarkCode;
    vector<M_PROG_NODE *> ProgNodeArray;
    ~M_CODE()
    {
        for(int i=0;i<ProgNodeArray.size();i++)
        {
            delete ProgNodeArray[i];
        }
        delete sCode;
        delete sMarkCode;
    }
};


class CUseScript
{
public:
    map<string,M_EXPRESSION_ELEMENT*>* gVariables;
    vector<string *> aMethods;
    int intError;
    int intLineError;

    int SetLog(ofstream* lg, int LogAllMes);
	ofstream* log;
	int LogAll;

    M_CODE * Code;

	CUseScript()
    {
       Code = new M_CODE();
	   LogAll=0;
	}
    ~CUseScript()
    {
       delete Code;
       for (int i=0;i<aMethods.size();i++)
       {
          delete aMethods[i];
       }
    }

public:
    int Execute();
    int SetCode(string* sCode, int NoPrepare);
    string * SerializeProgNodes();
    int SetSerializeProgNodes(string* SerString);    
    string GetStringError();
    int EcsapeString(string *strText,char Escape);
    M_EXPRESSION_ELEMENT * (*ptrFunction)(void* ptrClientClass, string* MethodName, vector<M_EXPRESSION_ELEMENT *>* outarrElements);
    void* ptrClientClass;

private:

    int SplitString(string* strSplit, vector<string*>* outArray, char* Separator);
    
    string OrdinalForm(int number, int size);

    M_EXPRESSION_ELEMENT* AddNextExpresionElement(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int Depth, int Type, string* Name);

    M_EXPRESSION_ELEMENT* AddNextExpresionIntElement(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int Depth, int Type, int intValue);

    int AnalyzeExpression(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int* MaxDepth, string* sExpression);

    int CalculateOperation(M_EXPRESSION_ELEMENT* One, string Operator, M_EXPRESSION_ELEMENT* Two);

    M_EXPRESSION_ELEMENT* CallMethod(string* MethodName, vector<M_EXPRESSION_ELEMENT *>* arguments);

    int CompileMethod(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int BeginSection, int EndSection,int *Removed);

    int CopyConstArrayExpression(vector<M_EXPRESSION_ELEMENT *>* SourceArray, vector<M_EXPRESSION_ELEMENT *>* NewArray);

    void CutCommandByType(string* strCommand, string* strMarkCommand, vector<string *> *ptrElements, vector<string *> *ptrMarkElements);

    void DebugExpression(vector<M_EXPRESSION_ELEMENT *>* outarrElements, char* sFileName);

    void DebugProgNodes(vector<M_PROG_NODE *>* aProgNodes, char* sFileName);

    int DeleteSection(string* strAnalyzeText, string* strMarks, char chMarkSection);

    int Execute(vector<M_PROG_NODE *>* aProgNodes);

    int FillMarkSection(string* strMark, char chMarkBegin,  char chMarkEnd);

    int GetExpElementIntValue(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iIndex);

    string* GetExpElementName(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iIndex);

    int GetExpElementType(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iIndex);

    int GetNextSection(vector<M_EXPRESSION_ELEMENT *>* outarrElements, int iDepth, int* BeginSection, int* EndSection);

    M_PROG_NODE* GetPtrNode(vector<M_PROG_NODE *>* aProgNodes, int iIndex);

    int GetVariableValue(M_EXPRESSION_ELEMENT* Variable);

    int IntExpElementDepth(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iIndex);

    string* JoinSections(vector<string *> *ptrElements, int IndexBegin, int IndexEnd);

    int LinkNodes(vector<M_PROG_NODE *> *aProgNodes);

    int MakeNodes(vector<M_PROG_NODE *> *aProgNodes, vector<string *>* aLines, vector<string *> *aMarkLines);

    int MarkKeyword(string* strAnalyzeText, string* strMarks, string sSearchString, char chMarkBegin,  char chMarkEnd, char* chNeib);

    int MarkSections(string* strAnalyzeText, string* strMarks, char chBegin, char chEnd, char chMarkBegin,  char chMarkEnd, int iInsertEnd);

    int MarkSigns(string* strAnalyzeText, string* strMark, char *sign, char chMarkSign);

    int NextCommand(vector<string *> *ptrMarkElements, int Index, char FindMark);

    int NodesFromLines(vector<M_PROG_NODE *> *aProgNodes, string* strCommand, string* strMarkCommand);

    int PrepareNodes(vector<M_PROG_NODE *> *aProgNodes);

    int ProcessExpression(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int iDepth);

    int SectionCalculate(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int BeginIndex, int EndIndex);

    int SectionCalculateByOperator(vector<M_EXPRESSION_ELEMENT *> *outarrElements, int BeginIndex, int EndIndex, char *Operator1, char *Operator2, int *Removed);

    void SeparateLines(vector<string *> *arroutLines, vector<string *> *arroutMarkLines,  string* strAnalyzeText, string* strMark, char * chLineSeparators, char chEmpty);

    int SetVariableValue(string* ValueName, M_EXPRESSION_ELEMENT* pValue);

    int MarkCode(string *strAnalyzeText, string *strMark);

    int EscapeSequention(string* strAnalyzeText, string* strMark, char MarkString, char Escape);

    void TrimRight(string* sStr, char* ArraySep);

    void TrimLeft(string* sStr, char* ArraySep);
};

#endif
