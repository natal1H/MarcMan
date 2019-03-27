#include <vector>
#include <iostream>
#include <CMarcManPlugin.h>

extern "C" {
   #include "md5.h"
};

/* ---------------------------------------------------------
 * Plugin wrapper
 * ---------------------------------------------------------
 */

class CMarcManPlugin_md5: public CMarcManPlugin {
   private:
      string name;
      vector<string> functionList;
      string computeHash(const string *input);

   public:
      virtual string getName() { return name; }
      virtual vector<string> getFunctionList() { return functionList; }
      virtual int callFunction(CMarcAnalyzer *inputRecord, CMarcAnalyzer *outputRecord, const string *functionName, vector<M_EXPRESSION_ELEMENT *> *functionArguments, M_EXPRESSION_ELEMENT *returnValue );
      CMarcManPlugin_md5();
};

CMarcManPlugin_md5::CMarcManPlugin_md5() {
   name = "md5";
   functionList.push_back("ComputeHash");
}



int CMarcManPlugin_md5::callFunction(CMarcAnalyzer *inputRecord, CMarcAnalyzer *outputRecord, const string *functionName, vector<M_EXPRESSION_ELEMENT *> *functionArguments, M_EXPRESSION_ELEMENT *returnValue ) {

    if(*functionName == "ComputeHash") {
        if(functionArguments->size() == 1 && (*functionArguments)[0]->iType == EX_STRING) {
            returnValue->iType = EX_STRING;
            *(returnValue->sName) = computeHash((*functionArguments)[0]->sName);
            return 0;
        }
        else {
            returnValue->iType = EX_ERROR;
            *(returnValue->sName) = "Invalid arguments for function '"+(*functionName)+"' in plugin '"+name+"'";
            return 1;
        }
     }
    else {
        returnValue->iType = EX_ERROR;
        *(returnValue->sName) = "Function '"+(*functionName)+"' not provided by plugin '"+name+"'";
        return 1;
    }
}

string CMarcManPlugin_md5::computeHash(const string *input) {
   char buf[33];
   MD5_CTX mdContext;
    
   memset(buf,0,33);
   MD5Init(&mdContext);
   MD5Update(&mdContext, (unsigned char *)input->c_str(), input->length());
   MD5Final(&mdContext);

   for (int i = 0; i < 16; i++)
      sprintf (&buf[2*i],"%02x", (&mdContext)->digest[i]);
   
   return buf;
}


/* ---------------------------------------------------------
 * DO NOT EDIT THE LINES BELOW
 * ---------------------------------------------------------
 */

MM_EXPORT_PLUGIN(CMarcManPlugin_md5)
