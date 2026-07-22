CC=g++

#生成的软件名称
EXE_NAME=gethost_udpserver
EXE_PATH=../debug
OBJ_PATH=../obj

EXE=$(EXE_PATH)/$(EXE_NAME)
LIB=-ljsoncpp -lrt -liconv

OBJ=$(OBJ_PATH)/gbk_utf8.o \
$(OBJ_PATH)/udp_sock_client.o \
$(OBJ_PATH)/udp_sock_server.o \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/server.o \
$(OBJ_PATH)/getNet_wan.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/display.o \
$(OBJ_PATH)/tcp_sock_client.o \
$(OBJ_PATH)/tcp_sock_pakage.o \
$(OBJ_PATH)/crc8.o \
$(OBJ_PATH)/getMac.o \
$(OBJ_PATH)/udp_radio.o \
$(OBJ_PATH)/net_cont.o

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

$(OBJ_PATH)/gbk_utf8.o:gbk_utf8.h
	$(CC) -o $(OBJ_PATH)/gbk_utf8.o -c gbk_utf8.cpp

$(OBJ_PATH)/udp_sock_client.o:udp_sock_client.h
	$(CC) -o $(OBJ_PATH)/udp_sock_client.o -c udp_sock_client.cpp

$(OBJ_PATH)/udp_sock_server.o:udp_sock_server.h
	$(CC) -o $(OBJ_PATH)/udp_sock_server.o -c udp_sock_server.cpp

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/server.o:server.h
	$(CC) -o $(OBJ_PATH)/server.o -c server.cpp

$(OBJ_PATH)/getNet_wan.o:getNet_wan.h
	$(CC) -o $(OBJ_PATH)/getNet_wan.o -c getNet_wan.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/display.o:display.h
	$(CC) -o $(OBJ_PATH)/display.o -c display.cpp

$(OBJ_PATH)/tcp_sock_client.o:tcp_sock_client.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_client.o -c tcp_sock_client.cpp

$(OBJ_PATH)/tcp_sock_pakage.o:tcp_sock_pakage.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_pakage.o -c tcp_sock_pakage.cpp

$(OBJ_PATH)/crc8.o:crc8.h
	$(CC) -o $(OBJ_PATH)/crc8.o -c crc8.cpp

$(OBJ_PATH)/getMac.o:getMac.h
	$(CC) -o $(OBJ_PATH)/getMac.o -c getMac.cpp

$(OBJ_PATH)/udp_radio.o:udp_radio.h
	$(CC) -o $(OBJ_PATH)/udp_radio.o -c udp_radio.cpp

$(OBJ_PATH)/net_cont.o:net_cont.h
	$(CC) -o $(OBJ_PATH)/net_cont.o -c net_cont.cpp

.PHONY:clean
clean:
	-rm -rf $(EXE) $(OBJ)
