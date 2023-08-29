#include <QDebug>
#include <QMetaEnum>

#include "netConn.h"

namespace module {
namespace net {
NetConn::NetConn(QObject* parent)
    : QObject {parent}
    , _settings {}
    , _countEnabled {true} {
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

void NetConn::socketGroupSendMsg(const QString& message) const {
    QByteArray data = message.toLocal8Bit();
    for (const auto& socket : _socketList) {
        socket->write(data);
        // 立刻发送缓存数据，避免阻塞
        if (!socket->flush()) {
            throw std::runtime_error("write failed");
        }
    }
}


void NetConn::init() {}

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
    // 如果是未连接则开始连接或监听，否则将跳过
    if (_settings.state == ConnState::Disconnected) {
        setState(ConnState::Connecting);

        // 根据不同类型分别启动，不写switch是因为会出现jump passby跳过初始化报错
        if (_settings.type == ConnType::TcpClient) {
            // 初始化
            QSharedPointer<QTcpSocket> socket = QSharedPointer<QTcpSocket>(new QTcpSocket());
            _socketList.append(socket);
            // 绑定信号
            connect(socket.data(),
                    &QAbstractSocket::connected,
                    this,
                    std::bind(&NetConn::onSocketConnected, this, socket.data()));
            connect(socket.data(),
                    &QAbstractSocket::disconnected,
                    this,
                    std::bind(&NetConn::onSocketDisconnected, this, socket.data()));

            connect(socket.data(),
                    &QAbstractSocket::readyRead,
                    this,
                    std::bind(&NetConn::onSocketReadyRead, this, socket.data()));
            connect(socket.data(),
                    &QAbstractSocket::errorOccurred,
                    this,
                    std::bind(&NetConn::onSocketError, this, std::placeholders::_1, socket.data()));

            // 开始连接，连接状态会在槽函数中改变
            socket->connectToHost(_settings.serverAddress, _settings.port);

        } else if (_settings.type == ConnType::TcpServer) {
            // 初始化
            _tcpServer = QSharedPointer<QTcpServer>(new QTcpServer(this));
            // 绑定信号
            connect(_tcpServer.data(),
                    &QTcpServer::newConnection,
                    this,
                    &NetConn::onTCPServerNewConnectd);

            // 开始监听，server可以直接判断listen是否成功，client socket就要在错误响应里判断
            if (!_tcpServer->listen(QHostAddress(_settings.serverAddress), _settings.port)) {
                qDebug() << "server err" << _tcpServer->errorString();
                setState(ConnState::Disconnected);

                return;
            } else {
                qDebug() << "server start" << _settings.serverAddress << _settings.port;
                setState(ConnState::Connected);

                return;
            }
        } else {
            setState(ConnState::Disconnected);
        }
    }
}

void NetConn::stop() {
    if (_settings.state == ConnState::Connected) {
        // 根据不同类型分别停止
        switch (_settings.type) {
            case ConnType::TcpClient:
                // 默认非server模式下，用且仅用列表里的第0个socket
                _socketList.at(0)->disconnectFromHost();
                break;

            case ConnType::TcpServer:
                _tcpServer->disconnect();
                _tcpServer->close();
                _socketList.clear();
                setState(ConnState::Disconnected);
                break;

            default:
                break;
        }
    }
}

void NetConn::connectBtnClicked() {
    if (_settings.state != ConnState::Disconnected) {
        stop();
    } else {
        run();
    }
}

void NetConn::setTimeStampEnableState(bool state) {
    _settings.timeStampEnabled = state;
}

bool NetConn::sendMessage(QString message) {
    if (_settings.state == ConnState::Connected) {
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

void NetConn::disconnectToClient(QString address, QString port) {
    qDebug() << "in onDisconnectToClients";

    _socketList.removeIf([address, port](const QSharedPointer<QTcpSocket>& socket) {
        return socket->peerAddress().toString() == address && socket->peerPort() == port;
    });
}

void NetConn::setState(const ConnState state) {
    if (_settings.state != state) {
        _settings.state = state;
        emit stateChanged(state);
    }
}


void NetConn::onSocketConnected(QTcpSocket* tempTcpSocketUser) {
    qDebug() << "Connected "
             << "ip:" << tempTcpSocketUser->peerAddress()
             << "port: " << tempTcpSocketUser->peerPort();
    setState(ConnState::Connected);
}

void NetConn::onSocketDisconnected(QTcpSocket* tempTcpSocketUser) {
    qDebug() << "Disconnected "
             << "ip:" << tempTcpSocketUser->peerAddress()
             << "port: " << tempTcpSocketUser->peerPort();

    // 在

    setState(ConnState::Disconnected);
}


void NetConn::onSocketReadyRead(QTcpSocket* tempTcpSocketUser) {
    if (_settings.receiveEnabled) {
        QByteArray data = tempTcpSocketUser->readAll();
        QString message = QString::fromLocal8Bit(data);
        qDebug() << "recv: " << message;

        // 根据不同设置格式化消息字符串
        if (_settings.timeStampEnabled) {
            int year, month, day;
            QDateTime::currentDateTime().date().getDate(&year, &month, &day);
            QString date = QString::number(year, 10) + "-" + QString::number(month, 10) + "-"
                + QString::number(day, 10);
            message = tr("[") + date + tr(" ") + QDateTime::currentDateTime().time().toString()
                + tr("]") + message;
        }

        // 发出消息改变的信号传到界面进行显示
        emit messageChanged(message);

        // 发出计数信号
        if (_countEnabled) {
            _recvBytesCount += data.length();
            emit statisticsChanged(QString::number(_recvBytesCount),
                                   QString::number(_sendBytesCount));
        }
    }
}

void NetConn::onSocketError(QAbstractSocket::SocketError error, QTcpSocket* tempTcpSocketUser) {
    qDebug() << "Socket error:" << error << "ip:" << tempTcpSocketUser->peerAddress()
             << "port: " << tempTcpSocketUser->peerPort();
    setState(ConnState::Disconnected);

    QString message = "error:";
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    message += QString(metaEnum.valueToKey(error));

    emit errorOccurred(message);
}

void NetConn::onTCPServerNewConnectd() {
    // 当前连接来的客户端
    auto nextSocket = _tcpServer->nextPendingConnection();
    QSharedPointer<QTcpSocket> socket = QSharedPointer<QTcpSocket>(nextSocket);
    _socketList.append(socket);

    // 添加信号槽
    connect(socket.data(),
            &QAbstractSocket::connected,
            this,
            std::bind(&NetConn::onSocketConnected, this, socket.data()));
    connect(socket.data(),
            &QAbstractSocket::disconnected,
            this,
            std::bind(&NetConn::onSocketDisconnected, this, socket.data()));

    connect(socket.data(),
            &QAbstractSocket::readyRead,
            this,
            std::bind(&NetConn::onSocketReadyRead, this, socket.data()));
    connect(socket.data(),
            &QAbstractSocket::errorOccurred,
            this,
            std::bind(&NetConn::onSocketError, this, std::placeholders::_1, socket.data()));
}


}    // namespace net
}    // namespace module
