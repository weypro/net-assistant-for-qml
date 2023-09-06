//
// Created by wey on 2023/9/4.
//
#include "netConn.h"

#include "netConnItem.h"
namespace module {
namespace net {
void NetConnItem::init(NetConn* conn) {
    _conn=conn;
    serverlistener.init(conn);
    clientlistener.init(conn);

    server
        .bind_recv(&svr_listener::on_recv, serverlistener)    // by reference
        .bind_connect(&svr_listener::on_connect, &serverlistener)    // by pointer
        .bind_disconnect(&svr_listener::on_disconnect, &serverlistener)
        .bind_start(std::bind(
            &svr_listener::on_start, &serverlistener, std::ref(server)))    //     use std::bind
        .bind_stop(
            &svr_listener::on_stop, serverlistener, std::ref(server));    // not use std::bind

    client.bind_disconnect(&clt_listener::on_disconnect, clientlistener)
        .bind_connect(
            &clt_listener::on_connect, clientlistener, std::ref(client))    // not use std::bind
        .bind_recv(
            &clt_listener::on_recv, clientlistener, std::ref(client));    // not use std::bind
}

void NetConnItem::setType(ConnType type) {
    _type = type;
}

void NetConnItem::run(const std::string& address, const std::string& port) {
    // 在开始启动前确保上一次连接全部关闭，因为有重连机制可能会连上
    client.stop();
    server.stop();

    if (_type == ConnType::TcpClient) {
        client.start(address, port);
    } else {
        server.start(address, port);
    }
}

void NetConnItem::stop() {
    if (_type == ConnType::TcpClient) {
        client.stop();
    }else{
        server.stop();
    }
}


void NetConnItem::send(const std::string& message) {
    if (_type == ConnType::TcpClient) {
        client.async_send(message);
    } else {
        server.async_send(message);
    }
}
}    // namespace net
}    // namespace module
