#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "CUseScript.h"
#include "CMarcManPlugin.h"
#include "CMarcAnalyzer.h"
#include "dlwrapper.h"

#define CPM_PLUGINS_DIRECTORY    "PluginsDirectory"
#define CPM_PLUGINS              "Plugins"
#define CPM_FUNCTION_DELIMITER   "."

class CPluginManager {
   public:
      CPluginManager();
      ~CPluginManager();
      string getErrorMessage();
      void setLog(ostream *log, int level);
      int registerPlugins(map<string, M_EXPRESSION_ELEMENT *> *configuration);
      int unregisterPlugins();
      int loadPlugins(map<string, M_EXPRESSION_ELEMENT *> *configuration);
      int unloadPlugins();
      int callPluginFunction(CMarcAnalyzer *inputRecord, CMarcAnalyzer *outputRecord, const string *name,vector<M_EXPRESSION_ELEMENT *> *args, M_EXPRESSION_ELEMENT *retVal);
      int isEnabled();
   private:
      ostream *log;
      int logLevel;
      int pluginManagerEnabled;
      map <string, DLIB_HANDLE> pluginHandles;
      map <string, CMarcManPlugin *> plugins;
      string errorMessage;
      
      void setErrorMessage(string message);
      int registerPlugin(const string pluginPath);
      int unregisterPlugin(const string pluginame);
};

#endif
