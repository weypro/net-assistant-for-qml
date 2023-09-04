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
#include <cstdlib>
#include <deque>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <utility>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "MessagePacket.h"
#include "netConnItemSettings.h"


namespace module {
namespace net {

using tcp = boost::asio::ip::tcp;


class NetConn;
class chat_server;

//----------------------------------------------------------------------

class chat_room {
public:
    void join(chat_participant_ptr participant) {
        participants_.insert(participant);
    }

    void leave(chat_participant_ptr participant) {
        participants_.erase(participant);
    }

    void setServer(chat_server* s) {
        _server = s;
    }

    chat_server* server() {
        return _server;
    }

    void deliver(const MessagePacket& msg) {
        for (auto participant : participants_)
            participant->deliver(msg);
    }

    void close(){
        for(auto& item:participants_){
            item->close();
        }
        participants_.clear();
        qDebug()<<"room close";
    }

private:
    std::set<chat_participant_ptr> participants_;
    chat_server* _server;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant
    , public std::enable_shared_from_this<chat_session> {
public:
    chat_session(tcp::socket socket, chat_room& room)
        : socket_(std::move(socket))
        , room_(room) {}
    ~chat_session();

    void start() {
        room_.join(shared_from_this());
        do_read_body();
    }

    void deliver(const MessagePacket& msg);

    void close();

private:
    void do_read_body();

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if(self->_closeRequest){
                    return;
                }
                if (!ec) {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty()) {
                        do_write();
                    }
                } else {
                    room_.leave(shared_from_this());
                }
            });
    }

    tcp::socket socket_;
    chat_room& room_;
    MessagePacket read_msg_;
    MessageQueue write_msgs_;

    // 用于同步，是否有关闭请求
    std::atomic<bool> _closeRequest = false;
    // 用于同步，在关闭前无法析构
    std::atomic<bool> _closeReady = false;
};

//----------------------------------------------------------------------

class chat_server {
public:
    explicit chat_server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint,
                         boost::asio::io_context& context, NetConn* conn)
        : acceptor_(io_context, endpoint)
        , _conn(conn)
        , io_context(context) {
        room_.setServer(this);
        do_accept();
    }

    // 构造即启动，析构即关闭
    ~chat_server() {
        close();
        while (!_closeReady) {
            ;
        }
        qDebug()<<"server deconstruct";
    };

    // 删除拷贝构造和赋值
    chat_server(chat_server& s) = delete;
    chat_server& operator=(const chat_server& s) = delete;
    chat_server(chat_server&& s) = default;
    chat_server& operator=(chat_server&& s) = default;

    chat_room& room() {
        return room_;
    }

    NetConn* conn() {
        return _conn;
    }

    boost::asio::io_context& context() {
        return io_context;
    }

private:
    void do_accept();

    tcp::acceptor acceptor_;
    chat_room room_;
    // 无conn所有权，仅为方便调用
    NetConn* _conn;
    boost::asio::io_context& io_context;

    // 用于同步，在关闭前无法析构
    std::atomic<bool> _closeReady = false;

    void close() {
        boost::asio::post(io_context, [&]() {
            acceptor_.close();
            room_.close();
            _closeReady = true;
        });
    }
};


// 网络连接管理
class NetConn : public QObject {
    Q_OBJECT
public:
    explicit NetConn(QObject* parent = nullptr);
    ~NetConn();

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

    void showReceiveMessage(const QString& message) {
        emit messageChanged(message);
    }

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
    //    QList<QSharedPointer<QTcpSocket>> _socketList;
    std::mutex _socketListMutex;
    std::vector<tcp::socket> _socketList;
    // boost::asio的设计导致记录raw socket是不合适的，

    // 创建io_service对象
    boost::asio::io_service io_service;

    // 设置
    NetConnItemSettings _settings;
    // 连接状态
    ConnState _state = ConnState::Disconnected;

    // 计数
    uint64_t _recvBytesCount;
    uint64_t _sendBytesCount;
    // 计数启用
    bool _countEnabled;

    boost::asio::io_context io_context;

    // 重连取消/完成信号，现在只有client一个socket需要重连，不需要线程池
    std::future<void> _reconnectFinalResult;
    std::atomic<bool> _reconnectCancel;

    // 设置状态并发出信号
    void setState(const ConnState state);

    // 创建绑定信号后的socket
    QSharedPointer<QTcpSocket> createSocketWithSignal(QTcpSocket* rawSocketPtr);
    // 从列表中清除指定socket，当连接被动断开时，也应该调用该函数
    inline void removeSocket(const QString& address, quint16 port);
    // 重新连接
    inline void reconnectSocket(const QString& address, quint16 port);
    // 取消重连
    void cancelReconnect();

    std::thread serviceThread;
    void doReadSocket(boost::asio::io_context& io_context) {
        qDebug() << "asdf";
        try {
            auto& socket = _socketList[0];
            std::array<char, 128> recv_buf;
            while (1) {
                if (stopFlag) {
                    break;
                }
                try {
                    std::future<std::size_t> recv_length = socket.async_read_some(
                        boost::asio::buffer(recv_buf), boost::asio::use_future);

                    // Do other things here while the receive completes.

                    std::string bufstr;
                    bufstr.append(recv_buf.data(), recv_length.get());
                    //            qDebug() << bufstr;
                    //                std::cout<<bufstr;
                    qDebug() << bufstr;
                    emit messageChanged(QString::fromStdString(bufstr));
                } catch (std::exception& error) {
                    // 断开连接在有async read的地方处理
                    qDebug() << socket.remote_endpoint().port();
                    {
                        qDebug() << "remove";
                        try {
                            closeSocket(socket);

                            if (_settings.type == ConnType::TcpClient) {
                                // 假设client模式下在断联后_socketList的长度应该为0，否则抛异常
                                if (_socketList.size() != 0) {
                                    qDebug() << "list size err";
                                    throw std::runtime_error("list size err");
                                } else {
                                    setState(ConnState::Disconnected);
                                }
                            }
                            qDebug() << "Current socketlist length after removing: "
                                     << _socketList.size();
                        } catch (...) {
                            qDebug() << "exception:Current socketlist length after removing: "
                                     << _socketList.size();
                        }
                    }

                    // emit errorOccurred(QString(error.what()));
                    break;
                }
            }
        } catch (std::system_error& e) {
            qDebug() << e.what();
        }
    }


    void do_write() {
        auto& socket = _socketList[0];
        boost::asio::async_write(
            socket,
            boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
            [this, &socket](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty()) {
                        do_write();
                    }
                } else {
                    emit errorOccurred(QString::fromStdString(ec.what()));
                }
            });
    }


    void write(const std::string& msg) {
        boost::asio::post(io_context, [this, msg]() {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(msg);
            if (!write_in_progress) {
                do_write();
            }
        });
    }
    std::deque<std::string> write_msgs_;

    //    boost::asio::io_service::strand strand_;
    std::shared_ptr<boost::asio::io_service::strand> strand_;
    std::shared_ptr<tcp::acceptor> acceptor_;


    void server_run() {
        acceptor_->async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                emit messageChanged(QString("new client"));
                {
                    std::lock_guard lock(_socketListMutex);
                    this->_socketList.push_back(std::move(socket));
                }

                std::thread t(
                    [this, &io_context = io_context]() { this->doReadSocket(io_context); });
                t.detach();
            } else {
                // 在监听器关闭后，所有记录的连接也要清除
                try {
                    for (const auto& socket : this->_socketList) {
                        closeSocket(socket);
                    }
                } catch (...) {
                    qDebug() << "exception:Current socketlist length after removing: "
                             << this->_socketList.size();
                }
                // 最后设置状态
                this->setState(ConnState::Disconnected);
                return;
            }

            server_run();
        });
    }

    void closeSocket(const tcp::socket& socket) {
        std::lock_guard lock(_socketListMutex);
        // 注意在删除前不能写socket.close();
        _socketList.erase(
            std::remove_if(_socketList.begin(),
                           _socketList.end(),
                           [&](const tcp::socket& i) {
                               if (i.remote_endpoint().port() == socket.remote_endpoint().port()
                                   && i.remote_endpoint().address()
                                       == socket.remote_endpoint().address())
                                   return true;
                               else
                                   return false;
                           }),
            _socketList.end());
    }

    std::shared_ptr<chat_server> _server;

    std::atomic<bool> stopFlag;

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
