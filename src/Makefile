CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCDIR = solutions
BINDIR = bin
TESTDIR = tests

SOURCES_SOLUTIONS = $(wildcard $(SRCDIR)/*.cpp)
SOURCES = $(SOURCES_SOLUTIONS) generator.cpp

EXECUTABLES_SOLUTIONS = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(SOURCES_SOLUTIONS))
EXECUTABLES = $(EXECUTABLES_SOLUTIONS) $(BINDIR)/generator

.PHONY: all clean

all: $(EXECUTABLES)

$(BINDIR)/%: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BINDIR)/generator: generator.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)
	rm -rf $(TESTDIR)
