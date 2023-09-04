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


void NetConn::init() {
    item.init();


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

    // 取消重连任务
    //    cancelReconnect();

    // 如果是未连接则开始连接或监听，否则将跳过
    if (_state == ConnState::Disconnected) {
        setState(ConnState::Connecting);

        // 根据不同类型分别启动，不写switch是因为会出现jump passby跳过初始化报错
        if (_settings.type == ConnType::TcpClient) {
            //            // 初始化
            //            QSharedPointer<QTcpSocket> socket = createSocketWithSignal(new
            //            QTcpSocket());
            //            //            {
            //            //                std::lock_guard lock(_socketListMutex);
            //            //                _socketList.append(socket);
            //            //            }
            //            setState(ConnState::Connecting);
            //            // 开始连接，连接状态会在槽函数中改变
            //            socket->connectToHost(_settings.serverAddress, _settings.port);
            //            if (!socket->waitForConnected(_settings.fistWaitingTime)) {
            //                qCritical() << "cannot connect";
            //                setState(ConnState::Disconnected);
            //            }

            // bind member function
            //            client->bind_disconnect(&clt_listener::on_disconnect, clientlistener);
            //            client->bind_connect(
            //                &clt_listener::on_connect, clientlistener, std::ref(*client.get()));
            //            client->bind_recv(&clt_listener::on_recv,
            //                              clientlistener,
            //                              std::ref(*client.get()));    // not use std::bind
            //
            //            client->start(_settings.serverAddress.toString().toStdString(),
            //                          std::to_string(_settings.port));

            item.setType(_settings.type);
            item.run(_settings.serverAddress.toString().toStdString(),
                     std::to_string(_settings.port));

        } else if (_settings.type == ConnType::TcpServer) {
            item.setType(_settings.type);
            item.run(_settings.serverAddress.toString().toStdString(),
                     std::to_string(_settings.port));

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
            //            // 默认非server模式下，用且仅用列表里的第0个socket
            //            auto socket = _socketList.at(0);
            //            socket->disconnectFromHost();
            item.stop();
            setState(ConnState::Disconnected);
        } else if (_settings.type == ConnType::TcpServer) {
            item.stop();
            setState(ConnState::Disconnected);
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

void NetConn::disconnectToClient(QString address, quint16 port) {
    qCritical() << "disconnectToClient "
                << "ip:" << address << "port: " << port;
    removeSocket(address, port);
}

void NetConn::setState(const ConnState state) {
    if (_state != state) {
        _state = state;
        emit stateChanged(state);
    }
}

QSharedPointer<QTcpSocket> NetConn::createSocketWithSignal(QTcpSocket* rawSocketPtr) {
    QSharedPointer<QTcpSocket> socket =
        QSharedPointer<QTcpSocket>(rawSocketPtr, &QTcpSocket::deleteLater);
    // 绑定信号
//    connect(socket.data(),
//            &QAbstractSocket::connected,
//            this,
//            std::bind(&NetConn::onSocketConnected, this, socket));
//    connect(socket.data(),
//            &QAbstractSocket::disconnected,
//            this,
//            std::bind(&NetConn::onSocketDisconnected, this, socket.data()));
//    connect(socket.data(),
//            &QAbstractSocket::readyRead,
//            this,
//            std::bind(&NetConn::onSocketReadyRead, this, socket.data()));
//    connect(socket.data(),
//            &QAbstractSocket::errorOccurred,
//            this,
//            std::bind(&NetConn::onSocketError, this, std::placeholders::_1, socket.data()));

    return socket;
}

inline void NetConn::removeSocket(const QString& address, quint16 port) {
    qDebug() << "Removesocket "
             << "ip:" << address << "port: " << port;
    {
        std::lock_guard lock(_socketListMutex);

        _socketList.removeIf([address, port](const QSharedPointer<QTcpSocket>& socket) {
            return socket->peerAddress().toString() == address && socket->peerPort() == port;
        });
        qDebug() << "Current socketlist length after removing: " << _socketList.length();
    }
}

void NetConn::reconnectSocket(const QString& address, quint16 port) {
    // 用于从重连线程内传递取消/执行完成信号给主线程
    std::promise<void> outerPromise;
    _reconnectFinalResult = outerPromise.get_future();
    // 执行重连任务线程
    std::thread t([this,
                   address,
                   port,
                   _socketList = _socketList,
                   maxTimes = _settings.reconnectMaxTimes,
                   waitingTime = _settings.reconnectWaitingTime,
                   promise = std::move(outerPromise)]() mutable {
        // 尝试重连的次数
        int retry = 0;
        // 创建一个临时的socket对象，用于尝试连接
        auto socket = createSocketWithSignal(new QTcpSocket());

        // 循环重连，直到成功或达到最大次数或收到取消信号
        while (true) {
            qInfo() << "retrying tims: " << retry;
            // 连接到指定的地址和端口
            socket->connectToHost(address, port);
            // 等待连接结果，超时时间为5秒
            bool connected = socket->waitForConnected(waitingTime);
            // 如果连接成功，退出循环
            if (connected) {
                break;
            }
            // 是否达到最大次数
            retry++;
            if (retry >= maxTimes) {
                break;
            }
            // 是否收到取消信号
            if (_reconnectCancel) {
                break;
            }
        }
        // 如果取消了但也重连成功了，那就不管，因为之前已经在绑定好的connected事件中添加到列表里了
        //        // 如果连接成功，将临时的socket对象转为智能指针，并加入到socketList中
        //        if (socket->state() == QAbstractSocket::ConnectedState) {
        //            {
        //                std::lock_guard lock(_socketListMutex);
        //                _socketList.append(socket);
        //            }

        //            // 添加信号槽
        //            connect(socket.data(),
        //                    &QAbstractSocket::connected,
        //                    this,
        //                    std::bind(&NetConn::onSocketConnected, this, socket.data()));
        //            connect(socket.data(),
        //                    &QAbstractSocket::disconnected,
        //                    this,
        //                    std::bind(&NetConn::onSocketDisconnected, this, socket.data()));
        //            connect(socket.data(),
        //                    &QAbstractSocket::readyRead,
        //                    this,
        //                    std::bind(&NetConn::onSocketReadyRead, this, socket.data()));
        //            connect(socket.data(),
        //                    &QAbstractSocket::errorOccurred,
        //                    this,
        //                    std::bind(&NetConn::onSocketError, this, std::placeholders::_1,
        //                    socket.data()));
        //        } else {
        //            qCritical() << "reconnect failed";
        //        }
        if (socket->state() != QAbstractSocket::ConnectedState) {
            socket->abort();
            qCritical() << "reconnect failed";
        } else {
            //            auto newSocket = createSocketWithSignal(socket);
            //            newSocket->setSocketDescriptor(socket->socketDescriptor());
            //            qInfo() << "Connected "
            //                    << "ip:" << socket->peerAddress() << "port: " <<
            //                    socket->peerPort();
            // setState(ConnState::Connected);
            //  client确认连接上就放入列表
            //            {
            //                std::lock_guard lock(_socketListMutex);
            //                _socketList.append(newSocket);
            //                qDebug() << "Current socketlist length after appending: " <<
            //                _socketList.length();
            //            }
        }
        // 发出取消/执行完成信号
        promise.set_value();
    });
    // 将线程对象分离，让它在后台运行
    t.detach();
}

// 取消重连任务的函数
void NetConn::cancelReconnect() {
    try {
        // 如果有重连任务在运行，需要取消
        if (_reconnectFinalResult.valid()) {
            // 设置标志位
            _reconnectCancel = true;
            // 获取取消重连的完成信号
            _reconnectFinalResult.get();
            // 还原标志位
            _reconnectCancel = false;
        }
    } catch (std::exception& error) {
        qFatal() << "cancel reconnect exception:" << error.what();
    }
}


void NetConn::onSocketConnected(QSharedPointer<QTcpSocket> socket) {
    qInfo() << "Connected "
            << "ip:" << socket->peerAddress() << "port: " << socket->peerPort();
    setState(ConnState::Connected);
    // client确认连接上就放入列表
    {
        std::lock_guard lock(_socketListMutex);
        _socketList.append(socket);
        qDebug() << "Current socketlist length after appending: " << _socketList.length();
    }
}

void NetConn::onSocketDisconnected(QTcpSocket* socket) {
    qCritical() << "Disconnected "
                << "ip:" << socket->peerAddress() << "port: " << socket->peerPort();
    // 规定socket断开就清除，重连任务在错误处理里单独建立线程执行，重连上了再加入socketlist
    // 目前每项Conn里的socketlist只用来管理已建立的socket，其他视作每项本身的信息：
    // 作为client时，socket对象包含“需要连接的server的信息（地址和端口）"，但这是作为设置项无需在list记录
    // 作为server时，socket对象包含client的信息，但不会记录“历史连接过的client的信息”
    socket->abort();
    removeSocket(socket->peerAddress().toString(), socket->peerPort());

    // 如果不是server模式（其他模式只能有一个socket），那么就认为连接全部关闭，server模式需要自己手动关闭
    if (_settings.type != ConnType::TcpServer) {
        // 如果不满足以上假设，说明逻辑出现问题，则抛出异常
        if (!_socketList.empty()) {
            throw std::runtime_error("disconnect error: socketlist is not empty");
        }
        setState(ConnState::Disconnected);
    }
}


void NetConn::onSocketReadyRead(QTcpSocket* socket) {
    if (_settings.receiveEnabled) {
        QByteArray data = socket->readAll();
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

void NetConn::onSocketError(QAbstractSocket::SocketError error, QTcpSocket* socket) {
    qCritical() << "Socket error:" << error << "ip:" << socket->peerAddress()
                << "port: " << socket->peerPort();

    // 对于client的拒绝连接，需要手动调用disconnect，没有重连机制
    if (_settings.type == ConnType::TcpClient
        && error == QAbstractSocket::SocketError::ConnectionRefusedError) {
        socket->disconnect();
        onSocketDisconnected(socket);
    }

    // 对于client的服务端关闭错误，根据设置决定是否启动重连线程
    if (_settings.type == ConnType::TcpClient
        && error == QAbstractSocket::SocketError::RemoteHostClosedError
        && _settings.reconnectEnabled) {
        // 启动重连线程
        reconnectSocket(socket->peerAddress().toString(), socket->peerPort());
    }

    // 传出错误消息
    QString message = "error:";
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    message += QString(metaEnum.valueToKey(error));

    emit errorOccurred(message);
}

void NetConn::onTCPServerNewConnectd() {
    // 当前连接来的客户端
    auto nextSocket = _tcpServer->nextPendingConnection();
    // 转为智能指针后存入socket列表
    QSharedPointer<QTcpSocket> socket = createSocketWithSignal(nextSocket);
    {
        std::lock_guard lock(_socketListMutex);
        _socketList.append(socket);
        qDebug() << "Current socketlist length after appending: " << _socketList.length();
    }
}


}    // namespace net
}    // namespace module
