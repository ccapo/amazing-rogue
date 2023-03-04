SRCDIR = src
INCDIR = include
CPP = g++

CFLAGS = -O2 -s -w -I$(INCDIR)
#CFLAGS = -O0 -g -Wall -I$(INCDIR)

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CPP) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

HEADERS = $(wildcard $(INCDIR)/*.hpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: combat_simulation

combat_simulation: $(HEADERS) $(OBJECTS)
	@$(CPP) $(CFLAGS) $(OBJECTS) -o $@ -lm
	@rm -f $(OBJECTS)

clean:
	@rm -f $(OBJECTS)
	@rm -f combat_simulation
