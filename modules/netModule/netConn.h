#ifndef NETCONN_H
#define NETCONN_H

#include <QDateTime>
#include <QHash>
#include <QMetaEnum>
#include <QObject>
#include <QSharedPointer>
#include <QStringListModel>
#include <QTcpServer>
#include <QTcpSocket>
#include <future>
#include <mutex>
#include <thread>

//#include <asio2/tcp/tcp_server.hpp>

#include "netConnItem.h"

namespace module {
namespace net {



// 网络连接管理
class NetConn : public QObject {
    Q_OBJECT
public:
    explicit NetConn(QObject* parent = nullptr);

    // 获取连接类型字符串表，用于combobox显示
    Q_INVOKABLE QStringList connTypeStrList() const;
    // 获取服务器地址
    Q_INVOKABLE QString serverAddress() const;
    // 获取端口号
    Q_INVOKABLE int port() const;
    // 获取时间戳设置
    Q_INVOKABLE bool timeStampEnableState() const;
    // 获取接收启用设置
    Q_INVOKABLE bool receiveEnableState() const;
    // socket群发消息
    Q_INVOKABLE void socketGroupSendMsg(const QString& message) const;

public slots:
    // 初始化
    void init();
    // 设置连接类型
    void setConnType(ConnType type);
    // 设置服务器地址
    void setServerAddress(QString address);
    // 设置端口号
    void setServerPort(QString port);
    // 开始运行（作为client或server）
    void run();
    // 停止运行
    void stop();
    // 连接按钮触发，内部切换连接状态
    void connectBtnClicked();

    // 设置时间戳启用状态
    void setTimeStampEnableState(bool state);
    // 设置接收启用状态
    void setReceiveEnabledState(bool state);

    // 发送消息
    bool sendMessage(QString message);

    // 重置计数
    void resetCount();
    // 设置计数启用状态
    void setCountEnabledState(bool state);

    // 主动断开连接
    void disconnectToClient(QString address, quint16 port);


signals:
    // 连接状态改变
    void stateChanged(const ConnState state);
    // 接受消息改变
    void messageChanged(const QString& message);
    // 统计数据改变（发送计数和接受计数）
    void statisticsChanged(const QString& recvCountStr, const QString& sendCountStr);
    // 错误信号
    void errorOccurred(const QString& message);
    // 重连成功
    void reconnectSucceeded();

private:
    // tcp server，用组合的方式为每个连接item都提供了server和client的能力，方便直接使用
    QSharedPointer<QTcpServer> _tcpServer;
    // 用来管理已建立的socket
    // 默认非server模式下，用且仅用列表里的第0个socket
    QList<QSharedPointer<QTcpSocket>> _socketList;
    std::mutex _socketListMutex;

    // 设置
    NetConnItemSettings _settings;
    // 连接状态
    ConnState _state = ConnState::Disconnected;

    // 计数
    uint64_t _recvBytesCount;
    uint64_t _sendBytesCount;
    // 计数启用
    bool _countEnabled;

    // 重连取消/完成信号，现在只有client一个socket需要重连，不需要线程池
    std::future<void> _reconnectFinalResult;
    std::atomic<bool> _reconnectCancel;

    // 设置状态并发出信号
    void setState(const ConnState state);


    NetConnItem item;
    // 创建绑定信号后的socket
    QSharedPointer<QTcpSocket> createSocketWithSignal(QTcpSocket* rawSocketPtr);
    // 从列表中清除指定socket，当连接被动断开时，也应该调用该函数
    inline void removeSocket(const QString& address, quint16 port);
    // 重新连接
    inline void reconnectSocket(const QString& address, quint16 port);
    // 取消重连
    void cancelReconnect();
private slots:
    // 套接字连接成功时
    void onSocketConnected(QSharedPointer<QTcpSocket> socket);
    // 套接字断开连接时
    void onSocketDisconnected(QTcpSocket* socket);
    // 套接字有可读数据时
    void onSocketReadyRead(QTcpSocket* socket);
    // 套接字发生错误时
    void onSocketError(QAbstractSocket::SocketError error, QTcpSocket* socket);
    // tcp_server模式下有新客户端连接时
    void onTCPServerNewConnectd();
    // 重连成功时
    void onReconnectSucceeded() {}
};
}    // namespace net
}    // namespace module

#endif    // NETCONN_H
