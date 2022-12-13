#include "ftpmanager.h"

#include <QFile>

// test
#include <QDebug>
#include <QThread>

FtpProtocol::FtpProtocol(const QString &host, const QString &user, const QString &pass) : mFtpHost(host), mFtpUserName(user), mFtpUserPass(pass)
{
    // 填充状态码
    mStatusObject = QJsonDocument::fromJson("{"
                                            "\"CONE\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"N\"},"
                                            "\"ABOR\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"ALLO\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"DELE\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"CWD\":  {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"CDUP\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"SMNT\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"HELP\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"MODE\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"NOOP\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"PASV\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"QUIT\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"SITE\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"PORT\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"SYST\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"STAT\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"RMD\":  {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"MKD\":  {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"PWD\":  {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"STRU\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"TYPE\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"APPE\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"LIST\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"NLST\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"REIN\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"RETR\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"STOR\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"STOU\": {\"1\": \"W\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"RNFR\": {\"1\": \"E\", \"2\": \"E\", \"3\": \"V\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"RNTO\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"REST\": {\"1\": \"E\", \"2\": \"E\", \"3\": \"V\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"USER\": {\"1\": \"E\", \"2\": \"E\", \"3\": \"V\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"PASS\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"V\", \"4\": \"F\", \"5\": \"F\"},"
                                            "\"ACCT\": {\"1\": \"E\", \"2\": \"S\", \"3\": \"E\", \"4\": \"F\", \"5\": \"F\"}"
                                            "}").object();
    //Connection Establishment
    mListCommand.append("CONE");
}

void FtpProtocol::slot_start_connect_ftp_server()
{
    if ((nullptr != mSocketCommand) && (mSocketCommand->state() == QTcpSocket::ConnectedState)) return;

    mListCommand.clear();

    // 准备连接文件服务
    mListCommand.append("CONE");

    mSocketCommand = new QTcpSocket(this);
    connect(mSocketCommand, &QTcpSocket::disconnected, this, &FtpProtocol::slot_socket_command_close);
    connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_socket_command_data_recv);
    connect(mSocketCommand, &QTcpSocket::connected, this, &FtpProtocol::slot_socket_command_connect);
    mSocketCommand->connectToHost(QHostAddress(mFtpHost), 21);
}

void FtpProtocol::slot_ftp_start_upload_file(const QString &file, const QString &remotepath, bool breakpointresume)
{
    mUploadFileFlag = true;
    mFileName = file;
    mRemoteFilePath = remotepath;
    mLocalFileInfo = QFileInfo(file);

    // 重置上传的文件大小
    mTotalFileSize = 0;
    mTotalUploadLength = 0;
    mTaskStatus = false;
    mServerFileInfo.clear();

    // 返回到顶层文件夹( 在使用时，根据实际情况更改根目录位置 )
    mListCommand.append(QString("CWD /home/ftp_root\r\n").toUtf8());

    // 切换路径 (目录只能一层层进入，因为碰到最内部文件夹不存在，没办法一次创建多层文件夹)
    auto listDirName = QDir(mRemoteFilePath).path().split('/');
    for (auto &name : listDirName)
    {
        mListCommand.append(QString("CWD %1\r\n").arg(name).toUtf8());
    }

    if (breakpointresume)
    {
        // 需要断点续传，此处发送 NLST
        mListCommand.append(QString("TYPE A\r\n").toUtf8());
        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("NLST\r\n").toUtf8());
    }
    else
    {
        // 不要断点续传，直接发送 STOR
        mListCommand.append(QString("TYPE I\r\n").toUtf8());
        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("STOR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
    }

    sendNextCommannd();
}

void FtpProtocol::slot_ftp_start_download_file(const QString &file, const QString &remotepath, bool breakpointresume)
{
    Q_UNUSED(breakpointresume);
    mDownloadFileFlag = true;
    mFileName = file;
    mLocalFileInfo = QFileInfo(QString("%1/%2").arg(mDownloadPath, mFileName));
    mRemoteFilePath = remotepath;
    mTotalDownloadLength = 0;
    mServerFileInfo.clear();

    // 返回到顶层文件夹( 在使用时，根据实际情况更改根目录位置 )
    mListCommand.append(QString("CWD /home/ftp_root\r\n").toUtf8());

    // 切换路径
    auto listDirName = QDir(mRemoteFilePath).path().split('/');
    for (auto &name : listDirName)
    {
        mListCommand.append(QString("CWD %1\r\n").arg(name).toUtf8());
    }

    if (breakpointresume)
    {
        mFileStream.open(mLocalFileInfo.absoluteFilePath().toLocal8Bit().toStdString(), ios::binary | ios::out | ios::app);
        if (!mFileStream.is_open())
        {
            mResultMessage = "无法创建本地文件";
            return;
        }

        mListCommand.append(QString("TYPE A\r\n").toUtf8());
        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("NLST\r\n").toUtf8());
    }
    else
    {
        mFileStream.open(mLocalFileInfo.absoluteFilePath().toLocal8Bit().toStdString(), ios::binary | ios::out | ios::trunc);
        if (!mFileStream.is_open())
        {
            mResultMessage = "无法创建本地文件";
            return;
        }
        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("TYPE I\r\n").toUtf8());
        mListCommand.append(QString("RETR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
    }

    sendNextCommannd();
}

void FtpProtocol::slot_socket_file_data_recv()
{
    QByteArray array = mSocketData->readAll();
    if (mListCommand.size() > 0)
    {
        QString cmd = mListCommand.first().left(4).trimmed();
        if (cmd == "NLST")
        {
            mServerDirInfo += array.toStdString().data();
        }
        else if (cmd == "LIST")
        {
            mServerFileInfo += array.toStdString().data();
        }
        else if (cmd == "RETR")
        {
            mTotalDownloadLength += array.size();

            float percent = mTotalDownloadLength * 1.0 / mTotalFileSize;
            emit sgl_file_download_process(mFileName, percent * 100);
            mFileStream.write(array, array.size());
        }
    }
}

void FtpProtocol::slot_socket_file_data_close()
{
    qDebug() << "FtpProtocol::slot_socket_file_data_close " << mListCommand;
    mDataRecvFlag = true;
    if (!mCommandRecvFlag) return;
    if (mListCommand.size() == 0) return;
    QString cmd = mListCommand.first().left(4).trimmed();
    if (cmd == "NLST")
    {
        parse_nlst_data_pack();
    }
    else if (cmd == "LIST")
    {
        parse_list_data_pack();
    }
    else if (cmd == "RETR")
    {
        parse_retr_data_pack();
    }
    else
    {
        mResultMessage = "数据传输功能异常";
        return close();
    }
}

void FtpProtocol::slot_socket_command_connect()
{
    emit sgl_ftp_connect_status_change(true);
}

void FtpProtocol::slot_socket_command_close()
{
    emit sgl_ftp_connect_status_change(false);

    if (mTaskStatus) return;
    mResultMessage = "服务器连接断开";
    close();
}

void FtpProtocol::slot_socket_command_data_recv()
{
    if (mListCommand.size() == 0) return;
    QByteArray datagram = mSocketCommand->readAll();
    QString str = QString::fromLocal8Bit(datagram).split("\r\n", Qt::SkipEmptyParts).last();
    QString cmd = mListCommand.first().left(4).trimmed();
    QString status = mStatusObject.value(cmd).toObject().value(str.at(0)).toString();

    qDebug() << "mResultMessage " << str << " " << status << " " << mListCommand << " " << cmd;
    if (cmd != "QUIT") mResultMessage = str;

    // 如果命令失败，立即返回
    if ((status == "E") || ((status == "F")))
    {
        // 但是如果是改变目录失败，允许直接发送新建
        if (cmd == "CWD")
        {
            QString floder = mListCommand.first().split(" ").last().remove("\r\n");
            mListCommand.insert(0, QString("MKD %1\r\n").arg(floder).toUtf8());
            sendNextCommannd();
            return;
        }
        else
        {
            qDebug() << "文件传输异常";
            return close();
        }
    }

    // 部分命令需要处理
    if (cmd == "CONE")
    {
        mListCommand.removeFirst();
        // 准备登录
        mListCommand.append(QString("USER %1\r\n").arg(mFtpUserName).toUtf8());
        mListCommand.append(QString("PASS %1\r\n").arg(mFtpUserPass).toUtf8());
    }
    else if (cmd == "PASS")
    {
        mListCommand.removeFirst();
        emit sgl_ftp_connect_status_change(status == "S");

        // 不存在其他命令，不用继续执行
        return;
    }
    else if (cmd == "PASV")
    {
        mListCommand.removeFirst();
        QStringList list = str.split(',');
        uint32_t p1 = list.at(4).toUInt() * 256;
        uint32_t p2 = QString(list.at(5)).remove(".").remove(")").toUInt();
        if (nullptr == mSocketData)
        {
            mSocketData = new QTcpSocket;
            connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_socket_file_data_recv);
            connect(mSocketData, &QTcpSocket::disconnected, this, &FtpProtocol::slot_socket_file_data_close);
        }
        if (mSocketData->state() == QTcpSocket::ConnectedState) mSocketData->close();
        mSocketData->connectToHost(QHostAddress(mFtpHost), p1 + p2);
    }
    else if ((cmd == "NLST") && (status == "W"))
    {
        // 此处需要继续等待
        return;
    }
    else if ((cmd == "NLST") && (status == "S"))
    {
        mCommandRecvFlag = true;
        if (!mDataRecvFlag) return;
        return parse_nlst_data_pack();
    }
    else if ((cmd == "LIST") && (status == "S"))
    {
        mCommandRecvFlag = true;
        if (!mDataRecvFlag) return;
        return parse_list_data_pack();
    }
    else if (((cmd == "APPE") || (cmd == "STOR")) && (status == "W"))
    {
        mFileStream.open(mFileName.toLocal8Bit().toStdString(), ios::binary | ios::in);
        if (!mFileStream.is_open())
        {
            mListCommand.clear();
            mSocketData->close();
            mResultMessage = "本地文件打开失败";
            return;
        }

        mTotalFileSize = mLocalFileInfo.size();
        if (mTotalUploadLength > mTotalFileSize)
        {
            mListCommand.clear();
            mFileStream.close();
            mSocketData->close();
            mResultMessage = "服务器文件大小异常";
            return;
        }
        else if (mTotalUploadLength == mTotalFileSize)
        {
            mListCommand.clear();
            emit sgl_ftp_upload_task_finish(mFileName, true, "文件已经存在，不再继续上传");
            mResultMessage = "文件上传完成";
            mTaskStatus = true;
            mFileStream.close();
            mSocketData->close();
            return;
        }

        // 检查网络状态
        int tryCount = 0;
        while (mSocketData->state() != QTcpSocket::ConnectedState)
        {
            tryCount++;
            if(tryCount > 5 * 15)
            {
                mResultMessage = "无法连接到服务器";
                return close();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        // 定位本地文件读取位置
        if (mTotalUploadLength > 0) mFileStream.seekg(mTotalUploadLength);
        while (mFileStream.peek() != EOF)
        {
            uint64_t subSize = 0;
            QByteArray array;
            if (mTotalFileSize <= 1460) subSize = mTotalFileSize;
            else if ((mTotalFileSize - mTotalUploadLength) <= 1460) subSize = mTotalFileSize - mTotalUploadLength;
            else subSize = 1460;

            array.resize(subSize);
            mFileStream.read(array.data(), subSize);
            // 有可能还没有成功连接
            mSocketData->write(array.data(), subSize);
            mSocketData->flush();
            mSocketData->waitForBytesWritten(-1);

            mTotalUploadLength += subSize;
            float percent = mTotalUploadLength * 1.0 / mTotalFileSize;
            emit sgl_file_upload_process(mFileName, percent * 100);
        }

        mFileStream.close();
        mSocketData->close();
        // 等待上传完成的信号就好，不用继续执行后续逻辑
        return;
    }
    else if (((cmd == "APPE") || (cmd == "STOR")) && (status == "S"))
    {
        mListCommand.removeFirst();

        mResultMessage = "文件上传完成";
        mTaskStatus = true;

        // 发送结果消息
        emit sgl_ftp_task_response(mFileName, mTaskStatus, mResultMessage);

        // 发送任务结束信号
        emit sgl_ftp_upload_task_finish(mFileName, mTaskStatus, mResultMessage);
    }
    else if ((cmd == "RETR") && (status == "W"))
    {
        // 等待就好
        return;
    }
    else if ((cmd == "RETR") && (status == "S"))
    {
        mCommandRecvFlag = true;
        if (!mDataRecvFlag) return;
        return parse_retr_data_pack();
    }
    else if ((cmd == "QUIT") && (status == "S"))
    {
        mListCommand.removeFirst();
        close();
    }
    else if ((cmd == "MKD") && (status == "S"))
    {
        mListCommand.removeFirst();
    }
    else
    {
        if (status == "W") return;
        mListCommand.removeFirst();
    }

    sendNextCommannd();
}

void FtpProtocol::close()
{
    qDebug() << "FtpProtocol::close";
    // 文件流关闭
    if (mFileStream.is_open())
    {
        mFileStream.close();
    }
    // 数据通道关闭
    if (nullptr != mSocketData)
    {
        disconnect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_socket_file_data_recv);
        disconnect(mSocketData, &QTcpSocket::disconnected, this, &FtpProtocol::slot_socket_file_data_close);
        mSocketData->close();
        mSocketData->deleteLater();
    }
    // 命令通道关闭
    if (nullptr != mSocketCommand)
    {
        disconnect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_socket_command_data_recv);
        mSocketCommand->close();
        mSocketCommand->deleteLater();
    }

    mListCommand.clear();

    // 发送结果消息
    emit sgl_ftp_task_response(mFileName, mTaskStatus, mResultMessage);
}

void FtpProtocol::sendNextCommannd()
{
    if (mListCommand.length() == 0) return;

    QString cmd = mListCommand.first();
    qDebug() << "FtpProtocol::sendNextCommannd " << cmd << " size " << mListCommand;
    mSocketCommand->write(cmd.toStdString().data());
    mSocketCommand->flush();
}

int64_t FtpProtocol::parseFileSize()
{
    auto list =  mServerFileInfo.split(' ', Qt::SkipEmptyParts);
    qDebug() << "FtpProtocol::parseFileSize " << mServerFileInfo;
    if (mServerFileInfo.startsWith('-')) // linux 下的文件标志
    {
        if (list.size() != 9)
        {
            mResultMessage = "Linux 系统文件大小解析失败";
            close();
            return -1;
        }
        else
        {
            return list.at(4).toULongLong();
        }
    }
    else
    {
        if (list.size() != 4)
        {
            mResultMessage = "Windows 系统文件大小解析失败";
            close();
            return -1 ;
        }
        else
        {
            return list.at(2).toULongLong();
        }
    }
}

void FtpProtocol::downloadFile()
{
    uint64_t length = mLocalFileInfo.size();

    // 默认已下载大小
    mTotalDownloadLength = length;

    mListCommand.append(QString("PASV\r\n").toUtf8());
    mListCommand.append(QString("TYPE I\r\n").toUtf8());

    if (length > 0)
    {
        mListCommand.append(QString("REST %1\r\n").arg(QString::number(length)).toUtf8());
    }

    mListCommand.append(QString("RETR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
    sendNextCommannd();
}

void FtpProtocol::uploadFile()
{
    mListCommand.append(QString("PASV\r\n").toUtf8());
    mListCommand.append(QString("TYPE I\r\n").toUtf8());
    if (mTotalUploadLength > 0)
    {
        mListCommand.append(QString("APPE %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
    }
    else
    {
        mListCommand.append(QString("STOR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
    }

    sendNextCommannd();
}

void FtpProtocol::parse_nlst_data_pack()
{
    qDebug() << "FtpProtocol::parse_nlst_data_pack " << mServerDirInfo << " " << mFileName;
    mCommandRecvFlag = false;
    mDataRecvFlag = false;
    mListCommand.removeFirst();
    auto list = mServerDirInfo.split("\r\n", Qt::SkipEmptyParts);

    if (!list.contains(mLocalFileInfo.fileName()))
    {
        if (mDownloadFileFlag)
        {
            mResultMessage = "文件不存在";

            // 退出登录
            return;
        }
        else if (mUploadFileFlag)
        {
            mTotalUploadLength = 0;
            uploadFile();
        }
    }
    else
    {
        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("TYPE A\r\n").toUtf8());
        mListCommand.append(QString("LIST %1\r\n").arg(mLocalFileInfo.fileName().toUtf8().data()));
        sendNextCommannd();
    }
}

void FtpProtocol::parse_list_data_pack()
{
    mCommandRecvFlag = false;
    mDataRecvFlag = false;
    mListCommand.removeFirst();
    if (mDownloadFileFlag)
    {
        mTotalFileSize = parseFileSize();
        if (mTotalFileSize < 0) return;
        downloadFile();
    }
    else if (mUploadFileFlag)
    {
        mTotalUploadLength = parseFileSize();
        if (mTotalUploadLength < 0) return;
        uploadFile();
    }
}

void FtpProtocol::parse_retr_data_pack()
{
    mCommandRecvFlag = false;
    mDataRecvFlag = false;
    mListCommand.removeFirst();
    mFileStream.close();
    if (mTotalDownloadLength != mTotalFileSize)
    {
        mResultMessage = "文件大小异常";
    }
    else
    {
        emit sgl_file_download_process(mFileName, 100.00);

        mResultMessage = "文件下载完成";
        mTaskStatus = true;
    }

    sendNextCommannd();
}

///////////////////////////////////////////////////////////////////////////////////// 分割 /////////////////////////////////////////////////////////////////////////////////////////////

FtpManager::FtpManager(const QString &host, const QString &user, const QString &pass, QObject *parent)
    : QObject{parent}, mFtpProtocol(host, user, pass)
{

}

FtpManager::~FtpManager()
{
    mFtpWorkThread.exit(0);
    mFtpWorkThread.deleteLater();
}

void FtpManager::init()
{
    mFtpProtocol.moveToThread(&mFtpWorkThread);
    connect(&mFtpProtocol, &FtpProtocol::sgl_file_upload_process, this, &FtpManager::sgl_file_upload_process, Qt::QueuedConnection);
    connect(&mFtpProtocol, &FtpProtocol::sgl_file_download_process, this, &FtpManager::sgl_file_download_process, Qt::QueuedConnection);
    connect(&mFtpProtocol, &FtpProtocol::sgl_ftp_upload_task_finish, this, &FtpManager::sgl_ftp_upload_task_finish, Qt::QueuedConnection);

    connect(&mFtpProtocol, &FtpProtocol::sgl_ftp_connect_status_change, this, &FtpManager::slot_ftp_connect_status_change, Qt::QueuedConnection);
    connect(this, &FtpManager::sgl_start_connect_ftp_server, &mFtpProtocol, &FtpProtocol::slot_start_connect_ftp_server, Qt::QueuedConnection);
    connect(this, &FtpManager::sgl_ftp_start_upload_file, &mFtpProtocol, &FtpProtocol::slot_ftp_start_upload_file, Qt::QueuedConnection);
    connect(this, &FtpManager::sgl_ftp_start_download_file, &mFtpProtocol, &FtpProtocol::slot_ftp_start_download_file, Qt::QueuedConnection);

    mFtpWorkThread.start();

    // 连接文件服务器
    emit sgl_start_connect_ftp_server();
}

void FtpManager::downloadFile(const QString &file, const QString &remotePath)
{
    emit sgl_ftp_start_download_file(file, remotePath, mOpenBreakPointResume);
}

void FtpManager::uploadFile(const QString &file, const QString &remotePath)
{
    if (!QFile::exists(file))
    {
        emit sgl_ftp_work_status_message("文件不存在，请检查文件位置");
        return;
    }

    if (!mFtpConnected)
    {
        emit sgl_ftp_work_status_message("未能连接到文件服务，请重新连接");
        return;
    }

    emit sgl_ftp_start_upload_file(file, remotePath, mOpenBreakPointResume);
}

void FtpManager::slot_ftp_connect_status_change(bool status)
{
    // 记录文件服务状态
    mFtpConnected = status;
    emit sgl_ftp_connect_status_change(status);
}

bool FtpManager::getOpenBreakPointResume() const
{
    return mOpenBreakPointResume;
}

void FtpManager::setOpenBreakPointResume(bool newOpenBreakPointResume)
{
    mOpenBreakPointResume = newOpenBreakPointResume;
}

const QString &FtpManager::getDownloadPath() const
{
    return mDownloadPath;
}

void FtpManager::setDownloadPath(const QString &newDownloadPath)
{
    mDownloadPath = newDownloadPath;
}
