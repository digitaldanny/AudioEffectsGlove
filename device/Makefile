SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := inc
CC 		:= g++         

EXE 	:= $(BIN_DIR)/device
SRC     := $(shell find $(SRC_DIR) -name *.cpp)		# Find all cpp files in the src directory
OBJ 	:= $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) 	# Name obj files based on cpp filenames

# Desktop libraries
CFLAGS   := -Wall -I$(INC_DIR) -I. -I/usr/include/python3.6m  
LIBS     := -lpython3.6m   

# RPi libraries
#CFLAGS   := -Wall -I$(INC_DIR) -I. -I/usr/include/python3.5m  
#LIBS     := -lpython3.5m

# "all" and "clean" are not targets to build.
.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $^ -o $@

# Create the object files. Creation of the bin directory (BIN_DIR) is done with an
# order-only-prerequisite so that the directory will be created if it does not exist.
#
# Documentation on order-only-prereqs: https://www.gnu.org/software/make/manual/make.html#Prerequisite-Types
# "The rule to create the objdir directory will be run, if needed, before any ‘.o’ is built, 
# but no ‘.o’ will be built because the objdir directory timestamp changed."
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $^ $(LIBS) -o $@

$(BIN_DIR):
	mkdir -p $@

# Create object files from the source code. Again, object directory might not exist, so use
# order-only-prereq to create it.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)