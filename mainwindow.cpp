#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    connect(ui->btnDownload, &QPushButton::clicked, this, &MainWindow::slot_download_file);
    connect(ui->btnUpload, &QPushButton::clicked, this, &MainWindow::slot_upload_file);

    ui->widgetProgress->setProgressType(WidgetProgress::P_Cicle);
    ui->widgetProgress->setCicleWidth(10);

    mFtpManager = new FtpManager("101.34.253.220", "idsse", "123456");
    // 开启/关闭断点续传
    mFtpManager->setOpenBreakPointResume(true);
    mFtpManager->setDownloadPath("C:\\Users\\admin\\Desktop\\Dot");
    connect(mFtpManager, &FtpManager::sgl_file_download_process, this, &MainWindow::slot_file_download_percent);
    connect(mFtpManager, &FtpManager::sgl_file_upload_process, this, &MainWindow::slot_file_upload_percent);
    connect(mFtpManager, &FtpManager::sgl_ftp_upload_task_finish, this, &MainWindow::slot_ftp_upload_task_finish);

    connect(mFtpManager, &FtpManager::sgl_ftp_connect_status_change, this, [this](bool status)
    {
        ui->statusBar->showMessage(status ? "文件服务连接成功" : "文件服务连接失败或断开");
    });

    // 初始化文件服务连接
    mFtpManager->init();

    ui->tbDownload->setText("demo3.tif");
    ui->tbUpload->setText("C:\\Users\\admin\\Desktop\\demo3.tif");
}

void MainWindow::slot_download_file()
{
    QString file = ui->tbDownload->text().trimmed();
    mFtpManager->downloadFile(file, "upload/TS-261");
}

void MainWindow::slot_upload_file()
{
    QString file = ui->tbUpload->text().trimmed();
    mFtpManager->uploadFile(file, "upload/TS-261");
}

void MainWindow::slot_file_download_percent(const QString &file, float percent)
{
    if (file == ui->tbDownload->text())
    {
        ui->widgetProgress->setValue(percent);
    }
}

void MainWindow::slot_file_upload_percent(const QString &file, float percent)
{
    if (file == ui->tbUpload->text())
    {
        ui->widgetProgress->setValue(percent);
    }
}

void MainWindow::slot_ftp_upload_task_finish(const QString &file, bool status, const QString &msg)
{
    qDebug() << "file " << file << "   " << "status " << status << "   " << "msg " << msg;

    ui->statusBar->showMessage(msg);
}

