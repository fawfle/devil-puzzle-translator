# the c++ compiler
CXX = g++

# -Wall                        - gives warnings (enable all warnings)
# -g                           - generate debug info
# -I          -Iglad/include   - adds the glad/include directory to be searched for header files
# -l          -lglfw           - search for the glfw library when linking
# -l          -ldl             - -link the dynamic loader
CXXFLAGS = -Wall -g -pedantic

TARGETS = ./src/puzzle-translator.cpp ./src/pieces.cpp

BUILD_TARGET = ./build/translator

all: $(TARGET)
	$(CXX) $(CXXFLAGS) $(TARGETS) -o $(BUILD_TARGET)

clean:
	$(RM) $(TARGET)
