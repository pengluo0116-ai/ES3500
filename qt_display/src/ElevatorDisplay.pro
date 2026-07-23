QT       += core gui sql widgets
TARGET    = es3500_display
TEMPLATE  = app

CONFIG   += c++11
DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES  += main.cpp mainwindow.cpp
HEADERS  += mainwindow.h

# ARM 平台优化
unix:!macx {
    target.path = /root/es2000/
    INSTALLS += target
}

# 调试输出
CONFIG(debug, debug|release) {
    DEFINES += DEBUG_MODE
}
