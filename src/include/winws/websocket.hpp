#ifndef _websocket_hpp_
#define _websocket_hpp_

#include "winws/queue.hpp"
#include "winws/netsocket.hpp"
#include "winws/websocketframe.hpp"
#include "winws/websocketmessage.hpp"

#define BUFFER_SIZE 4096

class WebSocket : NetSocket {
private:
public:
    void static init ();
    void static uninit ();

    Queue<WebSocketFrame*> framesQueue;
    Queue<WebSocketMessage*> messagesQueue;

    WebSocket (const char *host, const char *port);
    bool hasFrames();
    bool hasMessages();
    WebSocketMessage *getMessage();
    void sendMessage(const char *data, unsigned long long length, bool isBinary);
    void close();
};

#endif
