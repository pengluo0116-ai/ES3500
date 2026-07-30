#!/bin/bash
# ES3500 Universal Installer (FriendlyCore + Armbian)
set -e
echo "ES3500 Installer"

# 自动找源码目录
if [ -d ES3500-master ]; then SRC=ES3500-master
elif [ -d ES3500-master/src ]; then SRC=ES3500-master
elif [ -d src ]; then SRC=.
elif [ -d ../src ]; then SRC=..
else
  SRC=$(find . -maxdepth 3 -name "main.cpp" -path "*/src/*" 2>/dev/null | head -1 | xargs dirname)
  [ -z "$SRC" ] && SRC=$(find . -maxdepth 3 -name "install.sh" 2>/dev/null | head -1 | xargs dirname)
fi
echo "Source: $SRC"
cd "$SRC"

# 找 include
INC=""
for d in ES1500_ENYI_SERVER/web_cgi/webcgi_set/include web_cgi/webcgi_set/include collector/include src/include include; do
  [ -f "$d/json/json.h" ] || [ -f "$d/sqlite/sqlite3.h" ] && INC="$d" && break
done
echo "Include: $INC"

# 装依赖
sudo apt update -qq
sudo apt install -y -qq g++ libsqlite3-dev libjsoncpp-dev espeak

# 编译
echo "Compiling..."
SRCDIR=""
for d in src collector/src .; do
  [ -f "$d/main.cpp" ] && { SRCDIR="$d"; break; }
done
cd "$SRCDIR"
g++ -std=c++11 -O2 -I"../$INC" -I"../$INC/sqlite" -I"$INC" -I"$INC/sqlite" \
  -o collector *.cpp -lsqlite3 -ljsoncpp -lpthread 2>&1
sudo mkdir -p /root/es3500/bin
sudo cp collector /root/es3500/bin/
echo "Collector: $(file /root/es3500/bin/collector)"

# 服务
sudo tee /etc/systemd/system/es3500.service << 'SVC' > /dev/null
[Unit]
Description=ES3500 Collector
After=network.target
[Service]
Type=simple
ExecStart=/root/es3500/bin/collector
Restart=always
[Install]
WantedBy=multi-user.target
SVC
sudo systemctl daemon-reload
sudo systemctl enable es3500
sudo systemctl start es3500

# 验证
sleep 2
echo "Port 9009: $(sudo netstat -tlnp | grep -c 9009 || echo 0)"
echo "=== ES3500 READY ==="
