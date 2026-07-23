#CC=mipsel-openwrt-linux-g++
CC=g++

OBJ_PATH=../obj
BIN_PATH=../bin
EXE=$(BIN_PATH)/manager

LIB= -lpthread -lsqlite3 -lrt  -ljsoncpp -lhiredis

OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/task.o \
$(OBJ_PATH)/task_manager.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/task_udpserver.o \
$(OBJ_PATH)/task_web.o \
$(OBJ_PATH)/redis_client.o \
$(OBJ_PATH)/task_redis.o \
$(OBJ_PATH)/task_qt.o \
$(OBJ_PATH)/task_uwsgi.o \
$(OBJ_PATH)/task_mqttServer.o


$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/task.o:task.h
	$(CC) -o $(OBJ_PATH)/task.o -c task.cpp

$(OBJ_PATH)/task_manager.o:task_manager.h
	$(CC) -o $(OBJ_PATH)/task_manager.o -c task_manager.cpp

$(OBJ_PATH)/dbcontrl.o:dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/task_udpserver.o:task_udpserver.h
	$(CC) -o $(OBJ_PATH)/task_udpserver.o -c task_udpserver.cpp

$(OBJ_PATH)/task_web.o:task_web.h
	$(CC) -o $(OBJ_PATH)/task_web.o -c task_web.cpp

$(OBJ_PATH)/redis_client.o:redis_client.h
	$(CC) -o $(OBJ_PATH)/redis_client.o -c redis_client.cpp

$(OBJ_PATH)/task_redis.o:task_redis.h
	$(CC) -o $(OBJ_PATH)/task_redis.o -c task_redis.cpp

$(OBJ_PATH)/task_qt.o:task_qt.h
	$(CC) -o $(OBJ_PATH)/task_qt.o -c task_qt.cpp

$(OBJ_PATH)/task_uwsgi.o:task_uwsgi.h
	$(CC) -o $(OBJ_PATH)/task_uwsgi.o -c task_uwsgi.cpp

$(OBJ_PATH)/task_mqttServer.o:task_mqttServer.h
	$(CC) -o $(OBJ_PATH)/task_mqttServer.o -c task_mqttServer.cpp

.PHONY:clean
clean:
	-rm -rf $(OBJ) $(EXE)
