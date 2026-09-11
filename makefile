CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
LDFLAGS = -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

SRCS = main.cpp $(wildcard include/*.cpp)
TARGET = main.exe

all:
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)