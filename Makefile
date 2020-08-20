FLAGS 		= -g -Wall -o
LIBS 		= -I/usr/include/python3.5m -lpython3.5m
EXE_NAME 	= device 
CC 			= g++

all: $(EXE_NAME)

$(EXE_NAME): main.cpp nxp_interface_api.h
	@echo =============== Building device ===============
	$(CC) $(FLAGS) $(EXE_NAME) $(LIBS) main.cpp nxp_interface_api.cpp 
clean: 	 
	@echo =============== Cleaning ===============
	$(RM) $(EXE_NAME)