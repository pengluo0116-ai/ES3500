/**
 * ES2000 电梯运行实时监控系统 - Qt5 显示程序
 * 
 * 功能：从 SQLite 读取数据，深色仪表盘展示多电梯实时状态
 * 编译：qmake && make
 * 运行：./elevator_display
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QSqlDatabase>

// 电梯卡片数据结构
struct ElevatorInfo {
    QString deviceId;
    QString floorNo;
    QString door;       // 0:关门 1:开门 2:关门到位 3:开门中 4:开门到位 5:关门中
    QString status;     // 0:停止 1:上行 2:下行
    QString speed;
    QString temp;
    QString hum;
    QString pm2d5;
    QString pm10;
    QString accX;
    QString accY;
    QString accZ;
    QString amplitude;
    QString degreeFb;
    QString degreeLr;
    QString people;     // 0:无人 1:有人
    QString maintain;   // 0:未维保 1:维保中
    int     ebikeCnt;
    int     runNum;
    int     runTime;
    int     grade;
    bool    hasAlarm;
    QString updateTime;
    QString batteryVoltage;  // 电池电压
    bool    online;          // 在线状态
};

// 电梯卡片 Widget
class ElevatorCard : public QFrame {
    Q_OBJECT
public:
    explicit ElevatorCard(QWidget* parent = nullptr);
    void updateInfo(const ElevatorInfo& info);
    void setAlarm(bool alarm);

private:
    void setupUI();

    QLabel* m_deviceId;
    QLabel* m_floorNo;
    QLabel* m_direction;     // 上行/下行/停止
    QLabel* m_doorStatus;    // 开关门状态
    QLabel* m_people;
    QLabel* m_temp;
    QLabel* m_hum;
    QLabel* m_pm;
    QLabel* m_speed;
    QLabel* m_battery;
    QLabel* m_runInfo;
    QLabel* m_updateTime;
    QLabel* m_alarmIndicator;
    QFrame* m_cardBg;
};

// 主窗口
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void refreshData();

private:
    void setupUI();
    void initDatabase();
    QList<ElevatorInfo> queryAllElevators();

    QTimer*              m_timer;
    QGridLayout*         m_gridLayout;
    QMap<QString, ElevatorCard*> m_cards;
    QLabel*              m_titleLabel;
    QLabel*              m_clockLabel;
    QSqlDatabase         m_db;
};

#endif
