#!/bin/bash
# ============================================================
# ES3500 一键构建脚本
# 
# 在 Nano Pi M1 Plus 上直接运行此脚本编译所有程序
# 前置: sudo apt install build-essential g++ libsqlite3-dev libjsoncpp-dev qt5-default qtbase5-dev libqt5sql5-sqlite
# ============================================================

set -e
cd "$(dirname "$0")/.."

echo "=== ES3500 全量构建 ==="

# 1. Collector
echo ""
echo "[1/3] 编译采集管理进程..."
cd collector/src
make clean 2>/dev/null || true
make
cd ../..

# 2. Data API
echo ""
echo "[2/3] 编译数据 API..."
mkdir -p ES1500_ENYI_SERVER/web_cgi/data_api/bin
cd ES1500_ENYI_SERVER/web_cgi/data_api
g++ -std=c++11 -O2 -o data_api.cgi data_api.cpp \
    -I../../webcgi_set/include \
    -lsqlite3 -ljsoncpp
cd ../../..

# 3. Qt Display
echo ""
echo "[3/3] 编译 Qt 显示程序..."
cd qt_display/src
qmake ElevatorDisplay.pro
make clean 2>/dev/null || true
make
cd ../..

# 安装
echo ""
echo "=== 安装到 /root/es3500/ ==="
sudo mkdir -p /root/es3500/bin
sudo cp collector/src/collector /root/es3500/bin/
sudo cp ES1500_ENYI_SERVER/web_cgi/data_api/data_api.cgi /root/es3500/bin/
sudo cp qt_display/src/elevator_display /root/es3500/bin/

# 启动脚本
sudo cp firmware/overlay/root/es3500/start.sh /root/es3500/
sudo chmod +x /root/es3500/start.sh
sudo chmod +x /root/es3500/bin/*

# 服务
sudo cp firmware/overlay/etc/systemd/system/es3500-*.service /etc/systemd/system/
sudo systemctl daemon-reload

echo ""
echo "=== 构建完成 ==="
echo "运行: sudo /root/es3500/start.sh"
