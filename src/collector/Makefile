# ES3500 Collector - Makefile
# 目标平台: Nano Pi M1 Plus (ARMv7, Allwinner H3)
# 
# 本地编译 (在 Nano Pi 上):
#   make
#
# 交叉编译 (在 PC 上):
#   make CROSS=arm-linux-gnueabihf-

CROSS  ?= 
CC     = $(CROSS)g++
CFLAGS = -std=c++11 -O2 -Wall
LIBS   = -lsqlite3 -ljsoncpp -lpthread

# JSON 和 SQLite 头文件路径 (交叉编译时需要)
INCLUDES = -I../../ES1500_ENYI_SERVER/web_cgi/webcgi_set/include

SRC  = main.cpp http_server.cpp db_manager.cpp json_parser.cpp alarm_checker.cpp
OBJ  = $(SRC:.cpp=.o)
BIN  = collector

.PHONY: all clean install

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

install:
	@echo "安装到 /root/es3500/"
	mkdir -p /root/es3500
	cp $(BIN) /root/es3500/
	@echo "安装完成"
