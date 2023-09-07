#ifndef NETASSISTANT_NETCONNITEM_H
#define NETASSISTANT_NETCONNITEM_H

#include "netConnItemSettings.h"
#include "tcpClientHandler.h"

namespace module {
namespace net {

class NetConn;

class NetConnItem {
public:
    void init(NetConn* conn);

    void setType(ConnType type);

    void run(const std::string& address, const std::string& port);

    void stop();
    void send(const std::string& message);

private:
    asio2::tcp_server server;
    svr_listener serverlistener;
    asio2::tcp_client client;
    clt_listener clientlistener;

    ConnType _type;
    NetConn* _conn;
};
}    // namespace net
}    // namespace module

#endif    // NETASSISTANT_NETCONNITEM_H
