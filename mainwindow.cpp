#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QTcpSocket>
#include <chrono>
#include <thread>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();

    setWindowTitle("文件服务器测试程序");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QScreen *screen = QGuiApplication::screens().at(0);
    float width = 512;
    float height = 320;
    if (nullptr != screen)
    {
        QRect rect = screen->availableGeometry();
        width = rect.width() * 0.64 >= 512 ? 512 : rect.width() * 0.64;
        height = rect.height() * 0.64 >= 320 ? 320 : rect.height() * 0.64;
    }

    resize(width, height);
    connect(ui->btnDownload, &QPushButton::clicked, this, &MainWindow::slot_download_file);
    connect(ui->btnUpload, &QPushButton::clicked, this, &MainWindow::slot_upload_file);

    mFtpManager = new FtpManager;
    mFtpManager->setFtpHost("192.168.1.103");
    mFtpManager->setDownloadPath("C:\\Users\\admin\\Desktop");
    connect(mFtpManager, &FtpManager::sgl_file_download_process, this, &MainWindow::slot_file_download_percent);
    connect(mFtpManager, &FtpManager::sgl_file_task_finish, this, &MainWindow::slot_file_task_finish);

    ui->tbDownload->setText("高清云台_1.mp4");
}

void MainWindow::slot_download_file()
{
    QString file = ui->tbDownload->text().trimmed();
    mFtpManager->downloadFile(file);
}

void MainWindow::slot_upload_file()
{
    QString file = ui->tbUpload->text().trimmed();
    mFtpManager->uploadFile(file);
}

void MainWindow::slot_file_download_percent(const QString &file, float percent)
{
    if (file == ui->tbDownload->text())
    {
        ui->widgetProgress->setValue(percent);
    }
}

void MainWindow::slot_file_task_finish(const QString &file, bool status, const QString &msg)
{
    qDebug() << "file " << file << "   " << "status " << status << "   " << "msg " << msg;
}

