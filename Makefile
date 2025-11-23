CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = vi-du-con-tro
SOURCE = vi-du-con-tro.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
