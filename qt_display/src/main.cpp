/**
 * ES3500 电梯运行监测系统 - 入口
 * 开机画面 → 9宫格大屏
 */
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QFont>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ES3500 Elevator Monitor");
    app.setApplicationVersion("2.0.0");

    QFont font("WenQuanYi Micro Hei", 10);
    font.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(font);

    // ========== 开机画面 ==========
    QPixmap logoPixmap(800, 480);
    {
        logoPixmap.fill(QColor("#0d47a1"));  // 深蓝背景
        QPainter p(&logoPixmap);
        p.setRenderHint(QPainter::Antialiasing);

        // Logo 图片（如果存在）
        QPixmap neLogo("/root/es2000/logo/LOGO_NE注册.png");
        if (!neLogo.isNull()) {
            neLogo = neLogo.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.drawPixmap(300, 50, neLogo);
        }

        // 公司名 + 系统名
        QFont titleFont("WenQuanYi Micro Hei", 36, QFont::Bold);
        p.setFont(titleFont);
        p.setPen(Qt::white);
        p.drawText(QRect(0, 250, 800, 80), Qt::AlignCenter, "恩易电梯运行监测系统");

        // 启动提示
        QFont hintFont("WenQuanYi Micro Hei", 18);
        p.setFont(hintFont);
        p.setPen(QColor("#90CAF9"));
        p.drawText(QRect(0, 340, 800, 40), Qt::AlignCenter, "正在启动系统服务...");
    }

    QSplashScreen splash(logoPixmap);
    splash.show();
    app.processEvents();

    // ========== 3 秒后进入主界面 ==========
    MainWindow window;
    QTimer::singleShot(3000, [&]() {
        splash.close();
        window.showFullScreen();
    });

    return app.exec();
}
