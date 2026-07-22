/**
 * ES2000 电梯运行实时监控系统 - 主窗口实现
 */
#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QFrame>
#include <cmath>

// ==================== 暗色主题样式表 ====================
static const char* DARK_STYLE = R"(
    QMainWindow {
        background-color: #0d1117;
    }
    QLabel {
        color: #c9d1d9;
        background: transparent;
    }
    QScrollArea {
        background: transparent;
        border: none;
    }
    QScrollBar:vertical {
        background: #161b22;
        width: 8px;
    }
    QScrollBar::handle:vertical {
        background: #30363d;
        border-radius: 4px;
    }
)";

// 卡片样式
static const char* CARD_NORMAL = R"(
    background-color: #161b22;
    border: 1px solid #30363d;
    border-radius: 8px;
    padding: 8px;
)";

static const char* CARD_ALARM = R"(
    background-color: #2d1216;
    border: 2px solid #f85149;
    border-radius: 8px;
    padding: 8px;
)";

static const char* CARD_OFFLINE = R"(
    background-color: #161b22;
    border: 1px solid #30363d;
    border-radius: 8px;
    padding: 8px;
)";

// ==================== ElevatorCard ====================

ElevatorCard::ElevatorCard(QWidget* parent) : QFrame(parent) {
    setupUI();
    setFixedSize(320, 420);
    setStyleSheet(CARD_NORMAL);
}

void ElevatorCard::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(12, 10, 12, 10);

    // 设备ID + 报警指示
    auto* topRow = new QHBoxLayout();
    m_deviceId = new QLabel("6001");
    m_deviceId->setStyleSheet("font-size: 18px; font-weight: bold; color: #58a6ff;");
    
    m_alarmIndicator = new QLabel();
    m_alarmIndicator->setFixedSize(12, 12);
    m_alarmIndicator->setStyleSheet("background: #238636; border-radius: 6px;");
    
    topRow->addWidget(m_deviceId);
    topRow->addStretch();
    topRow->addWidget(m_alarmIndicator);
    layout->addLayout(topRow);

    // 分隔线
    auto* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #21262d;");
    layout->addWidget(sep);

    // 楼层 - 大字显示
    m_floorNo = new QLabel("1");
    m_floorNo->setAlignment(Qt::AlignCenter);
    m_floorNo->setStyleSheet("font-size: 56px; font-weight: bold; color: #f0f6fc;");
    layout->addWidget(m_floorNo);

    // 方向
    m_direction = new QLabel("停止");
    m_direction->setAlignment(Qt::AlignCenter);
    m_direction->setStyleSheet("font-size: 14px; color: #8b949e;");
    layout->addWidget(m_direction);

    // 开关门
    m_doorStatus = new QLabel("关门");
    m_doorStatus->setAlignment(Qt::AlignCenter);
    m_doorStatus->setStyleSheet("font-size: 14px; color: #7ee787;");
    layout->addWidget(m_doorStatus);

    // 有人/无人
    m_people = new QLabel("无人");
    m_people->setAlignment(Qt::AlignCenter);
    m_people->setStyleSheet("font-size: 13px; color: #8b949e;");
    layout->addWidget(m_people);

    // 温湿度
    auto* envRow = new QHBoxLayout();
    m_temp = new QLabel("--℃");
    m_temp->setStyleSheet("color: #d2a8ff;");
    m_hum = new QLabel("--%");
    m_hum->setStyleSheet("color: #79c0ff;");
    m_pm = new QLabel("PM --");
    m_pm->setStyleSheet("color: #7ee787;");
    envRow->addWidget(m_temp);
    envRow->addStretch();
    envRow->addWidget(m_hum);
    envRow->addStretch();
    envRow->addWidget(m_pm);
    layout->addLayout(envRow);

    // 速度 + 电池
    auto* infoRow = new QHBoxLayout();
    m_speed = new QLabel("-- m/s");
    m_speed->setStyleSheet("color: #8b949e;");
    m_battery = new QLabel("-- V");
    m_battery->setStyleSheet("color: #f0f6fc;");
    infoRow->addWidget(m_speed);
    infoRow->addStretch();
    infoRow->addWidget(m_battery);
    layout->addLayout(infoRow);

    // 运行信息
    m_runInfo = new QLabel("运行: --次");
    m_runInfo->setStyleSheet("color: #8b949e; font-size: 12px;");
    layout->addWidget(m_runInfo);

    // 更新时间
    m_updateTime = new QLabel("--");
    m_updateTime->setStyleSheet("color: #484f58; font-size: 11px;");
    m_updateTime->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_updateTime);

    layout->addStretch();
}

void ElevatorCard::updateInfo(const ElevatorInfo& info) {
    // 设备ID
    m_deviceId->setText(info.deviceId);

    // 在线状态
    if (!info.online) {
        setStyleSheet(CARD_OFFLINE);
        m_floorNo->setText("离线");
        m_floorNo->setStyleSheet("font-size: 36px; color: #484f58;");
        m_direction->setText("");
        m_doorStatus->setText("");
        m_people->setText("");
        m_temp->setText("--℃");
        m_hum->setText("--%");
        m_pm->setText("PM --");
        m_speed->setText("-- m/s");
        m_battery->setText("-- V");
        m_runInfo->setText("");
        return;
    }

    // 楼层
    m_floorNo->setText(info.floorNo);
    m_floorNo->setStyleSheet("font-size: 56px; font-weight: bold; color: #f0f6fc;");

    // 方向
    QString dirText = "停止";
    QString dirColor = "#8b949e";
    if (info.status == "1") { dirText = "▲ 上行"; dirColor = "#58a6ff"; }
    else if (info.status == "2") { dirText = "▼ 下行"; dirColor = "#58a6ff"; }
    m_direction->setText(dirText);
    m_direction->setStyleSheet(QString("font-size: 14px; color: %1;").arg(dirColor));

    // 开关门
    QString doorText, doorColor;
    int d = info.door.toInt();
    if (d == 0)      { doorText = "关门"; doorColor = "#7ee787"; }
    else if (d == 1) { doorText = "开门"; doorColor = "#f85149"; }
    else if (d == 2) { doorText = "关门到位"; doorColor = "#7ee787"; }
    else if (d == 3) { doorText = "开门中"; doorColor = "#d29922"; }
    else if (d == 4) { doorText = "开门到位"; doorColor = "#f85149"; }
    else if (d == 5) { doorText = "关门中"; doorColor = "#d29922"; }
    m_doorStatus->setText(doorText);
    m_doorStatus->setStyleSheet(QString("font-size: 14px; color: %1;").arg(doorColor));

    // 有人/无人
    bool hasPeople = (info.people == "1");
    m_people->setText(hasPeople ? "有人" : "无人");
    m_people->setStyleSheet(QString("font-size: 13px; color: %1;").arg(hasPeople ? "#f0883e" : "#8b949e"));

    // 温湿度
    m_temp->setText(info.temp + "℃");
    m_hum->setText(info.hum + "%");

    // PM
    QString pmText = QString("PM%1").arg(info.pm2d5.toDouble() > info.pm10.toDouble() ? "2.5" : "10");
    m_pm->setText(pmText + " " + (info.pm2d5.toDouble() > info.pm10.toDouble() ? info.pm2d5 : info.pm10));

    // 速度
    m_speed->setText(info.speed + " m/s");

    // 电池
    m_battery->setText(info.batteryVoltage + "V");

    // 运行信息
    m_runInfo->setText(QString("运行:%1次 | 评分:%2 | 电动车:%3次")
        .arg(info.runNum).arg(info.grade).arg(info.ebikeCnt));

    // 报警闪烁
    setAlarm(info.hasAlarm);

    // 更新时间
    if (!info.updateTime.isEmpty()) {
        m_updateTime->setText("更新: " + info.updateTime);
    }
}

void ElevatorCard::setAlarm(bool alarm) {
    if (alarm) {
        setStyleSheet(CARD_ALARM);
        m_alarmIndicator->setStyleSheet(
            "background: #f85149; border-radius: 6px;");
    } else {
        setStyleSheet(CARD_NORMAL);
        m_alarmIndicator->setStyleSheet(
            "background: #238636; border-radius: 6px;");
    }
}

// ==================== MainWindow ====================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    initDatabase();

    // 每秒刷新
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::refreshData);
    m_timer->start(1000);

    refreshData();
}

MainWindow::~MainWindow() {
    if (m_db.isOpen()) m_db.close();
}

void MainWindow::setupUI() {
    setWindowTitle("ES2000 电梯运行实时监控系统");
    setStyleSheet(DARK_STYLE);

    // 全屏显示（HDMI 大屏）
    showFullScreen();

    auto* central = new QWidget();
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(16, 8, 16, 8);
    mainLayout->setSpacing(8);

    // 顶部标题栏
    auto* titleBar = new QHBoxLayout();
    m_titleLabel = new QLabel("电梯运行实时监控系统");
    m_titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #f0f6fc;"
        "padding: 8px 16px;");

    m_clockLabel = new QLabel();
    m_clockLabel->setStyleSheet("font-size: 14px; color: #8b949e;");

    titleBar->addWidget(m_titleLabel);
    titleBar->addStretch();
    titleBar->addWidget(m_clockLabel);
    mainLayout->addLayout(titleBar);

    // 滚动区域 - 电梯卡片网格
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    auto* cardContainer = new QWidget();
    m_gridLayout = new QGridLayout(cardContainer);
    m_gridLayout->setSpacing(12);
    m_gridLayout->setContentsMargins(8, 8, 8, 8);

    scrollArea->setWidget(cardContainer);
    mainLayout->addWidget(scrollArea);
}

void MainWindow::initDatabase() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/root/es2000/es2000.db");

    if (!m_db.open()) {
        qWarning() << "数据库连接失败:" << m_db.lastError().text();
    }
}

QList<ElevatorInfo> MainWindow::queryAllElevators() {
    QList<ElevatorInfo> list;

    if (!m_db.isOpen()) {
        // 返回模拟数据用于演示
        for (int i = 1; i <= 6; i++) {
            ElevatorInfo info;
            info.deviceId = QString("600%1").arg(i);
            info.online = (i <= 4);
            info.floorNo = QString::number(i);
            info.door = "0";
            info.status = (i % 3 == 0) ? "2" : ((i % 3 == 1) ? "1" : "0");
            info.temp = QString::number(22 + i);
            info.hum = QString::number(45 + i * 2);
            info.pm2d5 = QString::number(15 + i * 5);
            info.pm10 = QString::number(30 + i * 3);
            info.speed = "0.0";
            info.people = (i == 2) ? "1" : "0";
            info.maintain = "0";
            info.batteryVoltage = "12.0";
            info.runNum = i * 100;
            info.grade = 85 + i;
            info.ebikeCnt = 0;
            info.hasAlarm = (i == 4);
            info.updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            list.append(info);
        }
        return list;
    }

    QSqlQuery query(m_db);
    query.exec("SELECT * FROM device_status ORDER BY device_id");

    while (query.next()) {
        ElevatorInfo info;
        info.deviceId       = query.value("device_id").toString();
        info.floorNo        = query.value("floor_no").toString();
        info.door           = query.value("door").toString();
        info.status         = query.value("status").toString();
        info.speed          = query.value("speed").toString();
        info.temp           = query.value("temp").toString();
        info.hum            = query.value("hum").toString();
        info.pm2d5          = query.value("pm2d5").toString();
        info.pm10           = query.value("pm10").toString();
        info.accX           = query.value("acc_x").toString();
        info.accY           = query.value("acc_y").toString();
        info.accZ           = query.value("acc_z").toString();
        info.amplitude      = query.value("amplitude").toString();
        info.degreeFb       = query.value("degree_fb").toString();
        info.degreeLr       = query.value("degree_lr").toString();
        info.people         = query.value("people").toString();
        info.maintain       = query.value("maintain").toString();
        info.ebikeCnt       = query.value("ebike_cnt").toInt();
        info.runNum         = query.value("run_num").toInt();
        info.runTime        = query.value("run_time").toInt();
        info.grade          = query.value("grade").toInt();
        info.hasAlarm       = query.value("has_alarm").toInt() == 1;
        info.updateTime     = query.value("update_time").toString();
        info.batteryVoltage = "12.0";  // TODO: 从协议获取
        info.online         = true;
        list.append(info);
    }

    return list;
}

void MainWindow::refreshData() {
    // 更新时钟
    m_clockLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 查询电梯数据
    QList<ElevatorInfo> elevators = queryAllElevators();

    // 清除旧卡片
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->hide();
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_cards.clear();

    // 计算每行卡片数
    int screenWidth = QApplication::desktop()->screenGeometry().width();
    int cols = std::max(1, (screenWidth - 32) / 340);  // 320px + 12px margin

    // 添加卡片
    for (int i = 0; i < elevators.size(); i++) {
        auto* card = new ElevatorCard();
        card->updateInfo(elevators[i]);

        int row = i / cols;
        int col = i % cols;
        m_gridLayout->addWidget(card, row, col);
        m_cards[elevators[i].deviceId] = card;
    }

    // 如果没有数据，显示提示
    if (elevators.isEmpty()) {
        auto* emptyLabel = new QLabel("等待设备数据...");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("font-size: 24px; color: #484f58; padding: 100px;");
        m_gridLayout->addWidget(emptyLabel, 0, 0, 1, 1);
    }
}
