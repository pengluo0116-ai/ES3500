# ES2000 电梯安全报警主机

## 项目简介

ES2000 是恩易科技（ENYI）的电梯安全报警主机系统，运行在 **Nano Pi M1 Plus** 上。

### 硬件架构

```
电梯传感器 → 必联7628 + STM32/APM32 → (HTTP POST JSON) → Nano Pi M1 Plus
```

### 软件组成

| 模块 | 说明 | 技术 |
|------|------|------|
| **collector** | HTTP Server，接收网关数据，解析 JSON，存储 SQLite，报警判断 | C++ + SQLite + jsoncpp |
| **elevator_display** | HDMI 大屏深色仪表盘，多电梯卡片实时监控 | Qt5 + QSqlDatabase |
| **data_api.cgi** | Web 数据 API（JSON 接口） | C++ CGI |
| **webcgi_set** | Web 配置管理后台 | C++ CGI |

### 数据协议

- **ES1500 电梯数据接口 V2.2A** — JSON 格式
- 包含: Count（累计）、Alarm（报警）、Parameters（设备信息）、State（实时状态）
- 错误代码附录: 6大类故障（主机/平层/开关门/温湿度/姿态/电流/极限）

## 快速开始

### 在 Nano Pi M1 Plus 上编译

```bash
# 1. 安装依赖
sudo apt update
sudo apt install -y build-essential g++ libsqlite3-dev libjsoncpp-dev \
    qt5-default qtbase5-dev libqt5sql5-sqlite lighttpd

# 2. 编译
cd build
bash build_all.sh

# 3. 启动
sudo /root/es2000/start.sh
```

### 生成 SD 卡固件

```bash
cd firmware

# 自动下载 Armbian + 打包
bash build_firmware.sh

# 或指定已有镜像
bash build_firmware.sh /path/to/Armbian_xxx.img
```

### 烧录

```bash
sudo dd if=ES2000_firmware_YYYYMMDD.img of=/dev/sdX bs=4M status=progress
```

## HTTP API

网关向 `http://192.168.1.120:9009/` POST JSON 数据：

```json
{
  "Count": { "EbikeInCnt": 0, "RunNum": 1, ... },
  "Alarm": { "HasAlarm": "0", "Ebike": "0", ... },
  "Parameters": { "Id": "6001", "Time": "2024-...", "Version": "ES1000_V2.0" },
  "State": { "Floor": "1", "Door": "0", "Temp": "25.0", ... }
}
```

Web 数据接口: `http://192.168.1.120/cgi-bin/data_api.cgi?cmd=list`

## 目录结构

```
ES2000-master/
├── collector/src/          # 采集管理进程
├── qt_display/src/         # Qt 仪表盘
├── ES1000_ENYI_SERVER/     # CGI Web 后台
│   └── web_cgi/
│       ├── webcgi_set/     # 配置管理
│       ├── web_upload/     # 地图上传
│       └── data_api/       # 数据 API
├── firmware/               # 固件打包
│   ├── build_firmware.sh
│   └── overlay/            # 覆盖文件
└── build/                  # 构建脚本
```
