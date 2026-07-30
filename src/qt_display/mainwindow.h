/**
 * ES3500 电梯运行监测系统 - 主窗口
 * 9宫格布局 + 报警红色置顶 + 语音播报
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QSqlDatabase>
#include <QProcess>
#include <QFrame>

// 电梯数据结构
struct ElevatorInfo {
    QString deviceId;
    QString floorNo;
    QString door;
    QString status;     // 0:停止 1:上行 2:下行
    QString speed;
    QString temp;
    QString hum;
    QString pm2d5;
    QString pm10;
    QString people;
    QString maintain;
    int     ebikeCnt;
    int     runNum;
    int     grade;
    bool    hasAlarm;
    QString alarmDesc;   // 报警描述
    QString updateTime;
    bool    online;
    QString gatewayIp;   // ES1500网关IP
    QString location;    // 位置（如"3号楼"）
};

// 单部电梯卡片
class ElevatorCard : public QFrame {
    Q_OBJECT
public:
    explicit ElevatorCard(QWidget* parent = nullptr);
    void updateInfo(const ElevatorInfo& info);
    bool hasAlarm() const { return m_hasAlarm; }
    QString deviceId() const { return m_deviceId->text(); }

private:
    void setupUI();
    QLabel* m_deviceId;
    QLabel* m_floorNo;
    QLabel* m_direction;
    QLabel* m_doorStatus;
    QLabel* m_people;
    QLabel* m_temp;
    QLabel* m_hum;
    QLabel* m_location;
    QLabel* m_runInfo;
    QLabel* m_updateTime;
    QLabel* m_alarmDot;
    bool    m_hasAlarm = false;
};

// 主窗口 - 9宫格
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void refreshData();

private:
    void setupUI();
    QList<ElevatorInfo> queryElevators();
    void speakAlarm(const ElevatorInfo& elev);
    void sortCards();

    QTimer*              m_timer;
    QGridLayout*         m_gridLayout;
    QWidget*             m_cardContainer;
    QLabel*              m_titleLabel;
    QLabel*              m_clockLabel;
    QSqlDatabase         m_db;
    QList<ElevatorCard*> m_cards;
    QSet<QString>        m_playedAlarms;  // 已播报过的报警
    QProcess*            m_ttsProcess;
};

#endif
