FLAGS 		= -g -Wall -o
LIBS 		= -I/usr/include/python3.5m -lpython3.5m
EXE_NAME 	= device 
CC 			= g++

all: $(EXE_NAME)

$(EXE_NAME): nxp_interface_api.cpp
	@echo =============== Building device ===============
	$(CC) $(FLAGS) $(EXE_NAME) $(LIBS) nxp_interface_api.cpp 
clean: 	 
	@echo =============== Cleaning ===============
	$(RM) $(EXE_NAME)