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
    Udp
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
    ConnState state = ConnState::Disconnected;
    QHostAddress serverAddress = QHostAddress("127.0.0.1");
    int port = 5678;

    // 接收设置
    // 是否显示时间戳
    bool timeStampEnabled = true;
    // 接收启用
    bool receiveEnabled = true;
};

}    // namespace net
}    // namespace module

#endif // NETCONNITEMSETTINGS_H
