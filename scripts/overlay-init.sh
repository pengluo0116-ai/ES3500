#!/bin/bash
LOG=/root/es3500/init.log
mkdir -p /root/es3500/bin /root/es3500 /root/build
exec > $LOG 2>&1
echo "=== ES3500 Init $(date) ==="

[ -f /root/.es3500_ok ] && { systemctl start es3500 2>/dev/null; exit 0; }

# 1. 网络
echo "Wait network..."
for i in $(seq 1 10); do ping -c1 -W2 114.114.114.114 >/dev/null 2>&1 && break; sleep 10; done
nmcli dev wifi connect easyele password JNZQ7572 2>/dev/null || true

# 2. 安装依赖 - 优先本地 debs，不行再 apt
echo "Install deps..."
if ls /root/debs/*.deb >/dev/null 2>&1; then
  dpkg -i /root/debs/*.deb 2>/dev/null || apt-get install -f -y -qq
  rm -rf /root/debs
  echo "DEBS OK"
else
  apt-get update -qq --allow-releaseinfo-change 2>/dev/null
  apt-get install -y -qq g++ libsqlite3-dev libjsoncpp-dev || { echo "APT FAIL"; exit 1; }
  echo "APT OK"
fi

# 3. 下载源文件
B="https://raw.githubusercontent.com/pengluo0116-ai/ES3500/master"
cd /root/build
DL() { curl -sL --connect-timeout 30 --retry 3 -o "$1" "$2" || { echo "FAIL: $1"; exit 1; }; }

mkdir -p inc/json inc/sqlite
echo "Download source..."
for f in main.cpp http_server.cpp http_server.h db_manager.cpp db_manager.h \
         json_parser.cpp json_parser.h alarm_checker.cpp alarm_checker.h; do
  DL "$f" "$B/collector/src/$f"
done

echo "Download headers..."
for h in json.h reader.h value.h writer.h autolink.h config.h features.h forwards.h; do
  DL "inc/json/$h" "$B/ES1500_ENYI_SERVER/web_cgi/webcgi_set/include/json/$h"
done
DL inc/sqlite/sqlite3.h "$B/ES1500_ENYI_SERVER/web_cgi/webcgi_set/include/sqlite/sqlite3.h"
DL inc/sqlite/sqlite3ext.h "$B/ES1500_ENYI_SERVER/web_cgi/webcgi_set/include/sqlite/sqlite3ext.h"

# 4. 编译
echo "Compile..."
g++ -std=c++11 -O2 -Iinc -Iinc/sqlite -Iinc/json \
  -o /root/es3500/bin/collector \
  main.cpp http_server.cpp db_manager.cpp json_parser.cpp alarm_checker.cpp \
  -lsqlite3 -ljsoncpp -lpthread

[ ! -f /root/es3500/bin/collector ] && { echo "COMPILE FAIL"; exit 1; }
echo "OK: $(ls -lh /root/es3500/bin/collector)"

# 5. 清理+启动
rm -rf /root/build
systemctl enable es3500 2>/dev/null
systemctl start es3500 2>/dev/null
touch /root/.es3500_ok
echo "=== READY ==="
