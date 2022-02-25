#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载样式
    qApp->setStyleSheet("file:///:/Resourse/qss/style.qss");

    MainWindow w;
    w.show();

    return a.exec();
}
