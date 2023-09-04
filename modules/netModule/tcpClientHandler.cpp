#include "tcpClientHandler.h"

namespace module {
namespace net {

void clt_listener::on_connect(asio2::tcp_client& client) {
    if (asio2::get_last_error())
        printf("connect failure : %d %s\n",
               asio2::last_error_val(),
               asio2::last_error_msg().c_str());
    else
        printf(
            "connect success : %s %u\n", client.local_address().c_str(), client.local_port());

    std::string str;
    int len = 128 + std::rand() % (300);
    for (int i = 0; i < len; i++) {
        str += (char)((std::rand() % 26) + 'a');
    }
    str += "\r\n";

    client.async_send(std::move(str));
}

void clt_listener::on_disconnect() {
    printf("disconnect : %d %s\n", asio2::last_error_val(), asio2::last_error_msg().c_str());
}

void clt_listener::on_recv(asio2::tcp_client& client, std::string_view data) {
    printf("recv : %zu %.*s\n", data.size(), (int)data.size(), data.data());

    std::string str;
    int len = 128 + std::rand() % (300);
    for (int i = 0; i < len; i++) {
        str += (char)((std::rand() % 26) + 'a');
    }
    str += "\r\n";

    // this is just a demo to show :
    // even if we force one packet data to be sent twice,
    // but the server must recvd whole packet once
    client.async_send(str.substr(0, str.size() / 2));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.async_send(str.substr(str.size() / 2));
    client.async_send(std::string("test"));
    // of course you can sent the whole data once
    // client.async_send(std::move(str));
}


/****************************/

void svr_listener::on_recv(std::shared_ptr<asio2::tcp_session>& session_ptr, std::string_view data)
{
    printf("recv : %zu %.*s\n", data.size(), (int)data.size(), data.data());

    // this is just a demo to show :
    // even if we force one packet data to be sent twice,
    // but the client must recvd whole packet once
    session_ptr->async_send(data.substr(0, data.size() / 2));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    session_ptr->async_send(data.substr(data.size() / 2));
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
    printf("start tcp server character : %s %u %d %s\n",
           server.listen_address().c_str(), server.listen_port(),
           asio2::last_error_val(), asio2::last_error_msg().c_str());
}

void svr_listener::on_stop(asio2::tcp_server& server)
{
    printf("stop tcp server character : %s %u %d %s\n",
           server.listen_address().c_str(), server.listen_port(),
           asio2::last_error_val(), asio2::last_error_msg().c_str());
}


}    // namespace net
}    // namespace module
