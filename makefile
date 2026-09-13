CXX = g++
CXXFLAGS = -std=c++17 -fopenmp -Iinclude
LDFLAGS = -Iinclude -Iinclude/imgui -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

SRCS = main.cpp $(wildcard include/*.cpp) include/imgui/imgui*.cpp include/imgui/rlImGui.cpp
TARGET = main.exe

all:
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)