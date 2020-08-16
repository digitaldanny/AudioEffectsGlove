FLAGS 		= -g -Wall -o
EXE_NAME 	= device 
CC 			= g++

all: $(EXE_NAME)

$(EXE_NAME): nxp_wrapper.cpp
	@echo =============== Building device ===============
	$(CC) $(FLAGS) $(EXE_NAME) nxp_wrapper.cpp
clean: 	 
	@echo =============== Cleaning ===============
	$(RM) $(EXE_NAME)