TARGET_CPPS := Explorer.cpp TestSAIS.cpp TestSearch.cpp RunSAISOn.cpp
CPP_FILES := $(filter-out $(TARGET_CPPS),$(wildcard *.cpp))
OBJ_FILES := $(CPP_FILES:.cpp=.o)

CPP_FLAGS = --std=c++17 -Wall -Werror -Wpedantic -O3 -g

all: test-sais test-search explore run-sais-on

$(OBJ_FILES): Makefile

test-search: $(OBJ_FILES) TestSearch.o
	g++ -o $@ $^

run-sais-on: $(OBJ_FILES) RunSAISOn.o
	g++ -o $@ $^

test-sais: $(OBJ_FILES) TestSAIS.o
	g++ -o $@ $^

explore: $(OBJ_FILES) Explorer.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $(CPP_FLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f *.o test-sais test-search explore run-sais-on *~
