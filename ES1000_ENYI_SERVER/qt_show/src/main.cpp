#include "main.h"
using namespace std;

int main(int argc, char **argv){
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    /*创建窗体*/
    Global_window::mwindow = new window;
    
    /*工作线程开启*/
    real_cont::init();
    real_cont::start();

    radio::init();
    radio::start();

    Global_window::mwindow->init();

    return app.exec();
}
