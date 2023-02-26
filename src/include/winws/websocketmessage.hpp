#ifndef _websocketmessage_hpp_
#define _websocketmessage_hpp_

class WebSocketMessage {
public:
    char *buf;
    unsigned long long length;
public:
    bool isBinary;
    WebSocketMessage (char *buf, unsigned long long length, bool isBinary);
};

#endif
