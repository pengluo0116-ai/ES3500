#CC=mipsel-openwrt-linux-g++
CC=g++

EXE_NAME=collect
EXE_PATH=../bin
EXE=$(EXE_PATH)/$(EXE_NAME)

OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/crc8.o \
$(OBJ_PATH)/gbk_utf8.o \
$(OBJ_PATH)/jsonConfFile.o \
$(OBJ_PATH)/shm_cont.o \
$(OBJ_PATH)/tcp_sock_client.o \
$(OBJ_PATH)/tcp_sock_pakage.o \
$(OBJ_PATH)/lift_vector.o \
$(OBJ_PATH)/post_ports.o \
$(OBJ_PATH)/post_manager.o \
$(OBJ_PATH)/post_http.o \
$(OBJ_PATH)/chttpclient.o \
$(OBJ_PATH)/post_tcpshort.o \
$(OBJ_PATH)/post_tcplong.o \
$(OBJ_PATH)/mq_collecter.o



LIB= -ljsoncpp -liconv -lrt  -lpthread -lcurl -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/crc8.o:crc8.h
	$(CC) -o $(OBJ_PATH)/crc8.o -c crc8.cpp

$(OBJ_PATH)/gbk_utf8.o:gbk_utf8.h
	$(CC) -o $(OBJ_PATH)/gbk_utf8.o -c gbk_utf8.cpp

$(OBJ_PATH)/jsonConfFile.o:jsonConfFile.h
	$(CC) -o $(OBJ_PATH)/jsonConfFile.o -c jsonConfFile.cpp

$(OBJ_PATH)/shm_cont.o:shm_cont.h
	$(CC) -o $(OBJ_PATH)/shm_cont.o -c shm_cont.cpp

$(OBJ_PATH)/tcp_sock_client.o:tcp_sock_client.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_client.o -c tcp_sock_client.cpp

$(OBJ_PATH)/tcp_sock_pakage.o:tcp_sock_pakage.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_pakage.o -c tcp_sock_pakage.cpp

$(OBJ_PATH)/lift_vector.o:lift_vector.h
	$(CC) -o $(OBJ_PATH)/lift_vector.o -c lift_vector.cpp

$(OBJ_PATH)/post_ports.o:post_ports.h
	$(CC) -o $(OBJ_PATH)/post_ports.o -c post_ports.cpp

$(OBJ_PATH)/post_manager.o:post_manager.h
	$(CC) -o $(OBJ_PATH)/post_manager.o -c post_manager.cpp

$(OBJ_PATH)/post_http.o:post_http.h
	$(CC) -o $(OBJ_PATH)/post_http.o -c post_http.cpp

$(OBJ_PATH)/chttpclient.o:chttpclient.h
	$(CC) -o $(OBJ_PATH)/chttpclient.o -c chttpclient.cpp

$(OBJ_PATH)/post_tcpshort.o:post_tcpshort.h
	$(CC) -o $(OBJ_PATH)/post_tcpshort.o -c post_tcpshort.cpp

$(OBJ_PATH)/post_tcplong.o:post_tcplong.h
	$(CC) -o $(OBJ_PATH)/post_tcplong.o -c post_tcplong.cpp

$(OBJ_PATH)/mq_collecter.o:mq_collecter.h
	$(CC) -o $(OBJ_PATH)/mq_collecter.o -c mq_collecter.cpp

.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
