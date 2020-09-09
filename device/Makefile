SRC_DIR := src unit_tests
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := inc
CXX 	:= g++         

EXE 	:= $(BIN_DIR)/device
SRC     := $(shell find $(SRC_DIR) -name *.cpp)		# Find all cpp files in the src directory
OBJ 	:= $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) 	# Name obj files based on cpp filenames

CFLAGS = -Wall
INCLUDES = -I$(INC_DIR) -I. -I/usr/include/python3.6m 
LIBS = -lpython3.6m
# ifeq ($(OS), Windows_NT)
# 	# Desktop libraries
# 	@echo "Building for Windows Desktop"
# 	CFLAGS = -Wall -I$(INC_DIR) -I. -I/usr/include/python3.6m 
# 	LIBS = -lpython3.6m
# else
# 	# RPi libraries
# 	@echo "Building for RPi"
# 	CFLAGS = -Wall -I$(INC_DIR) -I. -I/usr/include/python3.5m
# 	LIBS = -lpython3.5m
# endif

# "all" and "clean" are not targets to build.
.PHONY: all clean

all: $(EXE)

# Create the object files. Creation of the bin directory (BIN_DIR) is done with an
# order-only-prerequisite so that the directory will be created if it does not exist.
#
# Documentation on order-only-prereqs: https://www.gnu.org/software/make/manual/make.html#Prerequisite-Types
# "The rule to create the objdir directory will be run, if needed, before any ‘.o’ is built, 
# but no ‘.o’ will be built because the objdir directory timestamp changed."
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ $(LIBS) -o $@ $(INCLUDES)

# Create object files from the source code. Again, object directory might not exist, so use
# order-only-prereq to create it.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) 
	$(CXX) $(CFLAGS) -c $< -o $@

# This target is called for either the OBJ_DIR or BIN_DIR creation in above steps.
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)