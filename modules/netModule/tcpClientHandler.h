#ifndef NETASSISTANT_TCPCLIENTHANDLER_H
#define NETASSISTANT_TCPCLIENTHANDLER_H

#include <asio2/tcp/tcp_client.hpp>
#include <asio2/tcp/tcp_server.hpp>


namespace module {
namespace net {

class NetConn;

class clt_listener {
public:
    void on_connect(asio2::tcp_client& client);

    void on_disconnect() ;

    void on_recv(asio2::tcp_client& client, std::string_view data);

    // 需要在绑定前完成初始化
    void init(NetConn* conn);

private:
    // 用于调用界面更新，实际上应该用通知机制，起码弄个回调，但这里简单处理
    NetConn* _conn = nullptr;
};


class svr_listener
{
public:
    void on_recv(std::shared_ptr<asio2::tcp_session>& session_ptr, std::string_view data);
    void on_connect(std::shared_ptr<asio2::tcp_session>& session_ptr);
    void on_disconnect(std::shared_ptr<asio2::tcp_session>& session_ptr);
    void on_start(asio2::tcp_server& server);
    void on_stop(asio2::tcp_server& server);

    // 需要在绑定前完成初始化
    void init(NetConn* conn);

private:
    NetConn* _conn = nullptr;
};


}    // namespace net
}    // namespace module


#endif    // NETASSISTANT_TCPCLIENTHANDLER_H
