CC=g++

EXE_NAME=upali
EXE_PATH=../bin
EXE=$(EXE_PATH)/$(EXE_NAME)

OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/tcp_sock_client.o \
$(OBJ_PATH)/tcp_sock_pakage.o \
$(OBJ_PATH)/jsonConfFile.o \
$(OBJ_PATH)/Event_vector.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/tcp_sock_server_client.o \
$(OBJ_PATH)/crc8.o \
$(OBJ_PATH)/ali_mqtt_client.o \
$(OBJ_PATH)/getNet_wan.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/net_cont.o \
$(OBJ_PATH)/str_replace.o \
$(OBJ_PATH)/stc_cont.o \
$(OBJ_PATH)/usart_read.o \
$(OBJ_PATH)/shm_cont.o \
$(OBJ_PATH)/4Gmodule.o \
$(OBJ_PATH)/getMac.o

#-lgcov -Bstatic -liot_tls 
LIB= -L ../lib -liot_sdk -liot_hal -liot_tls -ljsoncpp -lsqlite3 -lpthread -lrt  

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/tcp_sock_client.o:tcp_sock_client.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_client.o -c tcp_sock_client.cpp

$(OBJ_PATH)/tcp_sock_pakage.o:tcp_sock_pakage.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_pakage.o -c tcp_sock_pakage.cpp

$(OBJ_PATH)/getMac.o:getMac.h
	$(CC) -o $(OBJ_PATH)/getMac.o -c getMac.cpp

$(OBJ_PATH)/jsonConfFile.o:jsonConfFile.h
	$(CC) -o $(OBJ_PATH)/jsonConfFile.o -c jsonConfFile.cpp

$(OBJ_PATH)/Event_vector.o:Event_vector.h
	$(CC) -o $(OBJ_PATH)/Event_vector.o -c Event_vector.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/tcp_sock_server_client.o:tcp_sock_server_client.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_server_client.o -c tcp_sock_server_client.cpp

$(OBJ_PATH)/crc8.o:crc8.h
	$(CC) -o $(OBJ_PATH)/crc8.o -c crc8.cpp

$(OBJ_PATH)/ali_mqtt_client.o: ali_mqtt_client.h
	$(CC) -o $(OBJ_PATH)/ali_mqtt_client.o -c ali_mqtt_client.cpp

$(OBJ_PATH)/getNet_wan.o: getNet_wan.h
	$(CC) -o $(OBJ_PATH)/getNet_wan.o -c getNet_wan.cpp

$(OBJ_PATH)/dbcontrl.o: dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/net_cont.o: net_cont.h
	$(CC) -o $(OBJ_PATH)/net_cont.o -c net_cont.cpp

$(OBJ_PATH)/str_replace.o: str_replace.h
	$(CC) -o $(OBJ_PATH)/str_replace.o -c str_replace.cpp

$(OBJ_PATH)/stc_cont.o: stc_cont.h
	$(CC) -o $(OBJ_PATH)/stc_cont.o -c stc_cont.cpp

$(OBJ_PATH)/usart_read.o: usart_read.h
	$(CC) -o $(OBJ_PATH)/usart_read.o -c usart_read.cpp

$(OBJ_PATH)/shm_cont.o: shm_cont.h
	$(CC) -o $(OBJ_PATH)/shm_cont.o -c shm_cont.cpp

$(OBJ_PATH)/4Gmodule.o:4Gmodule.h
	$(CC) -o $(OBJ_PATH)/4Gmodule.o -c 4Gmodule.cpp

.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
