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
#include <QTimer>

//////
/// 测试版本号 Qt Version 6.x.x
///
/// FTP 属于应用层协议，通过向安装了 FTP 服务（vsftp）的服务器的指定端口发送命令
/// 断点续传功能：https://www.cnblogs.com/duanxz/p/5127105.html
/// 命令解析说明：https://www.cnblogs.com/hongyuyingxiao/p/10486036.html

using namespace std;

class FtpProtocol : public QObject
{
    Q_OBJECT
public:
    FtpProtocol(const QString &host, const QString &file, const QString &downloadpath) : mFtpHost(host), mFileName(file), mDownloadPath(downloadpath)
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
    void sgl_file_task_finish(const QString &file, bool status, const QString &msg);

public slots:
    void slot_start_file_download()
    {
        mSocketCommand = new QTcpSocket(this);
        connect(mSocketCommand, &QTcpSocket::errorOccurred, this, &FtpProtocol::slot_socket_error_occurred);
        connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_download_command_result);
        mSocketCommand->connectToHost(QHostAddress(mFtpHost), 21);
    }

    void slot_start_file_upload()
    {
        mSocketCommand = new QTcpSocket(this);
        connect(mSocketCommand, &QTcpSocket::errorOccurred, this, &FtpProtocol::slot_socket_error_occurred);
        connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_upload_command_result);
        mSocketCommand->connectToHost(QHostAddress(mFtpHost), 21);
    }

private slots:
    void slot_socket_error_occurred(QAbstractSocket::SocketError)
    {
        // 服务连接失败
        mResultMessage = "服务器连接异常";
        mTaskStatus = false;
        clear();
    }

    void slot_recv_download_command_result()
    {
        QByteArray datagram = mSocketCommand->readAll();
        QString str = QString::fromLocal8Bit(datagram).split("\r\n", Qt::SkipEmptyParts).last();

        // 记录消息结果
        mResultMessage = str;
        if (mListCommand.size() == 0) return;

        QString cmd = mListCommand.first().left(4).trimmed();
        QString status = mStatusObject.value(cmd).toObject().value(str.at(0)).toString();

        // 如果命令失败，立即返回
        if ((status == "E") || ((status == "F")))
        {
            clear();
            mTaskStatus = false;
            return;
        }

        // 部分命令需要处理
        if (cmd == "CONE")
        {
            if ((status == "S") || (status == "V"))
            {
                mListCommand.remove(0, 1);
                // 准备登录
                mListCommand.append(QString("USER %1\r\n").arg(mFtpUserName).toLatin1());
                mListCommand.append(QString("PASS %1\r\n").arg(mFtpUserPass).toLatin1());
                mListCommand.append(QString("PASV\r\n").toLatin1());
                mListCommand.append(QString("TYPE A\r\n").toLatin1());
                mListCommand.append(QString("LIST %1\r\n").arg(mFileName).toLatin1());
            }
        }
        else if (cmd == "PASV")
        {
            mListCommand.remove(0, 1);
            QStringList list = str.split(',');
            uint32_t p1 = list.at(4).toUInt() * 256;
            uint32_t p2 = QString(list.at(5)).remove(".").remove(")").toUInt();
            mSocketData = new QTcpSocket;
            mSocketData->connectToHost(QHostAddress(mFtpHost), p1 + p2);
            connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_file_data_recv);
        }
        else if (cmd == "LIST" && (status == "S"))
        {
            // 由于是在单个线程中存在两个网络套接字，传输的最后一帧和传输完毕的结果相隔很近
            // 命令触发的情况下，数据通道不一定被触发了，等待一段时间
            uint16_t delay = (mServerFileInfo.length() == 0) ? 500 : 0;
            QTimer::singleShot(delay, this, [this]
            {
                if (mServerFileInfo.size() == 0)
                {
                    mResultMessage = "文件不存在或无法获取其大小";
                    mTaskStatus = false;
                    return clear();
                }

                mListCommand.remove(0, 1);
                parseFileSize();
                downloadFile();;
            });
            return;
        }
        else if (cmd == "RETR" && (status == "S"))
        {
            uint16_t delay = (mTotalDownloadLength != mTotalFileSize) ? 500 : 0;
            QTimer::singleShot(delay, this, [this]
            {
                mFileStream.close();
                mListCommand.remove(0, 1);
                if (mTotalDownloadLength != mTotalFileSize)
                {
                    mResultMessage = "文件大小异常";
                    mTaskStatus = false;
                    return clear();
                }
            });
            return;
        }
        else
        {
            if (mListCommand.size() > 0 && ((status == "S") || (status == "V")))
            {
                mListCommand.remove(0, 1);
            }
            else
            {
                return;
            }
        }

        sentCommand();
    }

    void slot_recv_upload_command_result()
    {
        while (mSocketCommand->bytesAvailable())
        {
            QByteArray datagram = mSocketCommand->readAll();
            QString str = QString::fromLocal8Bit(datagram);
            // 记录消息结果
            mResultMessage = str;
//            if (mLoginStep == 0)
//            {
//                if (!str.contains("220")) return clear();
//                mLoginStep++;
//                sentCmdUser();
//            }
//            else if (mLoginStep == 1)
//            {
//                if (!str.contains("331")) return clear();
//                mLoginStep++;
//                sentCmdPass();
//            }
//            else if (mLoginStep == 2)
//            {
//                if (!str.contains("230"))  return clear();
//                mLoginStep++;
//                sentCmdPasv();
//            }
//            else if (mLoginStep == 3)
//            {
//                if (!str.contains("227"))  return clear();
//                QStringList list = str.split(',');
//                uint32_t p1 = list.at(4).toUInt() * 256;
//                uint32_t p2 = QString(list.at(5)).remove(".").remove(")").toUInt();
//                mSocketData = new QTcpSocket;
//                mSocketData->connectToHost("192.168.1.103", p1 + p2);
//                connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_file_data_recv);

//                mLoginStep++;
//                sentCmdCwd("upload");
//            }
//            else if (mLoginStep == 4)
//            {
//                if (!str.contains("250"))  return clear();
//                mLoginStep++;
//                sentCmdStor();
//            }
//            else if (str.contains("226"))
//            {
//                mResultMessage = "成功";
//                mTaskStatus = true;
//                clear();
//            }
//            else if (str.contains("150"))
//            {
//                QFileInfo info(mFileName);
//                mTotalFileSize = info.size();
//                mFileStream.open(mFileName.toLocal8Bit().toStdString(), ios::binary | ios::in);
//                if (!mFileStream.is_open()) return clear();
//                uint64_t sentSize = 0;
//                while (mFileStream.peek() != EOF)
//                {
//                    uint64_t subSize = 0;
//                    QByteArray array;
//                    if (mTotalFileSize <= 1460) subSize = mTotalFileSize;
//                    else if ((mTotalFileSize - sentSize) <= 1460) subSize = mTotalFileSize - sentSize;
//                    else subSize = 1460;
//                    array.resize(subSize);
//                    sentSize += subSize;
//                    mFileStream.read(array.data(), subSize);
//                    mSocketData->write(array, array.length());
//                    mSocketData->flush();
//                }

//                mResultMessage = "文件上传完成";
//                mTaskStatus = true;
//                clear();
//            }
//            else
//            {
//                clear();
//            }
        }
    }

    void slot_file_data_recv()
    {
        // windows 03-01-22  10:57PM                   65 readme.txt
        QByteArray array = mSocketData->readAll();
        if (mListCommand.size() > 0)
        {
            QString cmd = mListCommand.first().left(4).trimmed();
            if (cmd == "LIST")
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

private:
    void clear()
    {
        // 文件流关闭
        if (mFileStream.is_open())
        {
            mFileStream.close();
        }
        // 数据通道关闭
        if (nullptr != mSocketData)
        {
            disconnect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_file_data_recv);
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

        // 发送结果消息
        emit sgl_file_task_finish(mFileName, mTaskStatus, mResultMessage);
        this->deleteLater();
    }

    void sentCommand()
    {
        if (mListCommand.length() > 0)
        {
            qDebug() << "send " << mListCommand.first().toStdString().data();
            mSocketCommand->write(mListCommand.first().toStdString().data());
        }
    }

    void parseFileSize()
    {
        auto list =  mServerFileInfo.split(' ', Qt::SkipEmptyParts);
        if (mServerFileInfo.startsWith('-')) // linux 下的文件标志
        {
            if (list.size() != 9)
            {
                mResultMessage = "Linux 系统文件大小解析失败";
                mTaskStatus = false;
                clear();
                return;
            }
            else
            {
                mTotalFileSize = list.at(4).toULongLong();
            }
        }
        else
        {
            if (list.size() != 4)
            {
                mResultMessage = "Windows 系统文件大小解析失败";
                mTaskStatus = false;
                clear();
                return;
            }
            else
            {
                mTotalFileSize = list.at(2).toULongLong();
            }
        }
    }

    void downloadFile()
    {
        mFileStream.open(QString("%1/%2").arg(mDownloadPath, mFileName).toLocal8Bit().toStdString(), ios::binary | ios::out);
        if (!mFileStream.is_open())
        {
            mResultMessage = "无法创建本地文件";
            mTaskStatus = false;
            return clear();
        }

        mListCommand.append(QString("PASV\r\n").toLatin1());
        mListCommand.append(QString("TYPE I\r\n").toLatin1());
        mListCommand.append(QString("RETR %1\r\n").arg(mFileName).toLatin1());
        sentCommand();
    }
private:
    // 服务地址
    QString mFtpHost;
    // 待操作的文件名称
    QString mFileName;
    // 等待执行的操作
    QStringList mListCommand;
    // 命令通道
    QTcpSocket *mSocketCommand = nullptr;
    // 数据通道
    QTcpSocket *mSocketData = nullptr;
    // 文件流
    fstream mFileStream;
    // 服务器文件描述
    QString mServerFileInfo;
    // 文件总大小
    uint64_t mTotalFileSize = 0;
    // 文件已下载大小
    uint64_t mTotalDownloadLength = 0;
    // 文件已上传大小
    uint64_t mTotalUploadLength = 0;
    // 结果消息
    QString mResultMessage;
    // 任务状态
    bool mTaskStatus = false;
    // 下载路径
    QString mDownloadPath;
    // 用户名 (默认匿名)
    QString mFtpUserName = "anonymous";
    // 密码
    QString mFtpUserPass = "@";

    // 状态码描述
    QJsonObject mStatusObject;
};

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = nullptr);

    void downloadFile(const QString &file);
    void uploadFile(const QString &file);

    const QString &getFtpHost() const;
    void setFtpHost(const QString &newFtpHost);

    const QString &getDownloadPath() const;
    void setDownloadPath(const QString &newDownloadPath);

signals:
    void sgl_file_download_process(const QString &file, float percent);
    void sgl_file_upload_process(const QString &file, float percent);
    void sgl_file_task_finish(const QString &file, bool status, const QString &msg);

private slots:
    void slot_file_task_finish(const QString &file, bool status, const QString &msg);

private:
    QMap<QString, QThread*> mMapThread;

    QString mFtpHost;

    QString mDownloadPath = "./";
};

#endif // FTPMANAGER_H
