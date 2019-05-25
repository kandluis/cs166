TARGET_CPPS := Explore.cpp RunTests.cpp
CPP_FILES := $(filter-out $(TARGET_CPPS),$(wildcard *.cpp))
OBJ_FILES := $(CPP_FILES:.cpp=.o)

CPP_FLAGS = --std=c++17 -Wall -Werror -Wpedantic -Ofast -march=native -g

all: explore run-tests

$(OBJ_FILES) $(TARGET_CPPS:.cpp=.o): Makefile

explore: $(OBJ_FILES) Explore.o
	g++ -o $@ $^

run-tests: $(OBJ_FILES) RunTests.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $(CPP_FLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f *.o explore run-tests *~
