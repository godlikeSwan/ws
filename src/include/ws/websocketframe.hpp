#ifndef _websocketframe_hpp_
#define _websocketframe_hpp_

#define WSF_MAX_HEADER_SIZE 1 + 1 + 8 + 4
#define WSF_MIN_HEADER_SIZE 1 + 1

#define WSF_OPCODE_CONT 0x0
#define WSF_OPCODE_TEXT 0x1
#define WSF_OPCODE_BIN 0x2
// %x3-7 are reserved for further non-control frames
#define WSF_OPCODE_CLOSE 0x8
#define WSF_OPCODE_PING 0x9
#define WSF_OPCODE_PONG 0xa
// %xB-F are reserved for further control frames

#include "ws/netsocket.hpp"

class WebSocketFrame {
public:
    bool fin;
    bool rsv[3];
    unsigned char opcode;
    bool mask;
    char maskingKey[4];
    char payloadLengthBytes;
    long long unsigned payloadLength;
    char *payload;

    unsigned long long wholeLength;
    char *buf;
    char *buffPayload;

    bool failed;

    WebSocketFrame ();
    // ~WebSocketFrame ();
    WebSocketFrame (bool fin, bool rsv[3], unsigned char opcode, bool mask, long long unsigned payloadLength, char maskingKey[4], const char *payload);
    void doMask ();
    void parse (NetSocket *socket);
};

#endif
