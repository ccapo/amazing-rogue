CXX = g++
SRCDIR = src
INCDIR = include
CFLAGS = -O2 -s -w -std=c++11 -no-pie -I$(INCDIR)
#CFLAGS = -O0 -g -std=c++11 -I$(INCDIR)
LFLAGS = -lm

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(wildcard $(INCDIR)/*.hpp)

all: map-gen

map-gen: $(OBJECTS) $(HEADERS)
	@$(CXX) $(CFLAGS) $(OBJECTS) -o "$@" $(LFLAGS)

clean:
	@rm -f $(OBJECTS)
	@rm -f map-gen
