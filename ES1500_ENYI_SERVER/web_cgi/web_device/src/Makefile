CC=g++

EXE_NAME=device.cgi
EXE_PATH=../bin
OBJ_PATH=../obj

EXE=$(EXE_PATH)/$(EXE_NAME)
LIB=-lhiredis -ljsoncpp -lsqlite3
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/base64.o \
$(OBJ_PATH)/ckuser_cookie.o \
$(OBJ_PATH)/contrl.o \
$(OBJ_PATH)/cookie.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/redis_client.o \
$(OBJ_PATH)/redis_devupdate.o \
$(OBJ_PATH)/jsonConfFile.o \
$(OBJ_PATH)/lsystem.o

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/base64.o:base64.h
	$(CC) -o $(OBJ_PATH)/base64.o -c base64.cpp

$(OBJ_PATH)/ckuser_cookie.o:ckuser_cookie.h
	$(CC) -o $(OBJ_PATH)/ckuser_cookie.o -c ckuser_cookie.cpp

$(OBJ_PATH)/contrl.o:contrl.h
	$(CC) -o $(OBJ_PATH)/contrl.o -c contrl.cpp

$(OBJ_PATH)/cookie.o:cookie.h
	$(CC) -o $(OBJ_PATH)/cookie.o -c cookie.cpp

$(OBJ_PATH)/dbcontrl.o:dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/redis_client.o:redis_client.h
	$(CC) -o $(OBJ_PATH)/redis_client.o -c redis_client.cpp

$(OBJ_PATH)/redis_devupdate.o:redis_devupdate.h
	$(CC) -o $(OBJ_PATH)/redis_devupdate.o -c redis_devupdate.cpp

$(OBJ_PATH)/jsonConfFile.o:jsonConfFile.h
	$(CC) -o $(OBJ_PATH)/jsonConfFile.o -c jsonConfFile.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
