//
// Created by wey on 2023/9/4.
//

#ifndef NETASSISTANT_NETCONNITEM_H
#define NETASSISTANT_NETCONNITEM_H

#include "netConnItemSettings.h"
#include "tcpClientHandler.h"

namespace module {
namespace net {

class NetConnItem {
public:
    void init();

    void setType(ConnType type);

    void run(const std::string& address, const std::string& port);

    void stop();
    void send();

private:
    asio2::tcp_server server;
    svr_listener serverlistener;
    asio2::tcp_client client;
    clt_listener clientlistener;

    ConnType _type;
};
}    // namespace net
}    // namespace module

#endif    // NETASSISTANT_NETCONNITEM_H
