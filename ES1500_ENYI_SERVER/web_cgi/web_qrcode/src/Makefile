CC=g++

BIN_NAME=qrcode.cgi
BIN_PATH=../bin
BIN=$(BIN_PATH)/$(BIN_NAME)

LIB= -lsqlite3 -ljsoncpp -lcurl
	
OBJ_PATH=../obj
OBJ= \
$(OBJ_PATH)/main.o \
$(OBJ_PATH)/base64.o \
$(OBJ_PATH)/ckuser_cookie.o \
$(OBJ_PATH)/cookie.o \
$(OBJ_PATH)/crc8.o \
$(OBJ_PATH)/dbcontrl.o \
$(OBJ_PATH)/err_msg.o \
$(OBJ_PATH)/jsonConfFile.o \
$(OBJ_PATH)/lsystem.o \
$(OBJ_PATH)/task.o \
$(OBJ_PATH)/task_cmd.o \
$(OBJ_PATH)/tcp_sock_client.o \
$(OBJ_PATH)/tcp_sock_pakage.o \
$(OBJ_PATH)/chttpclient.o


$(BIN):$(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LIB)

$(OBJ_PATH)/main.o:main.h
	$(CC) -o $(OBJ_PATH)/main.o -c main.cpp

$(OBJ_PATH)/base64.o:base64.h
	$(CC) -o $(OBJ_PATH)/base64.o -c base64.cpp

$(OBJ_PATH)/ckuser_cookie.o:ckuser_cookie.h
	$(CC) -o $(OBJ_PATH)/ckuser_cookie.o -c ckuser_cookie.cpp

$(OBJ_PATH)/cookie.o:cookie.h
	$(CC) -o $(OBJ_PATH)/cookie.o -c cookie.cpp

$(OBJ_PATH)/crc8.o:crc8.h
	$(CC) -o $(OBJ_PATH)/crc8.o -c crc8.cpp

$(OBJ_PATH)/dbcontrl.o:dbcontrl.h
	$(CC) -o $(OBJ_PATH)/dbcontrl.o -c dbcontrl.cpp

$(OBJ_PATH)/err_msg.o:err_msg.h
	$(CC) -o $(OBJ_PATH)/err_msg.o -c err_msg.cpp

$(OBJ_PATH)/jsonConfFile.o:jsonConfFile.h
	$(CC) -o $(OBJ_PATH)/jsonConfFile.o -c jsonConfFile.cpp

$(OBJ_PATH)/lsystem.o:lsystem.h
	$(CC) -o $(OBJ_PATH)/lsystem.o -c lsystem.cpp

$(OBJ_PATH)/task.o:task.h
	$(CC) -o $(OBJ_PATH)/task.o -c task.cpp

$(OBJ_PATH)/task_cmd.o:task_cmd.h
	$(CC) -o $(OBJ_PATH)/task_cmd.o -c task_cmd.cpp

$(OBJ_PATH)/tcp_sock_client.o:tcp_sock_client.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_client.o -c tcp_sock_client.cpp

$(OBJ_PATH)/tcp_sock_pakage.o:tcp_sock_pakage.h
	$(CC) -o $(OBJ_PATH)/tcp_sock_pakage.o -c tcp_sock_pakage.cpp

$(OBJ_PATH)/chttpclient.o:chttpclient.h
	$(CC) -o $(OBJ_PATH)/chttpclient.o -c chttpclient.cpp

.PHONY:clean
clean:
	-rm -rf $(OBJ) $(BIN)