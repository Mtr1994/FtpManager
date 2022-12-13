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
#include <QThread>

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
    FtpProtocol(const QString &host, const QString &user, const QString &pass);

signals:
    void sgl_file_download_process(const QString &file, float percent);
    void sgl_file_upload_process(const QString &file, float percent);
    void sgl_ftp_task_response(const QString &file, bool status, const QString &msg);

    // 文件传输结束
    void sgl_ftp_upload_task_finish(const QString &file, bool status, const QString &message);

    // 文件服务登录状态改变
    void sgl_ftp_connect_status_change(bool status);

public slots:
    // 连接文件服务器
    void slot_start_connect_ftp_server();

    // 开始上传文件
    void slot_ftp_start_upload_file(const QString &file, const QString &remotepath, bool breakpointresume);

    // 开始下载文件
    void slot_ftp_start_download_file(const QString &file, const QString &remotepath, bool breakpointresume);

private slots:
    // 数据通道数据处理
    void slot_socket_file_data_recv();

    // 数据通道关闭
    void slot_socket_file_data_close();

    // 文件服务连接
    void slot_socket_command_connect();

    // 文件服务关闭
    void slot_socket_command_close();

    // 命令通道数据处理
    void slot_socket_command_data_recv();

private:
    void close();

    void sendNextCommannd();

    int64_t parseFileSize();

    void downloadFile();

    void uploadFile();

    // 解析 NLST 回复数据
    void parse_nlst_data_pack();

    // 解析 LIST 回复数据
    void parse_list_data_pack();

    // 处理 RETR 回复后逻辑
    void parse_retr_data_pack();

private:
    // 服务地址
    QString mFtpHost;
    // 用户名 (默认匿名)
    QString mFtpUserName = "anonymous";
    // 密码
    QString mFtpUserPass = "@";
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
    // 下载路径 (默认当前目录)
    QString mDownloadPath = ".";
    // 状态码描述
    QJsonObject mStatusObject;
};

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(const QString &host, const QString &user, const QString &pass, QObject *parent = nullptr);
    ~FtpManager();

    void init();

    void downloadFile(const QString &file, const QString &remotePath = "");
    void uploadFile(const QString &file, const QString &remotePath = "");

    const QString &getDownloadPath() const;
    void setDownloadPath(const QString &newDownloadPath);

    bool getOpenBreakPointResume() const;
    void setOpenBreakPointResume(bool newOpenBreakPointResume);

signals:
    void sgl_file_download_process(const QString &file, float percent);
    void sgl_file_upload_process(const QString &file, float percent);

    // 发送信号，连接文件服务器
    void sgl_start_connect_ftp_server();

    // 文件传输结束
    void sgl_ftp_upload_task_finish(const QString &file, bool status, const QString &msg);

    // 文件服务登录状态改变
    void sgl_ftp_connect_status_change(bool status);

    // 开始上传文件
    void sgl_ftp_start_upload_file(const QString &file, const QString &remotepath, bool breakpointresume);

    // 开始下载文件
    void sgl_ftp_start_download_file(const QString &file, const QString &remotepath, bool breakpointresume);

    // 内部消息
    void sgl_ftp_work_status_message(const QString& message);

private slots:
    // 文件服务登录状态改变
    void slot_ftp_connect_status_change(bool status);

private:
    QString mDownloadPath = "./";

    // 传输协议实现
    FtpProtocol mFtpProtocol;

    // 工作线程
    QThread mFtpWorkThread;

    // 文件服务连接状态
    bool mFtpConnected = false;

    // 是否开启断点续传
    bool mOpenBreakPointResume = false;
};

#endif // FTPMANAGER_H
