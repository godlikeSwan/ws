#include "winws/websocketmessage.hpp"

WebSocketMessage::WebSocketMessage (char *buf, unsigned long long length, bool isBinary) {
    this->isBinary = isBinary;
    this->buf = new char[length + 1];
    for (unsigned long long i = 0; i < length; i++) this->buf[i] = buf[i];
    this->length = length;
}
