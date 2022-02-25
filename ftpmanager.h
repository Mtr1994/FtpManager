#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <thread>
#include <fstream>
#include <regex>
#include <QFileInfo>

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
    FtpProtocol(const QString &host, const QString &file, const QString &downloadpath) : mFtpHost(host), mFileName(file), mDownloadPath(downloadpath) { }

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
        mSocketCommand->connectToHost("192.168.1.103", 21);
    }

    void slot_start_file_upload()
    {
        mSocketCommand = new QTcpSocket(this);
        connect(mSocketCommand, &QTcpSocket::errorOccurred, this, &FtpProtocol::slot_socket_error_occurred);
        connect(mSocketCommand, &QTcpSocket::readyRead, this, &FtpProtocol::slot_recv_upload_command_result);
        mSocketCommand->connectToHost("192.168.1.103", 21);
    }

private slots:
    void slot_socket_error_occurred(QAbstractSocket::SocketError)
    {
        // 服务连接失败
    }

    void slot_recv_download_command_result()
    {
        while (mSocketCommand->bytesAvailable())
        {
            QByteArray datagram = mSocketCommand->readAll();
            QString str = QString::fromLocal8Bit(datagram);
            // 记录消息结果
            mResultMessage = str;
            if (mLoginStep == 0)
            {
                if (!str.contains("220")) return clear();
                mLoginStep++;
                sentCmdUser();
            }
            else if (mLoginStep == 1)
            {
                if (!str.contains("331")) return clear();
                mLoginStep++;
                sentCmdPass();
            }
            else if (mLoginStep == 2)
            {
                if (!str.contains("230"))  return clear();
                mLoginStep++;
                sentCmdPasv();
            }
            else if (mLoginStep == 3)
            {
                if (!str.contains("227")) return clear();
                QStringList list = str.split(',');
                uint32_t p1 = list.at(4).toUInt() * 256;
                uint32_t p2 = QString(list.at(5)).remove(".").remove(")").toUInt();
                mSocketData = new QTcpSocket;
                mSocketData->connectToHost("192.168.1.103", p1 + p2);
                connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_file_data_recv);

                mLoginStep++;
                sentCmdRetr();
            }
            else if (str.contains("150"))
            {
                std::string pattern("(.*)?\\((.*) bytes\\)(.*)?");
                std::regex reg(pattern);
                smatch result;
                std::string msg = str.toStdString();
                bool flag = regex_search(msg, result, reg);
                if (!flag || (result.size() != 4)) return clear();
                mTotalFileSize =  QString(result.str(2).data()).toULong();

                mFileStream.open(QString("%1/%2").arg(mDownloadPath, mFileName).toLocal8Bit().toStdString(), ios::binary | ios::out);
                if (!mFileStream.is_open())
                {
                    mResultMessage = "无法创建本地文件";
                    return clear();
                }
            }
            else if (str.contains("226"))
            {
                mResultMessage = "文件下载完成";
                mTaskStatus = true;
                clear();
            }
            else
            {
                clear();
            }
        }
    }

    void slot_recv_upload_command_result()
    {
        while (mSocketCommand->bytesAvailable())
        {
            QByteArray datagram = mSocketCommand->readAll();
            QString str = QString::fromLocal8Bit(datagram);
            // 记录消息结果
            mResultMessage = str;
            if (mLoginStep == 0)
            {
                if (!str.contains("220")) return clear();
                mLoginStep++;
                sentCmdUser();
            }
            else if (mLoginStep == 1)
            {
                if (!str.contains("331")) return clear();
                mLoginStep++;
                sentCmdPass();
            }
            else if (mLoginStep == 2)
            {
                if (!str.contains("230"))  return clear();
                mLoginStep++;
                sentCmdPasv();
            }
            else if (mLoginStep == 3)
            {
                if (!str.contains("227"))  return clear();
                QStringList list = str.split(',');
                uint32_t p1 = list.at(4).toUInt() * 256;
                uint32_t p2 = QString(list.at(5)).remove(".").remove(")").toUInt();
                mSocketData = new QTcpSocket;
                mSocketData->connectToHost("192.168.1.103", p1 + p2);
                connect(mSocketData, &QTcpSocket::readyRead, this, &FtpProtocol::slot_file_data_recv);

                mLoginStep++;
                sentCmdCwd("upload");
            }
            else if (mLoginStep == 4)
            {
                if (!str.contains("250"))  return clear();
                mLoginStep++;
                sentCmdStor();
            }
            else if (str.contains("226"))
            {
                mResultMessage = "成功";
                mTaskStatus = true;
                clear();
            }
            else if (str.contains("150"))
            {
                QFileInfo info(mFileName);
                mTotalFileSize = info.size();
                mFileStream.open(mFileName.toLocal8Bit().toStdString(), ios::binary | ios::in);
                if (!mFileStream.is_open()) return clear();
                uint64_t sentSize = 0;
                while (mFileStream.peek() != EOF)
                {
                    uint64_t subSize = 0;
                    QByteArray array;
                    if (mTotalFileSize <= 1460) subSize = mTotalFileSize;
                    else if ((mTotalFileSize - sentSize) <= 1460) subSize = mTotalFileSize - sentSize;
                    else subSize = 1460;
                    array.resize(subSize);
                    sentSize += subSize;
                    mFileStream.read(array.data(), subSize);
                    mSocketData->write(array, array.length());
                    mSocketData->flush();
                }

                mResultMessage = "文件上传完成";
                mTaskStatus = true;
                clear();
            }
            else
            {
                clear();
            }
        }
    }

    void slot_file_data_recv()
    {
        QByteArray array = mSocketData->readAll();
        mTotalDownloadLength += array.size();

        float percent = mTotalDownloadLength * 1.0 / mTotalFileSize;
        emit sgl_file_download_process(mFileName, percent * 100);
        mFileStream.write(array, array.size());
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

    void sentCmdUser()
    {
        QByteArray command = QString("USER anonymous").toLatin1();
        command += '\n';
        mSocketCommand->write(command);
    }

    void sentCmdPass()
    {
        QByteArray command =QString("PASS").toLatin1();
        command += '\n';
        mSocketCommand->write(command);
    }

    void sentCmdPasv()
    {
        QByteArray command =QString("PASV").toLatin1();
        command += '\n';
        mSocketCommand->write(command);
    }

    void sentCmdCwd(const string &path)
    {
        QByteArray command =QString("CWD %1").arg(path.data()).toLatin1();
        command += '\n';
        mSocketCommand->write(command);
    }

    void sentCmdRetr()
    {
        QString fileName = QString("RETR %1").arg(mFileName);
        QByteArray command = QByteArray(fileName.toUtf8());
        command += '\n';
        mSocketCommand->write(command);
    }

    void sentCmdStor()
    {
        // 如果文件已存在，返回 553 错误
        QString fileName = QString("STOR %1").arg(mFileName);
        QByteArray command = QByteArray(fileName.toUtf8());
        command += '\n';
        mSocketCommand->write(command);
    }

private:
    // 服务地址
    QString mFtpHost;
    // 代操作的文件名称
    QString mFileName;
    uint32_t mLoginStep = 0;
    QTcpSocket *mSocketCommand = nullptr;
    QTcpSocket *mSocketData = nullptr;
    fstream mFileStream;
    uint64_t mTotalFileSize = 0;
    uint64_t mTotalDownloadLength = 0;
    QString mResultMessage;
    bool mTaskStatus = false;
    QString mDownloadPath;
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
