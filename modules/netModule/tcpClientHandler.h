//
// Created by wey on 2023/9/4.
//

#ifndef NETASSISTANT_TCPCLIENTHANDLER_H
#define NETASSISTANT_TCPCLIENTHANDLER_H

#include <asio2/tcp/tcp_server.hpp>
#include <asio2/tcp/tcp_client.hpp>


namespace module {
namespace net {

class clt_listener {
public:
    void on_connect(asio2::tcp_client& client);

    void on_disconnect() ;

    void on_recv(asio2::tcp_client& client, std::string_view data) ;

};


class svr_listener
{
public:
    void on_recv(std::shared_ptr<asio2::tcp_session>& session_ptr, std::string_view data);
    void on_connect(std::shared_ptr<asio2::tcp_session>& session_ptr);
    void on_disconnect(std::shared_ptr<asio2::tcp_session>& session_ptr);
    void on_start(asio2::tcp_server& server);
    void on_stop(asio2::tcp_server& server);
};


}    // namespace net
}    // namespace module


#endif    // NETASSISTANT_TCPCLIENTHANDLER_H
