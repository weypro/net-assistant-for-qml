#include "tcpClientHandler.h"

#include "netConn.h"
namespace module {
namespace net {

void clt_listener::on_connect(asio2::tcp_client& client) {
    if (asio2::get_last_error()) {
        if (_conn != nullptr)
            _conn->setState(connEnum::ConnState::Disconnected);
        printf(
            "connect failure : %d %s\n", asio2::last_error_val(), asio2::last_error_msg().c_str());
    } else {
        if (_conn != nullptr)
            _conn->setState(connEnum::ConnState::Connected);
        printf("connect success : %s %u\n", client.local_address().c_str(), client.local_port());
    }
}

void clt_listener::on_disconnect() {
    if (_conn != nullptr)
        _conn->setState(connEnum::ConnState::Disconnected);
}

void clt_listener::on_recv(asio2::tcp_client& client, std::string_view data) {
    printf("recv : %zu %.*s\n", data.size(), (int)data.size(), data.data());

    if (_conn != nullptr)
        _conn->showReceivedMsg(std::string(data));
}


void clt_listener::init(NetConn* conn) {
    _conn = conn;
}

/****************************/

void svr_listener::on_recv(std::shared_ptr<asio2::tcp_session>& session_ptr, std::string_view data)
{
    if (_conn != nullptr)
        _conn->showReceivedMsg(std::string(data));
}

void svr_listener::on_connect(std::shared_ptr<asio2::tcp_session>& session_ptr)
{
    session_ptr->no_delay(true);

    printf("client enter : %s %u %s %u\n",
           session_ptr->remote_address().c_str(), session_ptr->remote_port(),
           session_ptr->local_address().c_str(), session_ptr->local_port());
}

void svr_listener::on_disconnect(std::shared_ptr<asio2::tcp_session>& session_ptr)
{
    printf("client leave : %s %u %s\n",
           session_ptr->remote_address().c_str(), session_ptr->remote_port(),
           asio2::last_error_msg().c_str());
}

void svr_listener::on_start(asio2::tcp_server& server)
{
    if (asio2::get_last_error()) {
        if (_conn != nullptr)
            _conn->setState(connEnum::ConnState::Disconnected);
        printf("start tcp server character failed : %s %u %d %s\n",
               server.listen_address().c_str(),
               server.listen_port(),
               asio2::last_error_val(),
               asio2::last_error_msg().c_str());
    } else {
        if (_conn != nullptr)
            _conn->setState(connEnum::ConnState::Connected);

        printf("start tcp server character : %s %u %d %s\n",
               server.listen_address().c_str(),
               server.listen_port(),
               asio2::last_error_val(),
               asio2::last_error_msg().c_str());
    }
}

void svr_listener::on_stop(asio2::tcp_server& server)
{
    _conn->setState(connEnum::ConnState::Disconnected);
    printf("stop tcp server character : %s %u %d %s\n",
           server.listen_address().c_str(), server.listen_port(),
           asio2::last_error_val(), asio2::last_error_msg().c_str());
}


void svr_listener::init(NetConn* conn) {
    _conn = conn;
}


}    // namespace net
}    // namespace module
