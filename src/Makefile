CXX = g++
CXXFLAGS = -std=c++11 -Wall -march=native -O3

SRCDIR = solutions
BINDIR = bin
TESTDIR = tests

SOURCES_SOLUTIONS = $(wildcard $(SRCDIR)/*.cpp)
SOURCES = $(SOURCES_SOLUTIONS) generator.cpp

EXECUTABLES_SOLUTIONS = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(SOURCES_SOLUTIONS))
EXECUTABLES = $(EXECUTABLES_SOLUTIONS) $(BINDIR)/generator

OMP_SOURCES = $(wildcard $(SRCDIR)/*omp.cpp)
OMP_EXECUTABLES = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(OMP_SOURCES))

$(OMP_EXECUTABLES): CXXFLAGS += -fopenmp


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
	# rm -rf $(TESTDIR)
