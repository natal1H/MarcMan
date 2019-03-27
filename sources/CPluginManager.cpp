#include "CConvertor.h"

/* ---------------------------------------------------------
 * Auxiliary functions 
 * ---------------------------------------------------------
 */

using namespace std;

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/* ---------------------------------------------------------
 * CPluginManager class
 * ---------------------------------------------------------
 */

CPluginManager::CPluginManager() {
    log = &cerr;
    logLevel = 0;
    pluginManagerEnabled = 0;
}

CPluginManager::~CPluginManager() {
    if(plugins.size() != 0) {
        unregisterPlugins();
    }
}

int CPluginManager::isEnabled() {
    return pluginManagerEnabled;
}

string CPluginManager::getErrorMessage(){
    return errorMessage;
}

void CPluginManager::setErrorMessage(string message) {
    errorMessage = message;
}

void CPluginManager::setLog(ostream *logStream, int level) {
    log = logStream;
    logLevel = level;
}

int CPluginManager::registerPlugins(map<string, M_EXPRESSION_ELEMENT *> *configuration) {
    vector<string> pluginLibraries;
    vector<string>::iterator iter;
    string pluginsDirectory = "";

    if(configuration->count(CPM_PLUGINS_DIRECTORY) > 0) {
        pluginsDirectory = *((*configuration)[CPM_PLUGINS_DIRECTORY]->sName);
    }

    if(configuration->count(CPM_PLUGINS) == 0) {
        return 0;
    }

    pluginManagerEnabled = 1;

    tokenize(*((*configuration)[CPM_PLUGINS]->sName),pluginLibraries,",");

    for (iter = pluginLibraries.begin(); iter != pluginLibraries.end(); iter++) {
        registerPlugin(pluginsDirectory + *iter);
    }

    return 0;
}

int CPluginManager::registerPlugin(const string pluginPath) {
    DLIB_HANDLE handle;
    DLIB_HANDLE symbol;
    vector<string>::iterator iter;
    vector<string> functionList;
    string pluginName;

    if (!(handle = DLIB_OPEN(pluginPath.c_str(),RTLD_NOW))) {
        (*log) << "Unable to open plugin '" << pluginPath << "': " << DLIB_ERROR() << endl << flush;
    }
    else {
        if(!(symbol=(DLIB_HANDLE)DLIB_SYM(handle, "create"))) {
            (*log) << "Unable to create plugin: " << DLIB_ERROR() << endl << flush;
        }
        else {
            CMarcManPlugin *plugin = ((CMarcManPlugin*(*)())(symbol))();

            if(plugin) {
                pluginName = plugin->getName();
                functionList = plugin->getFunctionList();

                (*log) << "Plugin name: " << pluginName << endl << flush;

                for(iter = functionList.begin(); iter != functionList.end(); iter++) {
                    (*log) << "Function: " << pluginName + CPM_FUNCTION_DELIMITER + *iter << endl;
                }

                if(plugins[pluginName]) {
                    (*log) << "Plugin '" << pluginName << "' already registered" << endl << flush;
                    (*log) << "Unregistering previously registered plugin" << endl << flush;
                    unregisterPlugin(pluginName);
                }

                plugins[pluginName] = plugin;
                pluginHandles[pluginName] = handle;
            }
            else {
                (*log) << "Unable to instantiate plugin in '" << pluginPath << "'" << endl << flush;
            }
        }
    }

    return 0;
}

int CPluginManager::unregisterPlugins() {
    map<string, CMarcManPlugin *>::iterator iter;

    if(pluginManagerEnabled) {
        for(iter = plugins.begin(); iter != plugins.end(); iter++) {
            unregisterPlugin((*iter).first);
        }
    }

    return 0;
}

int CPluginManager::unregisterPlugin(const string pluginName) {
    DLIB_HANDLE handle;
    DLIB_HANDLE symbol;

    handle = pluginHandles[pluginName];
    if(handle) {
        if(!(symbol = (DLIB_HANDLE)DLIB_SYM(handle, "destroy"))) {
            (*log) << "Unable to destroy plugin '" << pluginName << "': " << DLIB_ERROR() << endl << flush;
            return 1;
        };
        ((int (*)())(symbol))();
        DLIB_CLOSE(handle);

        plugins.erase(pluginName);
        pluginHandles.erase(pluginName);
    }
    else {
        (*log) << "Unable to unregister plugin '" << pluginName << "': plugin not registered" << endl << flush;
        return 1;
    }

    return 0; 
}


int CPluginManager::loadPlugins(map<string, M_EXPRESSION_ELEMENT *> *configuration) {
    map<string, CMarcManPlugin *>::iterator iter;

    if(!pluginManagerEnabled) 
        return 0;

    for(iter = plugins.begin(); iter != plugins.end(); iter++) {
        if((*iter).second->load(configuration) != 0) {
            setErrorMessage("Unable to load plugin '" + (*iter).second->getName() + "'"); 
            return 1;
        }
    }

    return 0;
}

int CPluginManager::unloadPlugins() {
    map<string, CMarcManPlugin *>::iterator iter;

    if(!pluginManagerEnabled) 
        return 0;

    for(iter = plugins.begin(); iter != plugins.end(); iter++) {
        if((*iter).second->unload() != 0) {
            setErrorMessage("Unable to unload plugin '" + (*iter).second->getName() + "'"); 
            return 1;
        }
    }

    return 0;
}

int CPluginManager::callPluginFunction(CMarcAnalyzer *inputRecord, CMarcAnalyzer *outputRecord, const string *functionName, vector<M_EXPRESSION_ELEMENT *> *functionArguments, M_EXPRESSION_ELEMENT *returnValue) {
    vector<string> tokens;
    vector<string>::iterator iter;
    CMarcManPlugin *plugin;

    if(!pluginManagerEnabled)
        return 1;

    tokenize(*functionName,tokens,CPM_FUNCTION_DELIMITER);

    if(tokens.size() != 2) return 1;

    iter = tokens.begin();
    if(( plugin = plugins[*iter]) == 0)
        return 1;
    
    return plugins[*iter]->callFunction(inputRecord, outputRecord, &(*(iter+1)), functionArguments, returnValue);
}

