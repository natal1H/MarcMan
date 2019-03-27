#ifndef CMARCMANPLUGIN_H
#define CMARCMANPLUGIN_H

#include <string>
#include <vector>
#include "CMarcAnalyzer.h"
#include "CUseScript.h"

using namespace std;

#ifdef WIN32

#include <windows.h>
#define MM_EXPORT_PLUGIN(plugin) extern "C" { __declspec(dllexport) CMarcManPlugin * __stdcall create() { return new(plugin); } __declspec(dllexport) void __stdcall destroy(CMarcManPlugin *plugin) { delete(plugin); } }

#else

#define MM_EXPORT_PLUGIN(plugin) extern "C" { CMarcManPlugin * create() { return new(plugin); } void destroy(CMarcManPlugin *plugin) { delete(plugin); } }

#endif

class CMarcManPlugin {
    public:
        virtual string getName() = 0;
        virtual vector<string> getFunctionList() = 0;
        virtual int callFunction(CMarcAnalyzer *inputRecord, CMarcAnalyzer *outputRecord, const string *functionName, vector<M_EXPRESSION_ELEMENT *> *functionArguments, M_EXPRESSION_ELEMENT *returnValue) = 0;
        virtual int load(map<string, M_EXPRESSION_ELEMENT *> *configuration) { return 0; };
        virtual int unload() { return 0; };
};

#endif
