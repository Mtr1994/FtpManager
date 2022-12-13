#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ftpmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

private slots:

    void slot_download_file();
    void slot_upload_file();

    void slot_file_download_percent(const QString &file, float percent);
    void slot_file_upload_percent(const QString &file, float percent);
    void slot_ftp_upload_task_finish(const QString &file, bool status, const QString &msg);

private:
    Ui::MainWindow *ui;

    QTcpSocket *mSocketDataPtr2 = nullptr;
    uint64_t mDataPort2 = 0;

    bool mConncted = false;

    uint64_t mDataPort = 0;

    uint mTaskIndex = 0;

    FtpManager *mFtpManager = nullptr;
};
#endif // MAINWINDOW_H
