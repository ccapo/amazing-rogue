CXX = g++
SRCDIR = src
INCDIR = include
#CFLAGS = -O2 -s -w -Wall -std=c++17 -no-pie -I$(INCDIR) `pkg-config libtcod --cflags`
CFLAGS = -O0 -g -w -Wall -std=c++17 -no-pie -I$(INCDIR) `pkg-config libtcod --cflags`
LFLAGS = `pkg-config libtcod --libs` -lSDL2 -lm

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(wildcard $(INCDIR)/*.hpp)

all: amazing-rogue

amazing-rogue: $(OBJECTS) $(HEADERS)
	@$(CXX) $(CFLAGS) $(OBJECTS) -o "$@" $(LFLAGS)
	@rm -fr $(OBJECTS)

clean:
	@rm -fr amazing-rogue
	@rm -fr $(OBJECTS)