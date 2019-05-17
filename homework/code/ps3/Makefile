TARGET_CPPS := TestOverlap.cpp
CPP_FILES := $(filter-out $(TARGET_CPPS),$(wildcard *.cpp))
OBJ_FILES := $(CPP_FILES:.cpp=.o)

CPP_FLAGS = --std=c++17 -Wall -Werror -Wpedantic -Ofast -g

all: test-overlap

$(OBJ_FILES) TestOverlap.o: Makefile

test-overlap: $(OBJ_FILES) TestOverlap.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $(CPP_FLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f *.o test-overlap *~
