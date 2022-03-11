#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <thread>
#include <fstream>
#include <regex>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <QStringList>
#include <QDir>

//////
/// 测试版本号 Qt Version 6.x.x
///
/// FTP 属于应用层协议，通过向安装了 FTP 服务（vsftp）的服务器的指定端口发送命令
/// 断点续传功能：https://www.cnblogs.com/duanxz/p/5127105.html
/// 命令解析说明：https://www.cnblogs.com/hongyuyingxiao/p/10486036.html
/// 默认采用 UTF-8 编码格式，请确认文件服务器系统编码格式为 UTF-8 编码格式，或者两者都采用其他一致的格式
/// 保证不会因为编码格式产生找不到文件或路径的问题
/// 修改 Windows 默认编码（GBK）为 UTF-8 ： https://blog.csdn.net/lee_ham/article/details/82634411

using namespace std;

class FtpProtocol : public QObject
{
    Q_OBJECT
public:
    FtpProtocol(const QString &host, const QString &user, const QString &pass, const QString &remotepath, const QString &file, const QString &downloadpath) : mFtpHost(host), mFtpUserName(user), mFtpUserPass(pass), mRemoteFilePath(remotepath), mFileName(file), mDownloadPath(downloadpath)
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
        mListCommand.append("CONE"); //Connection Establishment
    }

signals:
    void sgl_file_download_process(const QString &file, float percent);
    void sgl_file_upload_process(const QString &file, float percent);
    void sgl_ftp_task_response(const QString &file, bool status, const QString &msg);

public slots:
    void slot_start_file_download()
    {
        mDownloadFileFlag = true;
        mLocalFileInfo = QFileInfo(QString("%1/%2").arg(mDownloadPath, mFileName));
        mSocketCommand = new QTcpSocket(this);
        connect(mSocketCommand, &QTcpSocket::disconnected, this, &FtpProtocol::slot_command_socket_closed);
        connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_download_command_result);
        mSocketCommand->connectToHost(QHostAddress(mFtpHost), 21);
    }

    void slot_start_file_upload()
    {
        mUploadFileFlag = true;
        mLocalFileInfo = QFileInfo(mFileName);
        mSocketCommand = new QTcpSocket(this);
        connect(mSocketCommand, &QTcpSocket::disconnected, this, &FtpProtocol::slot_command_socket_closed);
        connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_upload_command_result);
        mSocketCommand->connectToHost(QHostAddress(mFtpHost), 21);
    }

private slots:

    void slot_recv_download_command_result()
    {
        if (mListCommand.size() == 0) return;
        QByteArray datagram = mSocketCommand->readAll();
        QString str = QString::fromLocal8Bit(datagram).split("\r\n", Qt::SkipEmptyParts).last();
        QString cmd = mListCommand.first().left(4).trimmed();
        QString status = mStatusObject.value(cmd).toObject().value(str.at(0)).toString();

        qDebug() << "mResultMessage " << str << " " << status << " " << mListCommand;

        if (cmd != "QUIT") mResultMessage = str;

        // 如果命令失败，立即返回
        if ((status == "E") || ((status == "F")))
        {
            return clear();
        }

        // 部分命令需要处理
        if (cmd == "CONE")
        {
            if ((status == "S") || (status == "V"))
            {
                mListCommand.removeFirst();
                // 准备登录
                mListCommand.append(QString("USER %1\r\n").arg(mFtpUserName).toUtf8());
                mListCommand.append(QString("PASS %1\r\n").arg(mFtpUserPass).toUtf8());
                mListCommand.append(QString("PASV\r\n").toUtf8());
                mListCommand.append(QString("TYPE A\r\n").toUtf8());

                // 切换路径
                auto listDirName = QDir(mRemoteFilePath).path().split('/');
                for (auto &name : listDirName)
                {
                    mListCommand.append(QString("CWD %1\r\n").arg(name.toUtf8()).toUtf8());
                }

                mListCommand.append(QString("NLST\r\n").toUtf8());
            }
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
                connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_data_socket_recv_data);
                connect(mSocketData, &QTcpSocket::disconnected, this, &FtpProtocol::slot_data_socket_closed);
            }
            mSocketData->close();
            mSocketData->connectToHost(QHostAddress(mFtpHost), p1 + p2);
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
        else if ((cmd == "RETR") && (status == "S"))
        {
            mCommandRecvFlag = true;
            if (!mDataRecvFlag) return;
            return parse_retr_data_pack();
        }
        else if ((cmd == "QUIT") && (status == "S"))
        {
            mListCommand.removeFirst();

            mResultMessage = "文件下载完成，退出登录";
            mTaskStatus = true;
            clear();
        }
        else
        {
            if (status == "W") return;
            mListCommand.removeFirst();
        }

        sendNextCommannd();
    }

    void slot_recv_upload_command_result()
    {
        if (mListCommand.size() == 0) return;
        QByteArray datagram = mSocketCommand->readAll();
        QString str = QString::fromLocal8Bit(datagram).split("\r\n", Qt::SkipEmptyParts).last();
        QString cmd = mListCommand.first().left(4).trimmed();
        QString status = mStatusObject.value(cmd).toObject().value(str.at(0)).toString();

        qDebug() << "mResultMessage " << str << " " << status << " " << mListCommand;
        if (cmd != "QUIT") mResultMessage = str;

        // 如果命令失败，立即返回
        if ((status == "E") || ((status == "F")))
        {
            return clear();
        }

        // 部分命令需要处理
        if (cmd == "CONE")
        {
            mListCommand.removeFirst();
            // 准备登录
            mListCommand.append(QString("USER %1\r\n").arg(mFtpUserName).toUtf8());
            mListCommand.append(QString("PASS %1\r\n").arg(mFtpUserPass).toUtf8());
            mListCommand.append(QString("PASV\r\n").toUtf8());
            mListCommand.append(QString("TYPE A\r\n").toUtf8());

            // 切换路径
            auto listDirName = QDir(mRemoteFilePath).path().split('/');
            for (auto &name : listDirName)
            {
                mListCommand.append(QString("CWD %1\r\n").arg(name.toUtf8()).toUtf8());
            }
            mListCommand.append(QString("NLST\r\n").toUtf8());
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
                connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_data_socket_recv_data);
                connect(mSocketData, &QTcpSocket::disconnected, this, &FtpProtocol::slot_data_socket_closed);
            }
            mSocketData->close();
            mSocketData->connectToHost(QHostAddress(mFtpHost), p1 + p2);
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
            mListCommand.removeFirst();
            mFileStream.open(mFileName.toLocal8Bit().toStdString(), ios::binary | ios::in);
            if (!mFileStream.is_open())
            {
                mResultMessage = "本地文件打开失败";
                mListCommand.append(QString("QUIT\r\n").toUtf8());
            }

            mTotalFileSize = mLocalFileInfo.size();
            if (mTotalUploadLength > mTotalFileSize)
            {
                mResultMessage = "服务器文件大小异常";
                mListCommand.append(QString("QUIT\r\n").toUtf8());
            }
            else if (mTotalUploadLength == mTotalFileSize)
            {
                emit sgl_file_upload_process(mFileName, 100);
                mResultMessage = "文件上传完成";
                mTaskStatus = true;

                mListCommand.append(QString("QUIT\r\n").toUtf8());
            }

            // 检查网络状态
            int tryCount = 0;
            while (mSocketData->state() != QTcpSocket::ConnectedState)
            {
                tryCount++;
                if(tryCount > 5 * 15)
                {
                    mResultMessage = "无法连接到服务器";
                    return clear();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            // 定位本地文件读取位置
            mFileStream.seekg(mTotalUploadLength);
            qDebug() << "mTotalUploadLength " << mTotalUploadLength;
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

            mSocketData->close();

            mTaskStatus = true;
            // 退出登录
            mListCommand.append(QString("QUIT\r\n").toUtf8());
        }
        else if ((cmd == "QUIT") && (status == "S"))
        {
            mListCommand.removeFirst();

            mResultMessage = "文件上传完成，退出登录";
            mTaskStatus = true;
            clear();
        }
        else
        {
            if (status == "W") return;
            mListCommand.removeFirst();
        }

        sendNextCommannd();
    }

    void slot_data_socket_recv_data()
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

    void slot_data_socket_closed()
    {
        qDebug() << "socket close " << mListCommand;
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
            return clear();
        }
    }

    void slot_command_socket_closed()
    {
        if (mTaskStatus) return;
        // 服务连接失败
        mResultMessage = "服务器连接断开";
        clear();
    }

private:

    void clear()
    {
        qDebug() << "stop ftp protocol";
        // 文件流关闭
        if (mFileStream.is_open())
        {
            mFileStream.close();
        }
        // 数据通道关闭
        if (nullptr != mSocketData)
        {
            disconnect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_data_socket_recv_data);
            disconnect(mSocketData, &QTcpSocket::disconnected, this, &FtpProtocol::slot_data_socket_closed);
            mSocketData->close();
            mSocketData->deleteLater();
        }
        // 命令通道关闭
        if (nullptr != mSocketCommand)
        {
            disconnect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_download_command_result);
            disconnect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_upload_command_result);
            mSocketCommand->close();
            mSocketCommand->deleteLater();
        }

        mListCommand.clear();

        // 发送结果消息
        emit sgl_ftp_task_response(mFileName, mTaskStatus, mResultMessage);
        this->deleteLater();
    }

    void sendNextCommannd()
    {
        if (mListCommand.length() > 0)
        {
            QString cmd = mListCommand.first();
            qDebug() << "send " << cmd << " size " << mListCommand;
            mSocketCommand->write(cmd.toStdString().data());
            mSocketCommand->flush();
        }
    }

    int64_t parseFileSize()
    {
        auto list =  mServerFileInfo.split(' ', Qt::SkipEmptyParts);
        qDebug() << "mServerFileInfo " << mServerFileInfo;
        if (mServerFileInfo.startsWith('-')) // linux 下的文件标志
        {
            if (list.size() != 9)
            {
                mResultMessage = "Linux 系统文件大小解析失败";
                clear();
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
                clear();
                return -1 ;
            }
            else
            {
                return list.at(2).toULongLong();
            }
        }
    }

    void downloadFile()
    {
        QString filePath = QString("%1/%2").arg(mDownloadPath, mFileName);
        uint64_t length = mLocalFileInfo.size();
        mTotalDownloadLength = length; // 默认已下载大小

        qDebug() << "filePath " << filePath;
        mFileStream.open(filePath.toLocal8Bit().toStdString(), ios::binary | ios::out | ((length > 0) ? ios::app : ios::trunc));
        if (!mFileStream.is_open())
        {
            mResultMessage = "无法创建本地文件";
            return clear();
        }

        mListCommand.append(QString("PASV\r\n").toUtf8());
        mListCommand.append(QString("TYPE I\r\n").toUtf8());

        if (length > 0)
        {
            mListCommand.append(QString("REST %1\r\n").arg(QString::number(length)).toUtf8());
        }

        mListCommand.append(QString("RETR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
        sendNextCommannd();
    }

    void uploadFile()
    {
        if (mTotalUploadLength > 0)
        {
            mListCommand.append(QString("PASV\r\n").toUtf8());
            mListCommand.append(QString("TYPE I\r\n").toUtf8());
            mListCommand.append(QString("APPE %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
        }
        else
        {
            mListCommand.append(QString("PASV\r\n").toUtf8());
            mListCommand.append(QString("TYPE I\r\n").toUtf8());
            mListCommand.append(QString("STOR %1\r\n").arg(mLocalFileInfo.fileName()).toUtf8());
        }

        sendNextCommannd();
    }

    // 解析 NLST 回复数据
    void parse_nlst_data_pack()
    {
        qDebug() << "parse nlst " << mServerDirInfo << " " << mFileName;
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
                mListCommand.append(QString("QUIT\r\n").toUtf8());
                sendNextCommannd();
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

    // 解析 LIST 回复数据
    void parse_list_data_pack()
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

    // 处理 RETR 回复后逻辑
    void parse_retr_data_pack()
    {
        mCommandRecvFlag = false;
        mDataRecvFlag = false;
        mListCommand.removeFirst();
        mFileStream.close();
        if (mTotalDownloadLength != mTotalFileSize)
        {
            mResultMessage = "文件大小异常";
            return clear();
        }

        emit sgl_file_download_process(mFileName, 100.00);

        mTaskStatus = true;
        // 退出登录
        mListCommand.append(QString("QUIT\r\n").toUtf8());
        sendNextCommannd();
    }

private:
    // 服务地址
    QString mFtpHost;
    // 用户名 (默认匿名)
    QString mFtpUserName;
    // 密码
    QString mFtpUserPass;
    // 服务端文件路径
    QString mRemoteFilePath;
    // 待操作的文件名称
    QString mFileName;
    // 等待执行的操作
    QStringList mListCommand;
    // 命令通道
    QTcpSocket *mSocketCommand = nullptr;
    // 数据通道
    QTcpSocket *mSocketData = nullptr;
    // 本地文件信息
    QFileInfo mLocalFileInfo;
    // 文件流
    fstream mFileStream;
    // 服务器目录描述
    QString mServerDirInfo;
    // 服务器文件描述
    QString mServerFileInfo;
    // 文件总大小
    int64_t mTotalFileSize = 0;
    // 文件已下载大小
    int64_t mTotalDownloadLength = 0;
    // 文件已上传大小
    int64_t mTotalUploadLength = 0;
    // 结果消息
    QString mResultMessage;
    // 任务状态
    bool mTaskStatus = false;
    // 命令是否回复
    bool mCommandRecvFlag = false;
    // 数据是否回复
    bool mDataRecvFlag = false;
    // 是否是下载任务
    bool mDownloadFileFlag = false;
    // 是否是上传任务
    bool mUploadFileFlag = false;
    // 下载路径
    QString mDownloadPath;
    // 状态码描述
    QJsonObject mStatusObject;
};

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = nullptr);

    void downloadFile(const QString &file, const QString &remotePath = "");
    void uploadFile(const QString &file, const QString &remotePath = "");

    const QString &getFtpHost() const;
    void setFtpHost(const QString &newFtpHost);

    const QString &getDownloadPath() const;
    void setDownloadPath(const QString &newDownloadPath);

    const QString &getFtpUserPass() const;
    void setFtpUserPass(const QString &newFtpUserPass);

    const QString &getFtpUserName() const;
    void setFtpUserName(const QString &newFtpUserName);

signals:
    void sgl_file_download_process(const QString &file, float percent);
    void sgl_file_upload_process(const QString &file, float percent);
    void sgl_ftp_task_response(const QString &file, bool status, const QString &msg);

private slots:
    void slot_ftp_task_response(const QString &file, bool status, const QString &msg);

private:
    QMap<QString, QThread*> mMapThread;

    QString mFtpHost;

    QString mDownloadPath = "./";

    // 登录用户名
    QString mFtpUserName = "anonymous";
    // 登录密码
    QString mFtpUserPass = "@";
};

#endif // FTPMANAGER_H
