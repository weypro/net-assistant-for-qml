#ifndef NETASSISTANT_MESSAGEPACKET_H
#define NETASSISTANT_MESSAGEPACKET_H

#include <deque>
#include <iostream>
#include <list>
#include <memory>

namespace module {
namespace net {

// 消息包，可作为缓冲区使用
class MessagePacket {
public:
    static constexpr std::size_t maxLength = 512;

    // 此处不额外做一个能用string初始化message的构造函数，因为需要判断字符串是否能容纳的下
    MessagePacket()
        : _dataLength(0) {}

    bool setText(std::string text) {
        if (text.length() > maxLength) {
            return false;
        }
        std::copy(text.begin(), text.end(), _data.data());
        setLength(text.length());
        return true;
    }

    const std::string text() {
        std::string bufstr;
        bufstr.append(_data.data(), _dataLength);
        return bufstr;
    }

    const std::array<char, maxLength>& data() const {
        return _data;
    }

    std::array<char, maxLength>& data() {
        return _data;
    }

    std::size_t length() const {
        return _dataLength;
    }

    void setLength(std::size_t length) {
        _dataLength = length;
    }

private:
    std::array<char, maxLength> _data;
    std::size_t _dataLength;
};


// 消息队列，boost::asio的write需要依靠队列发送，否则会有问题
typedef std::deque<MessagePacket> MessageQueue;




class chat_participant {
public:
    virtual ~chat_participant() {}
    virtual void deliver(const MessagePacket& msg) = 0;
    virtual void close()=0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;



}    // namespace net
}    // namespace module

#endif    // NETASSISTANT_MESSAGEPACKET_H
