CXX := g++
CXXFLAGS := -fdiagnostics-color=always -g -Wall -std=c++20 -lGL -lpthread -ldl -lrt -lX11
CXXFLAGS_SANITIZE := -fsanitize=address,undefined

LIBS := -lraylib 

SRC_DIR := src
SRC :=  $(SRC_DIR)/utilities.cpp $(SRC_DIR)/environment.cpp $(SRC_DIR)/entities.cpp \
		$(SRC_DIR)/plants.cpp $(SRC_DIR)/gui.cpp $(SRC_DIR)/playerState.cpp \
		$(SRC_DIR)/aquarium.cpp $(SRC_DIR)/shelf.cpp $(SRC_DIR)/main.cpp

OUTPUT := tiny-ecosystem

# Default target
all: $(OUTPUT)
.PRECIOUS: $(OUTPUT)

# Link all object files to create the executable
$(OUTPUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(SRC) $(LIBS) $(INCLUDES)

# Clean target to remove the executable
clear:
	rm -f $(OUTPUT)
