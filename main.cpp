#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载样式
    qApp->setStyleSheet("file:///:/Resourse/qss/style.qss");

    MainWindow w;
    w.show();

    // 处理因为 QSS 设置主窗口大小导致的程序默认不居中的问题
    QSize screenSize = a.primaryScreen()->availableSize();
    w.move((screenSize.width() - w.width()) / 2, (screenSize.height() - w.height()) / 2);

    return a.exec();
}
