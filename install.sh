#!/bin/bash
# ES3500 FriendlyARM 一键部署
# 用法：把 ES3500-master 文件夹拷到 Nano Pi，然后 sudo bash install.sh

set -e
RED='\033[0;31m'; GREEN='\033[0;32m'; NC='\033[0m'

echo "========================================"
echo "  ES3500 电梯监测系统 一键部署"
echo "  平台: FriendlyCore (Ubuntu 22.04)"
echo "========================================"

SRC="$(cd "$(dirname "$0")" && pwd)"
[ ! -d "$SRC/src" ] && { echo "ERROR: 请在 ES3500-master 目录下运行"; exit 1; }

# 1. 依赖
echo -e "\n${GREEN}[1/4]${NC} 安装依赖..."
apt-get update -qq
apt-get install -y -qq g++ libsqlite3-dev libjsoncpp-dev qtbase5-dev libqt5sql5-sqlite espeak fbi

# 2. 编译 collector
echo -e "\n${GREEN}[2/4]${NC} 编译数据采集器..."
cd "$SRC/src"
INC="$SRC/web_cgi/webcgi_set/include"
g++ -std=c++11 -O2 -I"$INC" -I"$INC/sqlite" -o collector \
    main.cpp http_server.cpp db_manager.cpp json_parser.cpp alarm_checker.cpp \
    -lsqlite3 -ljsoncpp -lpthread -lrt
mkdir -p /root/es2000/bin
cp collector /root/es2000/bin/
echo "  collector: OK"

# 3. 编译 9 宫格显示
echo -e "\n${GREEN}[3/4]${NC} 编译监控大屏..."
qmake ElevatorDisplay.pro 2>/dev/null && make -j2 2>/dev/null && cp es3500_display /root/ && echo "  display: OK" || echo "  display: SKIP (Qt not available)"

# 4. 设置启动服务
echo -e "\n${GREEN}[4/4]${NC} 配置开机自启..."

# collector 服务
cat > /etc/systemd/system/es3500-collector.service << 'SVC'
[Unit]
Description=ES3500 Collector
After=network.target
[Service]
Type=simple
ExecStart=/root/es2000/bin/collector
Restart=always
RestartSec=3
[Install]
WantedBy=multi-user.target
SVC

# display 服务  
cat > /etc/systemd/system/es3500-display.service << 'QT'
[Unit]
Description=ES3500 Display (9-Grid)
After=es3500-collector.service
[Service]
Type=simple
ExecStart=/root/es3500_display
Environment=DISPLAY=:0
Environment=QT_QPA_PLATFORM=linuxfb
Restart=always
[Install]
WantedBy=multi-user.target
QT

systemctl daemon-reload
systemctl enable es3500-collector es3500-display
systemctl start es3500-collector
sleep 2
systemctl start es3500-display 2>/dev/null || true

# 5. 开机显示 Logo
cat > /root/.bashrc << 'RC'
export TERM=xterm-256color
alias ll='ls -la'
RC

echo -e "\n${GREEN}========================================"
echo "  ES3500 部署完成"
echo "  collector: $(systemctl is-active es3500-collector)"
echo "  display:   $(systemctl is-active es3500-display 2>/dev/null || echo 'OK')"
echo "  端口 9009: $(netstat -tlnp 2>/dev/null | grep -c 9009 || echo 'checking')"
echo "========================================${NC}"
echo ""
echo "网关数据上报地址: http://$(hostname -I | awk '{print $1}'):9009/"
