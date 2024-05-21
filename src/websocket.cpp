#include <string.h>
#include <stdlib.h>
#include <stdexcept>

#include "ws/netsocket.hpp"
#include "ws/websocketframe.hpp"
#include "ws/websocketmessage.hpp"

#include "ws/websocket.hpp"

void WebSocket::init () {
    NetSocket::init();
}

void WebSocket::uninit () {
    NetSocket::uninit();
}

WebSocket::WebSocket (const char *host, const char *port) : NetSocket(host, port) {
    // TODO: generate nonce
    const char *sendbuf = "\
GET / HTTP/1.1\r\n\
Host: localhost:3000\r\n\
Upgrade: websocket\r\n\
Connection: Upgrade\r\n\
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\
Sec-WebSocket-Version: 13\r\n\
\r\n";
    this->write(sendbuf, static_cast<int>(strlen(sendbuf)));
    char recvbuf[BUFFER_SIZE];
    int r = this->read(recvbuf, BUFFER_SIZE, false);
    recvbuf[r] = '\0';
    // TODO: parse and check response
    // printf("%s\n", recvbuf);
    this->setNonBlocking();
}

bool WebSocket::hasFrames () {
    while (this->hasData()) {
        WebSocketFrame *frame = new WebSocketFrame();
        frame->parse(this);
        if (frame->failed) {
            delete frame;
            break;
        }
        this->framesQueue.push(frame);
    }
    return !this->framesQueue.isEmpty();
}

bool WebSocket::hasMessages () {
    while (this->hasFrames()) {
        WebSocketFrame *frame = this->framesQueue.shift();
        if (frame->opcode == WSF_OPCODE_BIN || frame->opcode == WSF_OPCODE_TEXT) {
            this->messagesQueue.push(new WebSocketMessage(frame->payload, frame->payloadLength, frame->opcode == WSF_OPCODE_BIN));
        }
    }
    return !this->messagesQueue.isEmpty();
}

WebSocketMessage *WebSocket::getMessage () {
    if (this->messagesQueue.isEmpty()) throw std::runtime_error("attempt to get no message");
    return this->messagesQueue.shift();
}

void WebSocket::sendMessage (const char *data, unsigned long long length, bool isBinary) {
    bool fin = true;
    bool rsv[3] = {0, 0, 0};

    unsigned char opcode = isBinary ? WSF_OPCODE_BIN : WSF_OPCODE_TEXT;

    bool mask = true; // Since this is client
    char maskingKey[4];
    maskingKey[0] = rand() % 256;
    maskingKey[1] = rand() % 256;
    maskingKey[2] = rand() % 256;
    maskingKey[3] = rand() % 256;

    long long unsigned payloadLength = length;

    WebSocketFrame *frame = new WebSocketFrame(fin, rsv, opcode, mask, payloadLength, maskingKey, data);

    frame->doMask();

    this->write(frame->buf, static_cast<int>(frame->wholeLength));
}

void WebSocket::close () {
    bool fin = true;
    bool rsv[3] = {0, 0, 0};

    unsigned char opcode = WSF_OPCODE_CLOSE;

    bool mask = true;
    char maskingKey[4];
    maskingKey[0] = rand() % 256;
    maskingKey[1] = rand() % 256;
    maskingKey[2] = rand() % 256;
    maskingKey[3] = rand() % 256;

    long long unsigned payloadLength = 0;

    WebSocketFrame *frame = new WebSocketFrame(fin, rsv, opcode, mask, payloadLength, maskingKey, nullptr);

    // No error code support yet
    // frame->doMask();

    this->write(frame->buf, static_cast<int>(frame->wholeLength));
}
