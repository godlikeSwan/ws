#include "ws/websocketframe.hpp"
#include "ws/netsocket.hpp"

#include <stdexcept>

WebSocketFrame::WebSocketFrame () {}
// WebSocketFrame::~WebSocketFrame () {
//     delete[] this->buf;
//     delete[] this->payload;
// }

WebSocketFrame::WebSocketFrame (bool fin, bool rsv[3], unsigned char opcode, bool mask, long long unsigned payloadLength, char maskingKey[4], const char *payload) {
    this->failed = false;

    if (payloadLength < 126) this->payloadLengthBytes = 0;
    if (payloadLength > 127 && payloadLength <= 65535) this->payloadLengthBytes = 2;
    if (payloadLength > 65535) this->payloadLengthBytes = 8;

    this->wholeLength = 1 + 1 + this->payloadLengthBytes + (mask ? 4 : 0) + payloadLength;

    this->buf = new char[this->wholeLength];

    char *current = this->buf;

    unsigned char flags = 0;
    this->fin = fin;
    if (fin) flags |= 0x80;
    // TODO: add rsv support
    this->opcode = opcode;
    flags |= opcode;

    *(current++) = flags;

    unsigned char len = 0;

    this->mask = mask;
    if (mask) len |= 0x80;
    this->payloadLength = payloadLength;
    if (this->payloadLengthBytes == 0) len |= payloadLength;
    if (this->payloadLengthBytes == 2) len |= 126;
    if (this->payloadLengthBytes == 8) len |= 127;
    *(current++) = len;

    if (this->payloadLengthBytes == 2) {
        *(current++) = payloadLength & 0xff;
        *(current++) = (payloadLength >> 8) & 0xff;
    } else if (this->payloadLengthBytes == 8) {
        *(current++) = payloadLength & 0xff;
        *(current++) = (payloadLength >> 8) & 0xff;
        *(current++) = (payloadLength >> 16) & 0xff;
        *(current++) = (payloadLength >> 24) & 0xff;
        *(current++) = (payloadLength >> 32) & 0xff;
        *(current++) = (payloadLength >> 40) & 0xff;
        *(current++) = (payloadLength >> 48) & 0xff;
        *(current++) = (payloadLength >> 56) & 0xff;
    }

    if (this->mask) {
        this->maskingKey[0] = maskingKey[0];
        *(current++) = maskingKey[0];
        this->maskingKey[1] = maskingKey[1];
        *(current++) = maskingKey[1];
        this->maskingKey[2] = maskingKey[2];
        *(current++) = maskingKey[2];
        this->maskingKey[3] = maskingKey[3];
        *(current++) = maskingKey[3];
    }

    this->buffPayload = current;

    this->payload = new char[this->payloadLength];
    for (long long unsigned i = 0; i < this->payloadLength; i++) {
        this->payload[i] = payload[i];
        *(current++) = payload[i];
    }
}

void WebSocketFrame::doMask () {
    for (unsigned long long i = 0; i < this->payloadLength; i++) {
        this->payload[i] ^= this->maskingKey[i % 4];
        this->buffPayload[i] = this->payload[i];
    }
}

void WebSocketFrame::parse (NetSocket *s) {
    this->failed = false;

    char header[WSF_MAX_HEADER_SIZE];

    int r = s->read(header, WSF_MIN_HEADER_SIZE, true);

    if (r < WSF_MIN_HEADER_SIZE) {
        this->failed = true;
        return;
    }

    unsigned char lenByte = static_cast<unsigned char>(header[1]);
    this->payloadLength = lenByte & 0x7f;

    this->payloadLengthBytes = 0;

    if (this->payloadLength == 126) {
        this->payloadLengthBytes = 2;
    } else if (this->payloadLength == 127) {
        this->payloadLengthBytes = 8;
    }

    unsigned char headerSize = 1 + 1 + this->payloadLengthBytes;

    r = s->read(header, headerSize, true);

    if (r < headerSize) {
        this->failed = true;
        return;
    }

    char *current = header;

    unsigned char flags = *(current++);
    this->fin = (flags & 0x80) >> 7;
    this->rsv[0] = (flags & 0x40) >> 6;
    this->rsv[1] = (flags & 0x20) >> 5;
    this->rsv[2] = (flags & 0x10) >> 4;
    this->opcode = flags & 0x0f;

    char len = *(current++);
    this->mask = (len & 0x80) >> 7 == 1;

    if (this->mask) throw std::runtime_error("masked frame from server");

    this->payloadLength = len & 0x7f;

    if (this->payloadLengthBytes == 2) {
        unsigned a = *(current++);
        unsigned b = *(current++);
        this->payloadLength = a + (b << 8);
    } else if (this->payloadLengthBytes == 8) {
        unsigned long long a = *(current++);
        unsigned long long b = *(current++);
        unsigned long long c = *(current++);
        unsigned long long d = *(current++);
        unsigned long long e = *(current++);
        unsigned long long f = *(current++);
        unsigned long long g = *(current++);
        unsigned long long h = *(current++);
        this->payloadLength = a + (b << 8) + (c << 16) + (d << 24) + (e << 32) + (f << 40) + (g << 48) + (h << 56);
    }

    this->wholeLength = headerSize + /* 4 */ + this->payloadLength;

    this->buf = new char[this->wholeLength + 1];

    r = s->read(this->buf, static_cast<int>(this->wholeLength), true);

    if (r < this->wholeLength) {
        delete[] this->buf;
        this->failed = true;
        return;
    }

    s->read(this->buf, static_cast<int>(this->wholeLength), false);

    this->payload = new char[this->payloadLength + 1];
    for (long long unsigned i = 0; i < this->payloadLength; i++) {
        this->payload[i] = this->buf[headerSize + i];
    }
}
