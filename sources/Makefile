OPTFLAGS = -O3
WARNFLAGS = -Wall -Wno-sign-compare
DEPFLAGS = -MM
PLATFORM = $(shell uname)
ifeq ($(PLATFORM),OSF1)
EXTRA_LDFLAGS = 
else
EXTRA_LDFLAGS = -ldl
endif

SOURCE = CConvertor.cpp CMarcAnalyzer.cpp CMarcFileReader.cpp \
    CRulesAnalyzer.cpp CUseScript.cpp CPluginManager.cpp
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCE:.cpp=.o)
PLUGINS = $(wildcard plugins/*)

STANDALONE = AppMain.cpp
DAEMON = DaemonMain.cpp
CLIENT = marcc.c

all: marcman marcmand marcc $(PLUGINS)

clean: $(PLUGINS)
	-rm -f *.o marcman marcmand marcc marcman.exe Makefile.dep

dep: Makefile.dep $(PLUGINS)

-include Makefile.dep

marcman: $(OBJECTS) $(STANDALONE:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^ $(EXTRA_LDFLAGS)

marcmand: $(OBJECTS) $(DAEMON:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^ $(EXTRA_LDFLAGS)

marcc: $(CLIENT:.c=.o)
	$(CC) $(EXTRA_LDFLAGS) $(LDFLAGS) -o $@ $^

Makefile.dep: $(SOURCE) $(STANDALONE) $(DAEMON) $(CLIENT) $(HEADERS)
	$(CXX) $(DEPFLAGS) $(SOURCE) $(STANDALONE) $(DAEMON) > Makefile.dep
	$(CC) $(DEPFLAGS) $(CLIENT) >> Makefile.dep

$(PLUGINS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(WARNFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) $(WARNFLAGS) -o $@ -c $<

.PHONY: all clean dep $(PLUGINS) 
