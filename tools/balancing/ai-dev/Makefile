SRCDIR = src
INCDIR = include
LIBDIR = lib
CPP = g++
UPX = upx -qq

CFLAGS = -O2 -s -w -I$(INCDIR)
#CFLAGS = -O0 -g -Wall -I$(INCDIR)

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CPP) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

HEADERS = $(wildcard $(INCDIR)/tsc/*.hpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: aitest

aitest: $(HEADERS) $(OBJECTS)
	@$(CPP) $(CFLAGS) $(OBJECTS) -o $@
	@rm -f $(OBJECTS)
#	$(UPX) $@

clean:
	@rm -f $(OBJECTS)
	@rm -f aitest
