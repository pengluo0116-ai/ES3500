#CC=mipsel-openwrt-linux-g++
CC=g++

EXE_NAME=baidu_tts_client
EXE_PATH=../bin
EXE=$(EXE_PATH)/$(EXE_NAME)

OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/c_md5.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/mq_collecter.o \
$(OBJ_PATH)/mq_vector.o \
$(OBJ_PATH)/radio_download.o \
$(OBJ_PATH)/str_replace.o \
$(OBJ_PATH)/chttpclient.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/radio_update.o



LIB= -ljsoncpp -lsqlite3 -liconv -lrt  -lpthread -lcurl -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/c_md5.o:c_md5.h
	$(CC) -o $(OBJ_PATH)/c_md5.o -c c_md5.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/mq_collecter.o:mq_collecter.h
	$(CC) -o $(OBJ_PATH)/mq_collecter.o -c mq_collecter.cpp

$(OBJ_PATH)/mq_vector.o:mq_vector.h
	$(CC) -o $(OBJ_PATH)/mq_vector.o -c mq_vector.cpp

$(OBJ_PATH)/radio_download.o:radio_download.h
	$(CC) -o $(OBJ_PATH)/radio_download.o -c radio_download.cpp

$(OBJ_PATH)/str_replace.o:str_replace.h
	$(CC) -o $(OBJ_PATH)/str_replace.o -c str_replace.cpp

$(OBJ_PATH)/chttpclient.o:chttpclient.h
	$(CC) -o $(OBJ_PATH)/chttpclient.o -c chttpclient.cpp

$(OBJ_PATH)/dbcontrl.o:dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/radio_update.o:radio_update.h
	$(CC) -o $(OBJ_PATH)/radio_update.o -c radio_update.cpp


.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
