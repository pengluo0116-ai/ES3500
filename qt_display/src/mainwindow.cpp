/**
 * ES3500 电梯运行监测系统 - 9宫格大屏
 */
#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QFont>
#include <QTimer>
#include <QProcess>
#include <QSet>
#include <algorithm>

// ==================== 主题 ====================
static const char* DARK_STYLE = R"(
    QMainWindow { background: #0a0e17; }
    QLabel { color: #c9d1d9; background: transparent; }
)";

static const char* CARD_NORMAL = R"(
    QFrame {
        background: #161b22; border: 2px solid #21262d;
        border-radius: 10px; padding: 6px;
    }
)";

static const char* CARD_ALARM = R"(
    QFrame {
        background: #2d1216; border: 3px solid #f85149;
        border-radius: 10px; padding: 6px;
    }
)";

// ==================== ElevatorCard ====================

ElevatorCard::ElevatorCard(QWidget* parent) : QFrame(parent) {
    setupUI();
    setFixedSize(380, 300);
    setStyleSheet(CARD_NORMAL);
}

void ElevatorCard::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(3);
    layout->setContentsMargins(10, 6, 10, 6);

    // 顶行：设备ID + 位置 + 报警点
    auto* topRow = new QHBoxLayout();
    m_alarmDot = new QLabel();
    m_alarmDot->setFixedSize(10, 10);
    m_alarmDot->setStyleSheet("background: #238636; border-radius: 5px;");
    
    m_deviceId = new QLabel("6001");
    m_deviceId->setStyleSheet("font-size: 16px; font-weight: bold; color: #58a6ff;");
    
    m_location = new QLabel();
    m_location->setStyleSheet("font-size: 11px; color: #484f58;");
    
    topRow->addWidget(m_alarmDot);
    topRow->addWidget(m_deviceId);
    topRow->addStretch();
    topRow->addWidget(m_location);
    layout->addLayout(topRow);

    // 分隔线
    auto* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #1a1f27; max-height: 1px;");
    layout->addWidget(sep);

    // 楼层 大字
    m_floorNo = new QLabel("1");
    m_floorNo->setAlignment(Qt::AlignCenter);
    m_floorNo->setStyleSheet("font-size: 48px; font-weight: bold; color: #f0f6fc;");
    layout->addWidget(m_floorNo);

    // 方向
    m_direction = new QLabel("停止");
    m_direction->setAlignment(Qt::AlignCenter);
    m_direction->setStyleSheet("font-size: 14px; color: #8b949e;");
    layout->addWidget(m_direction);

    // 开关门
    m_doorStatus = new QLabel("关门");
    m_doorStatus->setAlignment(Qt::AlignCenter);
    m_doorStatus->setStyleSheet("font-size: 13px; color: #7ee787;");
    layout->addWidget(m_doorStatus);

    // 有人/无人
    m_people = new QLabel("无人");
    m_people->setAlignment(Qt::AlignCenter);
    m_people->setStyleSheet("font-size: 12px; color: #8b949e;");
    layout->addWidget(m_people);

    // 温湿度行
    auto* envRow = new QHBoxLayout();
    m_temp = new QLabel("--℃");
    m_hum = new QLabel("--%");
    m_temp->setStyleSheet("color: #d2a8ff; font-size: 12px;");
    m_hum->setStyleSheet("color: #79c0ff; font-size: 12px;");
    envRow->addWidget(m_temp);
    envRow->addStretch();
    envRow->addWidget(m_hum);
    layout->addLayout(envRow);

    // 运行信息
    m_runInfo = new QLabel();
    m_runInfo->setStyleSheet("color: #484f58; font-size: 11px;");
    layout->addWidget(m_runInfo);

    // 更新时间
    m_updateTime = new QLabel("--");
    m_updateTime->setStyleSheet("color: #30363d; font-size: 10px;");
    m_updateTime->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_updateTime);
}

void ElevatorCard::updateInfo(const ElevatorInfo& info) {
    m_deviceId->setText(info.deviceId);
    m_location->setText(info.location.isEmpty() ? "" : info.location);

    if (!info.online) {
        setStyleSheet(CARD_NORMAL);
        m_floorNo->setText("离线");
        m_floorNo->setStyleSheet("font-size: 32px; color: #484f58;");
        m_hasAlarm = false;
        m_alarmDot->setStyleSheet("background: #484f58; border-radius: 5px;");
        return;
    }

    // 楼层
    m_floorNo->setText(info.floorNo);
    m_floorNo->setStyleSheet("font-size: 48px; font-weight: bold; color: #f0f6fc;");

    // 方向
    QString dir = "停止", dirColor = "#8b949e";
    if (info.status == "1") { dir = "▲ 上行"; dirColor = "#58a6ff"; }
    else if (info.status == "2") { dir = "▼ 下行"; dirColor = "#58a6ff"; }
    m_direction->setText(dir);
    m_direction->setStyleSheet(QString("font-size: 14px; color: %1;").arg(dirColor));

    // 开关门
    int d = info.door.toInt();
    QString dt, dc;
    if (d == 0) { dt = "关门"; dc = "#7ee787"; }
    else if (d == 1 || d == 4) { dt = "开门"; dc = "#f85149"; }
    else if (d == 3 || d == 5) { dt = "运行中"; dc = "#d29922"; }
    else { dt = "关门到位"; dc = "#7ee787"; }
    m_doorStatus->setText(dt);
    m_doorStatus->setStyleSheet(QString("font-size: 13px; color: %1;").arg(dc));

    // 有人/无人
    bool hasPeople = (info.people == "1");
    m_people->setText(hasPeople ? "🟢 有人" : "无人");
    m_people->setStyleSheet(QString("font-size: 12px; color: %1;").arg(hasPeople ? "#f0883e" : "#8b949e"));

    // 温湿度
    m_temp->setText(info.temp + "℃");
    m_hum->setText(info.hum + "%");

    // 运行信息
    m_runInfo->setText(QString("运行%1次 | 评分%2").arg(info.runNum).arg(info.grade));

    // 报警状态
    m_hasAlarm = info.hasAlarm;
    if (m_hasAlarm) {
        setStyleSheet(CARD_ALARM);
        m_alarmDot->setStyleSheet("background: #f85149; border-radius: 5px;");
        m_deviceId->setStyleSheet("font-size: 16px; font-weight: bold; color: #f85149;");
    } else {
        setStyleSheet(CARD_NORMAL);
        m_alarmDot->setStyleSheet("background: #238636; border-radius: 5px;");
        m_deviceId->setStyleSheet("font-size: 16px; font-weight: bold; color: #58a6ff;");
    }

    m_updateTime->setText("更新: " + info.updateTime);
}

// ==================== MainWindow ====================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_ttsProcess(nullptr) {
    setupUI();

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/root/es2000/es2000.db");
    m_db.open();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::refreshData);
    m_timer->start(1000);

    refreshData();
}

MainWindow::~MainWindow() {
    if (m_ttsProcess) { m_ttsProcess->kill(); delete m_ttsProcess; }
    if (m_db.isOpen()) m_db.close();
}

void MainWindow::setupUI() {
    setWindowTitle("恩易电梯运行监测系统");
    setStyleSheet(DARK_STYLE);

    auto* central = new QWidget();
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 6, 12, 6);
    mainLayout->setSpacing(6);

    // 标题栏
    auto* titleBar = new QHBoxLayout();
    m_titleLabel = new QLabel("恩易电梯运行监测系统");
    m_titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #f0f6fc;");
    m_clockLabel = new QLabel();
    m_clockLabel->setStyleSheet("font-size: 14px; color: #8b949e;");
    titleBar->addWidget(m_titleLabel);
    titleBar->addStretch();
    titleBar->addWidget(m_clockLabel);
    mainLayout->addLayout(titleBar);

    // 9宫格容器
    m_cardContainer = new QWidget();
    m_gridLayout = new QGridLayout(m_cardContainer);
    m_gridLayout->setSpacing(8);
    m_gridLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->addWidget(m_cardContainer);
}

QList<ElevatorInfo> MainWindow::queryElevators() {
    QList<ElevatorInfo> list;

    if (!m_db.isOpen()) {
        // 演示数据
        for (int i = 1; i <= 9; i++) {
            ElevatorInfo info;
            info.deviceId = QString::number(6000 + i);
            info.online = (i <= 8);
            info.floorNo = QString::number(i);
            info.door = (i == 3) ? "1" : "0";
            info.status = (i % 3 == 1) ? "1" : ((i % 3 == 2) ? "2" : "0");
            info.temp = QString::number(22 + i);
            info.hum = QString::number(45 + i * 2);
            info.people = (i == 2 || i == 5) ? "1" : "0";
            info.runNum = i * 100;
            info.grade = 85 + i;
            info.hasAlarm = (i == 3 || i == 7);
            info.alarmDesc = info.hasAlarm ? "困人报警" : "";
            info.location = QString("%1号楼").arg(i);
            info.updateTime = QDateTime::currentDateTime().toString("hh:mm:ss");
            list.append(info);
        }
        return list;
    }

    QSqlQuery query(m_db);
    query.exec("SELECT * FROM device_status ORDER BY has_alarm DESC, device_id ASC");
    while (query.next()) {
        ElevatorInfo info;
        info.deviceId   = query.value("device_id").toString();
        info.floorNo    = query.value("floor_no").toString();
        info.door       = query.value("door").toString();
        info.status     = query.value("status").toString();
        info.temp       = query.value("temp").toString();
        info.hum        = query.value("hum").toString();
        info.people     = query.value("people").toString();
        info.runNum     = query.value("run_num").toInt();
        info.grade      = query.value("grade").toInt();
        info.hasAlarm   = query.value("has_alarm").toInt() == 1;
        info.updateTime = query.value("update_time").toString();
        info.online     = true;
        list.append(info);
    }
    return list;
}

void MainWindow::refreshData() {
    m_clockLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 清除旧卡片
    for (auto* card : m_cards) {
        m_gridLayout->removeWidget(card);
        card->deleteLater();
    }
    m_cards.clear();

    // 查询数据（报警在前）
    QList<ElevatorInfo> elevators = queryElevators();

    // 创建9宫格卡片
    for (int i = 0; i < elevators.size() && i < 9; i++) {
        const auto& info = elevators[i];
        auto* card = new ElevatorCard();
        card->updateInfo(info);
        m_cards.append(card);

        int row = i / 3;
        int col = i % 3;
        m_gridLayout->addWidget(card, row, col);

        // 语音播报
        if (info.hasAlarm && !m_playedAlarms.contains(info.deviceId)) {
            speakAlarm(info);
            m_playedAlarms.insert(info.deviceId);
        }
        if (!info.hasAlarm) {
            m_playedAlarms.remove(info.deviceId);  // 报警消除后可再次播报
        }
    }

    // 填充空格
    for (int i = elevators.size(); i < 9; i++) {
        auto* empty = new QLabel("空");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("font-size: 20px; color: #1a1f27; border: 2px dashed #21262d; border-radius: 10px;");
        m_gridLayout->addWidget(empty, i / 3, i % 3);
    }
}

void MainWindow::speakAlarm(const ElevatorInfo& elev) {
    // 构建语音内容
    QString text;
    if (!elev.location.isEmpty()) text += elev.location;
    if (!elev.floorNo.isEmpty()) text += elev.floorNo + "层";
    text += elev.alarmDesc.isEmpty() ? "电梯故障" : elev.alarmDesc;
    text += "，请立即处理";

    // 用 espeak 语音合成（中文）
    QProcess* proc = new QProcess(this);
    connect(proc, QOverload<int>::of(&QProcess::finished), proc, &QProcess::deleteLater);
    proc->start("espeak", QStringList() << "-v" << "zh" << text);
    
    // 也尝试 festival
    QProcess* proc2 = new QProcess(this);
    connect(proc2, QOverload<int>::of(&QProcess::finished), proc2, &QProcess::deleteLater);
    proc2->start("bash", QStringList() << "-c" 
        << QString("echo '%1' | festival --tts 2>/dev/null || true").arg(text));
}

void MainWindow::sortCards() {
    // 报警卡片已在 queryElevators 中通过 SQL 排序
}
