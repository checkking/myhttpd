CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFALGS+=-O2

HEADERS=$(wildcard *.h)
TESTS= thread_test

all: $(TESTS)

$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cpp,$^) $(CXXFLAGS) -lpthread

thread_test: test/thread_test.cpp thread.cpp

clean:
	rm -f $(TESTS)
