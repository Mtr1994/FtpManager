﻿#include "ftpmanager.h"

#include <QFile>

// test
#include <QDebug>
#include <QThread>

FtpManager::FtpManager(QObject *parent)
    : QObject{parent}
{

}

void FtpManager::downloadFile(const QString &file)
{
    if (mFtpHost.isEmpty())
    {
        emit sgl_file_task_finish(file, false, "未指定文件服务器地址");
        return;
    }
    FtpProtocol *ftp = new FtpProtocol(mFtpHost, file, mDownloadPath);
    mMapThread.insert(file, new QThread());
    ftp->moveToThread(mMapThread.value(file));
    connect(ftp, &FtpProtocol::sgl_file_download_process, this, &FtpManager::sgl_file_download_process, Qt::QueuedConnection);
    connect(ftp, &FtpProtocol::sgl_file_task_finish, this, &FtpManager::slot_file_task_finish, Qt::QueuedConnection);
    connect(mMapThread.value(file), &QThread::started, ftp, &FtpProtocol::slot_start_file_download, Qt::QueuedConnection);
    mMapThread.value(file)->start();
}

void FtpManager::uploadFile(const QString &file)
{
    if (!QFile::exists(file))
    {
        emit sgl_file_task_finish(file, false, "文件不存在");
        return;
    }
    if (mFtpHost.isEmpty())
    {
        emit sgl_file_task_finish(file, false, "未指定文件服务器地址");
        return;
    }
    FtpProtocol *ftp = new FtpProtocol(mFtpHost, file, mDownloadPath);
    mMapThread.insert(file, new QThread());
    ftp->moveToThread(mMapThread.value(file));
    connect(ftp, &FtpProtocol::sgl_file_upload_process, this, &FtpManager::sgl_file_upload_process, Qt::QueuedConnection);
    connect(ftp, &FtpProtocol::sgl_file_task_finish, this, &FtpManager::slot_file_task_finish, Qt::QueuedConnection);
    connect(mMapThread.value(file), &QThread::started, ftp, &FtpProtocol::slot_start_file_upload, Qt::QueuedConnection);
    mMapThread.value(file)->start();
}

void FtpManager::slot_file_task_finish(const QString &file, bool status, const QString &msg)
{
    Q_UNUSED(status);
    if (!mMapThread.contains(file)) return;
    auto thread = mMapThread.take(file);
    connect(thread, &QThread::finished, this, [thread](){ thread->deleteLater();});
    thread->exit(0);

    // 发送消息给前端，判定任务状态
    emit sgl_file_task_finish(file, status, msg);
}

const QString &FtpManager::getDownloadPath() const
{
    return mDownloadPath;
}

void FtpManager::setDownloadPath(const QString &newDownloadPath)
{
    mDownloadPath = newDownloadPath;
}

const QString &FtpManager::getFtpHost() const
{
    return mFtpHost;
}

void FtpManager::setFtpHost(const QString &newFtpHost)
{
    mFtpHost = newFtpHost;
}
