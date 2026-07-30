#!/bin/bash
# ============================================================
# ES3500 固件打包脚本
# 
# 功能: 基于 Armbian 底座，叠加应用层，生成 SD 卡 .img 镜像
# 
# 用法:
#   ./build_firmware.sh [armbian_base.img]
# 
# 前置条件:
#   - Linux 系统 (或 WSL)
#   - 已安装: qemu-user-static, kpartx, rsync
#   - 交叉编译产物已放入 firmware/overlay/
# ============================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$SCRIPT_DIR/.."
OVERLAY_DIR="$SCRIPT_DIR/overlay"
WORK_DIR="$SCRIPT_DIR/work"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Armbian 镜像 (Nano Pi M1 Plus)
# 下载地址: https://www.armbian.com/nanopi-m1-plus/
ARMBIAN_URL="https://fi.mirror.armbian.de/archive/nanopim1plus/archive/Armbian_23.02.2_Nanopim1plus_bullseye_current_5.15.93.img.xz"
ARMBIAN_IMG=""

# ==================== 0. 参数解析 ====================

if [ -n "$1" ] && [ -f "$1" ]; then
    ARMBIAN_IMG="$1"
    echo "[INFO] 使用本地镜像: $ARMBIAN_IMG"
else
    ARMBIAN_IMG="$WORK_DIR/$(basename ${ARMBIAN_URL%.xz})"
fi

# ==================== 1. 下载/准备 Armbian 底座 ====================

prepare_base() {
    echo "========================================="
    echo "  Step 1: 准备 Armbian 基础镜像"
    echo "========================================="

    mkdir -p "$WORK_DIR" "$OUTPUT_DIR"

    if [ -f "$ARMBIAN_IMG" ]; then
        echo "[OK] 镜像已存在: $ARMBIAN_IMG"
        return
    fi

    echo "[INFO] 下载 Armbian 镜像..."

    # 优先使用本地缓存
    XZ_FILE="$WORK_DIR/$(basename $ARMBIAN_URL)"
    if [ ! -f "$XZ_FILE" ]; then
        wget -O "$XZ_FILE" "$ARMBIAN_URL" || {
            echo "[ERROR] 下载失败，请手动下载 Armbian for Nano Pi M1 Plus"
            echo "  下载地址: https://www.armbian.com/nanopi-m1-plus/"
            echo "  然后运行: $0 <镜像文件路径>"
            exit 1
        }
    fi

    echo "[INFO] 解压镜像..."
    xz -dk "$XZ_FILE" -c > "$ARMBIAN_IMG"
    echo "[OK] 镜像准备完成: $ARMBIAN_IMG"
}

# ==================== 2. 编译应用层 ====================

build_apps() {
    echo "========================================="
    echo "  Step 2: 编译应用层"
    echo "========================================="

    # 采集管理进程
    echo "[BUILD] collector..."
    cd "$ROOT_DIR/collector/src"
    make clean 2>/dev/null || true
    make CROSS=arm-linux-gnueabihf- || {
        echo "[WARN] ARM 交叉编译失败，使用本地 g++ 编译 (仅测试用)"
        make
    }

    # 数据 API CGI
    echo "[BUILD] data_api.cgi..."
    cd "$ROOT_DIR/ES1500_ENYI_SERVER/web_cgi/data_api"
    arm-linux-gnueabihf-g++ -std=c++11 -O2 -o data_api.cgi data_api.cpp \
        -I../../webcgi_set/include -L../../webcgi_set/lib -lsqlite3 -ljsoncpp 2>/dev/null || {
        g++ -std=c++11 -O2 -o data_api.cgi data_api.cpp -lsqlite3 -ljsoncpp
        echo "[WARN] 使用本地编译 (仅测试)"
    }

    # 配置 CGI
    echo "[BUILD] set.cgi..."
    cd "$ROOT_DIR/ES1500_ENYI_SERVER/web_cgi/webcgi_set/src"
    make clean 2>/dev/null || true
    make CROSS=arm-linux-gnueabihf- 2>/dev/null || {
        make
        echo "[WARN] 使用本地编译 (仅测试)"
    }

    # Qt 显示程序
    echo "[BUILD] elevator_display..."
    cd "$ROOT_DIR/qt_display/src"
    if command -v arm-linux-gnueabihf-qmake &> /dev/null; then
        arm-linux-gnueabihf-qmake ElevatorDisplay.pro
        make clean 2>/dev/null || true
        make
    else
        echo "[WARN] 未找到 ARM Qt 工具链，Qt 程序需在 Nano Pi 上编译"
        echo "  在 Nano Pi 上执行:"
        echo "  sudo apt install qt5-default qtbase5-dev libqt5sql5-sqlite"
        echo "  cd /root/es3500/qt_display/src && qmake && make"
    fi

    echo "[OK] 编译完成"
}

# ==================== 3. 准备 overlay 文件 ====================

prepare_overlay() {
    echo "========================================="
    echo "  Step 3: 准备 overlay 文件"
    echo "========================================="

    mkdir -p "$OVERLAY_DIR/root/es3500"
    mkdir -p "$OVERLAY_DIR/root/es3500/bin"
    mkdir -p "$OVERLAY_DIR/root/es3500/www"
    mkdir -p "$OVERLAY_DIR/etc/systemd/system"
    mkdir -p "$OVERLAY_DIR/etc/lighttpd/conf-enabled"

    # 复制二进制文件
    cp "$ROOT_DIR/collector/src/collector"  "$OVERLAY_DIR/root/es3500/bin/" 2>/dev/null || echo "[WARN] collector 未编译"
    cp "$ROOT_DIR/ES1500_ENYI_SERVER/web_cgi/webcgi_set/src/../bin/set.cgi" "$OVERLAY_DIR/root/es3500/bin/" 2>/dev/null || echo "[WARN] set.cgi 未编译"
    cp "$ROOT_DIR/ES1500_ENYI_SERVER/web_cgi/data_api/data_api.cgi" "$OVERLAY_DIR/root/es3500/bin/" 2>/dev/null || echo "[WARN] data_api.cgi 未编译"

    # Qt 程序
    cp "$ROOT_DIR/qt_display/src/elevator_display" "$OVERLAY_DIR/root/es3500/bin/" 2>/dev/null || echo "[WARN] elevator_display 未编译"

    # 复制 .so 库
    cp -r "$ROOT_DIR/ES1500_ENYI_SERVER/web_cgi/webcgi_set/lib/"* "$OVERLAY_DIR/usr/lib/" 2>/dev/null || true

    echo "[OK] 文件准备完成"
}

# ==================== 4. 创建 systemd 服务 ====================

create_services() {
    echo "========================================="
    echo "  Step 4: 创建启动服务"
    echo "========================================="

    # 采集进程服务
    cat > "$OVERLAY_DIR/etc/systemd/system/es3500-collector.service" << 'SERVICE'
[Unit]
Description=ES3500 Elevator Data Collector
After=network.target

[Service]
Type=simple
ExecStart=/root/es3500/bin/collector
WorkingDirectory=/root/es3500
Restart=always
RestartSec=5
StandardOutput=append:/root/es3500/collector.log
StandardError=append:/root/es3500/collector.log

[Install]
WantedBy=multi-user.target
SERVICE

    # Qt 显示服务
    cat > "$OVERLAY_DIR/etc/systemd/system/es3500-display.service" << 'SERVICE'
[Unit]
Description=ES3500 Elevator Monitor Display
After=es3500-collector.service

[Service]
Type=simple
ExecStart=/root/es3500/bin/elevator_display
Environment=DISPLAY=:0
Environment=QT_QPA_PLATFORM=linuxfb
User=root
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
SERVICE

    # lighttpd 配置
    cat > "$OVERLAY_DIR/etc/lighttpd/conf-enabled/10-es3500.conf" << 'CONF'
# ES3500 CGI 配置
server.modules += ( "mod_cgi" )

$HTTP["url"] =~ "^/cgi-bin/" {
    cgi.assign = ( ".cgi" => "" )
}

server.document-root = "/root/es3500/www"
CONF

    echo "[OK] 服务文件创建完成"
}

# ==================== 5. 启动脚本 ====================

create_startup_script() {
    cat > "$OVERLAY_DIR/root/es3500/start.sh" << 'SCRIPT'
#!/bin/bash
# ES3500 系统初始化脚本

echo "=== ES3500 系统初始化 ==="

# 创建工作目录
mkdir -p /root/es3500/logs
mkdir -p /root/es3500/data

# 检查数据库
if [ ! -f /root/es3500/es3500.db ]; then
    echo "初始化数据库..."
    sqlite3 /root/es3500/es3500.db "VACUUM;"
fi

# 设置权限
chmod +x /root/es3500/bin/*

# 启用服务
systemctl daemon-reload
systemctl enable es3500-collector
systemctl enable es3500-display

# 启动服务
systemctl start es3500-collector
sleep 2
systemctl start es3500-display

echo "=== ES3500 初始化完成 ==="
SCRIPT
    chmod +x "$OVERLAY_DIR/root/es3500/start.sh"
}

# ==================== 6. 打包镜像 ====================

package_image() {
    echo "========================================="
    echo "  Step 6: 打包固件镜像"
    echo "========================================="

    OUTPUT_IMG="$OUTPUT_DIR/ES3500_firmware_$(date +%Y%m%d).img"

    # 复制基础镜像
    cp "$ARMBIAN_IMG" "$OUTPUT_IMG"

    # 挂载镜像
    echo "[INFO] 挂载镜像..."
    LOOP_DEV=$(sudo losetup -f --show "$OUTPUT_IMG")
    
    # 刷新分区表
    sudo partprobe "$LOOP_DEV" 2>/dev/null || true
    sleep 1

    # 挂载 rootfs 分区（通常是第二个分区）
    ROOT_PART="${LOOP_DEV}p2"
    MOUNT_DIR="$WORK_DIR/mnt"

    sudo mkdir -p "$MOUNT_DIR"
    sudo mount "$ROOT_PART" "$MOUNT_DIR"

    echo "[INFO] 同步应用文件到 rootfs..."
    sudo rsync -av "$OVERLAY_DIR/" "$MOUNT_DIR/"

    # 第一次启动时运行初始化脚本
    sudo bash -c "echo '/bin/bash /root/es3500/start.sh' >> $MOUNT_DIR/etc/rc.local"
    sudo chmod +x "$MOUNT_DIR/etc/rc.local"

    # 卸载
    sudo umount "$MOUNT_DIR"
    sudo losetup -d "$LOOP_DEV"

    echo "========================================="
    echo "  固件生成完成!"
    echo "  文件: $OUTPUT_IMG"
    echo "  "
    echo "  烧录方法:"
    echo "  sudo dd if=$OUTPUT_IMG of=/dev/sdX bs=4M status=progress"
    echo "  "
    echo "  烧录后插入 Nano Pi M1 Plus 即可启动"
    echo "========================================="
}

# ==================== 主流程 ====================

main() {
    echo ""
    echo "  ╔══════════════════════════════════╗"
    echo "  ║   ES3500 固件打包工具 v1.0      ║"
    echo "  ╚══════════════════════════════════╝"
    echo ""

    prepare_base
    prepare_overlay
    create_services
    create_startup_script
    # build_apps  # 需要交叉编译工具链，按需启用
    package_image
}

main "$@"
