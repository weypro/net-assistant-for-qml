#include <QDebug>
#include <QMetaEnum>

#include "netConn.h"

namespace module {
namespace net {
NetConn::NetConn(QObject* parent)
    : QObject {parent}
    , _settings {}
    , _countEnabled {true}
    , _reconnectCancel {false} {
    // 需要在构造时手动注册类型
    qRegisterMetaType<ConnState>("ConnState");
    qRegisterMetaType<ConnType>("ConnType");

    resetCount();
}

QStringList NetConn::connTypeStrList() const {
    QHash<ConnType, QString> connTypeName {
        {ConnType::TcpClient, "TcpClient"},
        {ConnType::TcpServer, "TcpServer"},
    };

    // 遍历可能出现乱序问题，所以先这样简单写
    QStringList temp {};
    temp.append(connTypeName.value(ConnType::TcpClient));
    temp.append(connTypeName.value(ConnType::TcpServer));

    return temp;
}

QString NetConn::serverAddress() const {
    return _settings.serverAddress.toString();
}

int NetConn::port() const {
    return _settings.port;
}

bool NetConn::timeStampEnableState() const {
    return _settings.timeStampEnabled;
}

bool NetConn::receiveEnableState() const {
    return _settings.receiveEnabled;
}

void NetConn::socketGroupSendMsg(const QString& message) {
    item.send(message.toStdString());
}

void NetConn::showReceivedMsg(const std::string& message) {
    QByteArray data = QByteArray::fromStdString(message);
    QString qmessage = QString::fromLocal8Bit(data);
    qDebug() << "recv: " << qmessage;
    // 根据不同设置格式化消息字符串
    if (_settings.timeStampEnabled) {
        int year, month, day;
        QDateTime::currentDateTime().date().getDate(&year, &month, &day);
        QString date = QString::number(year, 10) + "-" + QString::number(month, 10) + "-"
            + QString::number(day, 10);
        qmessage = tr("[") + date + tr(" ") + QDateTime::currentDateTime().time().toString()
            + tr("]") + qmessage;
    }
    // 发出消息改变的信号传到界面进行显示
    emit messageChanged(qmessage);
    // 发出计数信号
    if (_countEnabled) {
        _recvBytesCount += data.length();
        emit statisticsChanged(QString::number(_recvBytesCount), QString::number(_sendBytesCount));
    }
}


void NetConn::init() {
    item.init(this);
}

void NetConn::setConnType(ConnType type) {
    _settings.type = type;

    qDebug() << "set type:" << type;
}

void NetConn::setServerAddress(QString address) {
    _settings.serverAddress = QHostAddress(address);

    qDebug() << "set addr:" << _settings.serverAddress;
}

void NetConn::setServerPort(QString port) {
    _settings.port = port.toInt();

    qDebug() << "set port:" << _settings.port << " from " << port;
}

void NetConn::run() {
    qDebug() << "run" << _state;

    // 如果是未连接则开始连接或监听，否则将跳过
    if (_state == ConnState::Disconnected) {
        setState(ConnState::Connecting);

        // 根据不同类型分别启动，不写switch是因为会出现jump passby跳过初始化报错
        if (_settings.type == ConnType::TcpClient || _settings.type == ConnType::TcpServer) {
            std::thread t([&]() {
                item.setType(_settings.type);
                item.run(_settings.serverAddress.toString().toStdString(),
                         std::to_string(_settings.port));
            });
            t.detach();

        } else {
            setState(ConnState::Disconnected);
        }
    }
}

void NetConn::stop() {
    qDebug() << "stop" << _state;
    if (_state == ConnState::Connected) {
        // 根据不同类型分别停止
        if (_settings.type == ConnType::TcpClient) {
            item.stop();
        } else if (_settings.type == ConnType::TcpServer) {
            item.stop();
        } else {
        }
    }
}

void NetConn::connectBtnClicked() {
    if (_state != ConnState::Disconnected) {
        stop();
    } else {
        run();
    }
}

void NetConn::setTimeStampEnableState(bool state) {
    _settings.timeStampEnabled = state;
}

bool NetConn::sendMessage(QString message) {
    if (_state == ConnState::Connected) {
        try {
            socketGroupSendMsg(message);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;    // 如果状态不是已连接，返回失败
}


void NetConn::resetCount() {
    _recvBytesCount = 0;
    _sendBytesCount = 0;
    emit statisticsChanged(QString::number(_recvBytesCount), QString::number(_sendBytesCount));
}

void NetConn::setCountEnabledState(bool state) {
    _countEnabled = state;
}

void NetConn::setReceiveEnabledState(bool state) {
    _settings.receiveEnabled = state;
}

void NetConn::setState(const ConnState state) {
    if (_state != state) {
        _state = state;
        emit stateChanged(state);
    }
}



}    // namespace net
}    // namespace module
