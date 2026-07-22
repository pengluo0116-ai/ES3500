/**
 * ES2000 电梯运行实时监控系统 - 入口
 */
#include <QApplication>
#include <QFont>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 全局字体
    QFont font("Microsoft YaHei", 10);
    font.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(font);

    // 应用名
    app.setApplicationName("ES2000 Elevator Monitor");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    window.show();

    return app.exec();
}
