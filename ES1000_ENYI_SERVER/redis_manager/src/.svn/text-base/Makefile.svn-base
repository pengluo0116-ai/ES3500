CC=g++

BIN_NAME=credis
BIN_PATH=../bin
BIN=$(BIN_PATH)/$(BIN_NAME)

LIB= -lsqlite3 -ljsoncpp -lhiredis
	
OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/redis_client.o \
$(OBJ_PATH)/redis_cont.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/db_cont.o \
$(OBJ_PATH)/Manager.o \
$(OBJ_PATH)/redis_devupdate.o

$(BIN):$(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/redis_client.o:redis_client.h
	$(CC) -o $(OBJ_PATH)/redis_client.o -c redis_client.cpp

$(OBJ_PATH)/redis_cont.o:redis_cont.h
	$(CC) -o $(OBJ_PATH)/redis_cont.o -c redis_cont.cpp

$(OBJ_PATH)/dbcontrl.o:dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/db_cont.o:db_cont.h
	$(CC) -o $(OBJ_PATH)/db_cont.o -c db_cont.cpp

$(OBJ_PATH)/Manager.o:Manager.h
	$(CC) -o $(OBJ_PATH)/Manager.o -c Manager.cpp

$(OBJ_PATH)/redis_devupdate.o:redis_devupdate.h
	$(CC) -o $(OBJ_PATH)/redis_devupdate.o -c redis_devupdate.cpp

.PHONY:clean
clean:
	-rm -rf $(OBJ) $(BIN)

