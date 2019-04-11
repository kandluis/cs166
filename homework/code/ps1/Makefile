CXXFLAGS = --std=c++17 -Wall -Werror -Wpedantic -O0 -g
CXX = g++

OBJECTS = FastestRMQ.o FischerHeunRMQ.o HybridRMQ.o PrecomputedRMQ.o SparseTableRMQ.o SegmentTreeRMQ.o
HEADERS = FastestRMQ.h FischerHeunRMQ.h HybridRMQ.h PrecomputedRMQ.h SparseTableRMQ.h SegmentTreeRMQ.h RMQEntry.h Timer.h

default: run-tests

$(OBJECTS): Makefile

run-tests: $(OBJECTS) RunTests.o Makefile
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) RunTests.o

RunTests.o: $(HEADERS) RunTests.cpp Makefile

%.o: $(HEADERS) %.cpp %.h

clean:
	rm -f run-tests *.o *~
