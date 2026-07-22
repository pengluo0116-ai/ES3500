#!/bin/bash
# ============================================================
# ES2000 Nano Pi M1 Plus 一键部署脚本
# 
# 用法: 把整个 ES2000-master 文件夹拷到 Nano Pi 的 /root/ 下
#       cd /root/ES2000-master && bash deploy.sh
# ============================================================

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
ES2000_DIR="/root/es2000"

# ==================== 1. 系统依赖 ====================
install_deps() {
    log_info "安装系统依赖..."

    apt-get update -qq

    # 编译工具
    apt-get install -y -qq build-essential g++ make cmake

    # 库
    apt-get install -y -qq libsqlite3-dev libjsoncpp-dev libpthread-stubs0-dev

    # Qt5 (HDMI 显示)
    apt-get install -y -qq qt5-default qtbase5-dev libqt5sql5-sqlite || {
        log_warn "qt5-default 不存在，尝试 qtbase5-dev 替代..."
        apt-get install -y -qq qtbase5-dev libqt5sql5-sqlite
    }

    # Web 服务器 (lighttpd 或 nginx)
    apt-get install -y -qq lighttpd || apt-get install -y -qq nginx

    # MQTT (可选)
    apt-get install -y -qq mosquitto mosquitto-clients libmosquitto-dev libpaho-mqtt-dev

    # Redis (可选)
    apt-get install -y -qq redis-server libhiredis-dev

    # 其他
    apt-get install -y -qq curl wget

    log_info "依赖安装完成"
}

# ==================== 2. 创建目录结构 ====================
setup_dirs() {
    log_info "创建目录..."
    mkdir -p $ES2000_DIR/{bin,logs,data,www,conf}
    mkdir -p $ES2000_DIR/data/map_images
}

# ==================== 3. 编译 collector (HTTP → SQLite) ====================
build_collector() {
    log_info "编译 collector (HTTP 数据接收器)..."
    cd "$ROOT_DIR/collector/src"

    # 使用项目自带的 jsoncpp/sqlite3 头文件
    INC_DIR="$ROOT_DIR/ES1000_ENYI_SERVER/web_cgi/webcgi_set/include"
    LIB_DIR="$ROOT_DIR/ES1000_ENYI_SERVER/web_cgi/webcgi_set/lib"

    g++ -std=c++11 -O2 -Wall \
        -I"$INC_DIR" \
        -o collector \
        main.cpp http_server.cpp db_manager.cpp json_parser.cpp alarm_checker.cpp \
        -lsqlite3 -ljsoncpp -lpthread 2>&1 | tail -3

    if [ -f collector ]; then
        cp collector $ES2000_DIR/bin/
        log_info "collector 编译成功 → $ES2000_DIR/bin/collector"
    else
        log_error "collector 编译失败"
    fi
}

# ==================== 4. 编译 CGI 模块 ====================
build_cgi() {
    log_info "编译 CGI 模块..."

    INC_DIR="$ROOT_DIR/ES1000_ENYI_SERVER/web_cgi/webcgi_set/include"

    # data_api.cgi
    cd "$ROOT_DIR/ES1000_ENYI_SERVER/web_cgi/data_api"
    g++ -std=c++11 -O2 -I"$INC_DIR" \
        -o data_api.cgi data_api.cpp \
        -lsqlite3 -ljsoncpp 2>&1 | tail -2

    if [ -f data_api.cgi ]; then
        cp data_api.cgi $ES2000_DIR/bin/
        log_info "data_api.cgi 编译成功"
    fi

    # set.cgi
    cd "$ROOT_DIR/ES1000_ENYI_SERVER/web_cgi/webcgi_set/src"
    make clean 2>/dev/null || true
    make 2>&1 | tail -2

    if [ -f ../bin/set.cgi ]; then
        cp ../bin/set.cgi $ES2000_DIR/bin/
        log_info "set.cgi 编译成功"
    fi

    # 复制 .so
    cp "$LIB_DIR"/*.so /usr/lib/ 2>/dev/null || true
    ldconfig 2>/dev/null || true
}

# ==================== 5. 编译 Qt 显示程序 ====================
build_qt() {
    log_info "编译 Qt 显示程序..."
    cd "$ROOT_DIR/qt_display/src"

    if command -v qmake &>/dev/null; then
        qmake ElevatorDisplay.pro 2>&1 | tail -2
        make clean 2>/dev/null || true
        make -j2 2>&1 | tail -5

        if [ -f elevator_display ]; then
            cp elevator_display $ES2000_DIR/bin/
            log_info "elevator_display 编译成功"
        else
            log_warn "Qt 编译失败，检查 Qt5 是否安装"
        fi
    else
        log_warn "qmake 未找到，跳过 Qt 编译"
    fi
}

# ==================== 6. 安装 systemd 服务 ====================
install_services() {
    log_info "安装 systemd 服务..."

    # collector 服务
    cat > /etc/systemd/system/es2000-collector.service << 'EOF'
[Unit]
Description=ES2000 Data Collector (HTTP Server)
After=network.target

[Service]
Type=simple
ExecStart=/root/es2000/bin/collector
WorkingDirectory=/root/es2000
Restart=always
RestartSec=5
StandardOutput=append:/root/es2000/logs/collector.log
StandardError=append:/root/es2000/logs/collector.log

[Install]
WantedBy=multi-user.target
EOF

    # Qt display 服务
    cat > /etc/systemd/system/es2000-display.service << 'EOF'
[Unit]
Description=ES2000 Elevator Display
After=es2000-collector.service

[Service]
Type=simple
ExecStart=/root/es2000/bin/elevator_display
Environment=DISPLAY=:0
Environment=QT_QPA_PLATFORM=linuxfb
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

    systemctl daemon-reload
    systemctl enable es2000-collector 2>/dev/null || true
    systemctl enable es2000-display 2>/dev/null || true

    log_info "服务安装完成"
}

# ==================== 7. 配置 Web 服务器 ====================
setup_web() {
    log_info "配置 Web 服务器..."

    # lighttpd
    if command -v lighttpd &>/dev/null; then
        cat > /etc/lighttpd/lighttpd.conf << 'EOF'
server.modules = ("mod_cgi", "mod_accesslog")
server.document-root = "/root/es2000/www"
server.port = 80

cgi.assign = ( ".cgi" => "" )
accesslog.filename = "/root/es2000/logs/lighttpd.log"

mimetype.assign = (
  ".html" => "text/html",
  ".css"  => "text/css",
  ".js"   => "application/javascript",
  ".json" => "application/json",
  ".png"  => "image/png",
  ".jpg"  => "image/jpeg"
)
EOF
        systemctl restart lighttpd 2>/dev/null || true
    fi
}

# ==================== 8. 创建启动脚本 ====================
create_launcher() {
    cat > $ES2000_DIR/start.sh << 'EOF'
#!/bin/bash
echo "=== ES2000 启动 ==="
systemctl start es2000-collector
sleep 2
systemctl start es2000-display
echo "collector: $(systemctl is-active es2000-collector)"
echo "display:  $(systemctl is-active es2000-display)"
echo "数据接口: http://$(hostname -I | awk '{print $1}'):9009/"
echo "Web后台:  http://$(hostname -I | awk '{print $1}')/cgi-bin/data_api.cgi?cmd=list"
EOF
    chmod +x $ES2000_DIR/start.sh
}

# ==================== 主流程 ====================
main() {
    echo ""
    echo "  ╔══════════════════════════════════╗"
    echo "  ║  ES2000 Nano Pi M1 Plus 部署    ║"
    echo "  ╚══════════════════════════════════╝"
    echo ""
    echo "  架构: 7628+STM32 →(HTTP POST)→ collector → SQLite → Qt显示"
    echo ""

    # 检查是否为 root
    if [ "$(id -u)" != "0" ]; then
        log_error "请用 root 运行: sudo bash deploy.sh"
        exit 1
    fi

    install_deps
    setup_dirs
    build_collector
    build_cgi
    build_qt
    install_services
    setup_web
    create_launcher

    echo ""
    echo "  ╔══════════════════════════════════╗"
    echo "  ║        部署完成!                 ║"
    echo "  ╚══════════════════════════════════╝"
    echo ""
    echo "  启动: sudo /root/es2000/start.sh"
    echo "  状态: systemctl status es2000-collector"
    echo ""
    echo "  网关数据上报地址:"
    echo "    POST http://192.168.1.120:9009/"
    echo "    Content-Type: application/json"
    echo "    Body: ES1500 协议 JSON"
    echo ""
}

main "$@"
