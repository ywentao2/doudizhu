CC  = g++
CXX = g++ -fno-elide-constructors

CFLAGS   = -g -Wall
CXXFLAGS = -g -Wall -std=c++20

SRC     := $(wildcard *.cpp)
OBJ     := $(SRC:.cpp=.o)
TARGET  := cards

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean all
clean:
	rm -f $(OBJ) $(TARGET)