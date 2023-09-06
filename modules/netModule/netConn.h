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
    Q_INVOKABLE void socketGroupSendMsg(const QString& message);
    // socket接受消息显示
    Q_INVOKABLE void showReceivedMsg(const std::string& message);
    // 增加发送字节数
    Q_INVOKABLE void addSendBytesCount(const uint64_t count);

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

    // 设置状态并发出信号
    void setState(const ConnState state);
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
    // 设置
    NetConnItemSettings _settings;
    // 连接状态
    ConnState _state = ConnState::Disconnected;

    // 计数
    uint64_t _recvBytesCount;
    uint64_t _sendBytesCount;
    // 计数启用
    bool _countEnabled;

    NetConnItem item;
};
}    // namespace net
}    // namespace module

#endif    // NETCONN_H
