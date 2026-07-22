#CC=mipsel-openwrt-linux-g++
#CC=g++

CC=arm-linux-gnueabi-g++

EXE_NAME=supdate
EXE_PATH=../bin
EXE=$(EXE_PATH)/$(EXE_NAME)

OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/Task.o \
$(OBJ_PATH)/Update.o


LIB= -lpthread

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/Task.o:Task.h
	$(CC) -o $(OBJ_PATH)/Task.o -c Task.cpp

$(OBJ_PATH)/Update.o:Update.h
	$(CC) -o $(OBJ_PATH)/Update.o -c Update.cpp

.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
