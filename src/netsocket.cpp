#include <stdexcept>

#include "ws/netsocket.hpp"

#include <string.h>

// Linux
#ifdef __linux__

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define NO_ERROR 0

#endif

#ifdef _WIN32
WSADATA NetSocket::wsaData;
#endif

void NetSocket::init () {
#ifdef _WIN32
    int result = WSAStartup(MAKEWORD(2,2), &NetSocket::wsaData);

    if (result != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
}

void NetSocket::uninit () {
#ifdef _WIN32
    WSACleanup();
#endif
}

NetSocket::NetSocket (const char *host, const char *port) {
    this->isBlocking = true;

    int r;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    r = getaddrinfo(host, port, &hints, &result);
    if (r != 0) {
#ifdef _WIN32
        WSACleanup();
#endif
        throw std::runtime_error("getaddrinfo failed");
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        this->socket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (this->socket == INVALID_SOCKET) {
            continue;
            // freeaddrinfo(result);
            // WSACleanup();
            // throw std::runtime_error("Error at socket()");
        }

        r = connect(this->socket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (r != SOCKET_ERROR) break;

#ifdef _WIN32
        closesocket(this->socket);
#endif
#ifdef __linux__
        ::close(this->socket);
#endif
        this->socket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (this->socket == INVALID_SOCKET) {
#ifdef _WIN32
        WSACleanup();
#endif
        throw std::runtime_error("Unable to connect to server!");
    }


}

void NetSocket::write (const char *data, int lenght) {
    int r = send(this->socket, data, lenght, 0);

    if (r == SOCKET_ERROR) {
#ifdef _WIN32
        closesocket(this->socket);
        WSACleanup();
#endif
        throw std::runtime_error("send failed");
    }
}

bool NetSocket::hasData () {
    int r;
    char b;
    r = recv(this->socket, &b, 1, MSG_PEEK);
    if (r == SOCKET_ERROR) {
#ifdef _WIN32
        if (this->isBlocking || WSAGetLastError() != WSAEWOULDBLOCK) {
#endif
#ifdef __linux__
        if (this->isBlocking || (errno != EAGAIN && errno != EWOULDBLOCK)) {
#endif
            throw std::runtime_error("recv failed");
        }
    }
    return r == 1;
}

bool NetSocket::isClosed () {
    int r;
    char b;
    r = recv(this->socket, &b, 1, MSG_PEEK);
    if (r == SOCKET_ERROR) {
#ifdef _WIN32
        if (this->isBlocking || WSAGetLastError() != WSAEWOULDBLOCK) {
#endif
#ifdef __linux__
        if (this->isBlocking || (errno != EAGAIN && errno != EWOULDBLOCK)) {
#endif
            throw std::runtime_error("recv failed");
        }
    }
    return r == 0;
}

int NetSocket::read (char *data, int maxlen, bool peek) {
    int r = recv(this->socket, data, maxlen, peek ? MSG_PEEK : 0);
    if (r == SOCKET_ERROR) {
        throw std::runtime_error("recv failed");
    }
    return r;
}

void NetSocket::setNonBlocking () {
#ifdef _WIN32
    u_long socketMode = 1; // non blocking
    int r = ioctlsocket(this->socket, FIONBIO, &socketMode);
#endif
#ifdef __linux__
    int r = fcntl(this->socket, F_SETFL, O_NONBLOCK);
#endif
    if (r != NO_ERROR) {
        throw std::runtime_error("ioctlsocket failed");
    }
    this->isBlocking = false;
}

void NetSocket::close () {
  #ifdef _WIN32
    int r = shutdown(this->socket, SD_SEND);
#endif
#ifdef __linux__
    int r = shutdown(this->socket, SHUT_WR);
#endif
#ifdef _WIN32
        closesocket(this->socket);
#endif
#ifdef __linux__
        ::close(this->socket);
#endif
    if (r == SOCKET_ERROR) {
        throw std::runtime_error("shutdown failed");
    }
}
