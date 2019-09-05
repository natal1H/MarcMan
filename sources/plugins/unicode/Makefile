LDFLAGS  = -shared
CXXFLAGS = -I../../ -fPIC
DEPFLAGS = -MM
OPTFLAGS = -O3

CSOURCE = $(wildcard *.c)
CXXSOURCE = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.cpp,%.o,$(wildcard *.cpp))
TARGET = $(patsubst %.cpp,%.so,$(wildcard plugin_*.cpp))

all: $(TARGET)
	
clean:
		-rm -f *.so *.o Makefile.dep *.a *.def *.dll so_locations
		
dep: Makefile.dep

-include Makefile.dep

$(TARGET): $(OBJECTS)
		$(CXX) $(LDFLAGS) -o $@ $(OBJECTS)

Makefile.dep: $(SOURCE) $(HEADERS)
		echo -n >Makefile.dep
ifneq ($(CXXSOURCE),)
		$(CXX) $(OPTFLAGS) $(DEPFLAGS) $(CXXFLAGS) $(CXXSOURCE) >> Makefile.dep
endif
ifneq ($(CSOURCE),)
		$(CC) $(OPTFLAGS) $(DEPFLAGS) $(CFLAGS) $(CSOURCE) >> Makefile.dep
endif
				
.PHONY: all clean dep
