#ifndef NETCONNITEMSETTINGS_H
#define NETCONNITEMSETTINGS_H

#include <QDateTime>
#include <QHash>
#include <QMetaEnum>
#include <QObject>
#include <QSharedPointer>
#include <QStringListModel>
#include <QTcpSocket>

namespace module {
namespace net {
// 在qml中统一调用connEnum.xxx
inline namespace connEnum {
Q_NAMESPACE

// 连接状态
enum class ConnState {
    Disconnected,
    Connecting,
    Connected
};
Q_ENUM_NS(ConnState)


// 连接类型
enum class ConnType {
    TcpClient,
    TcpServer,
    ModbusTcp
};
Q_ENUM_NS(ConnType)

}    // namespace connEnum

// 网络连接设置
class NetConnItemSettings : public QObject {
    Q_OBJECT
public:
    explicit NetConnItemSettings(QObject* parent = nullptr) {};

    // 基本设置
    ConnType type = ConnType::TcpClient;
    QHostAddress serverAddress = QHostAddress("127.0.0.1");
    int port = 5678;

    // 接收设置
    // 是否显示时间戳
    bool timeStampEnabled = true;
    // 接收启用
    bool receiveEnabled = true;

    // 重连设置
    // 当服务端断连时，作为客户端是否需要断线重连；作为服务端是不需要断线重连的
    bool reconnectEnabled = true;
    // 尝试重连的次数
    int reconnectMaxTimes = 10;
    // 单次重连的等待时间（毫秒）
    int reconnectWaitingTime = 1000;

    // 首次建立连接的等待时间
    int fistWaitingTime = 1000;
};

}    // namespace net
}    // namespace module

#endif // NETCONNITEMSETTINGS_H
