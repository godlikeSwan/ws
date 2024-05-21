#ifndef _netsocket_hpp_
#define _netsocket_hpp_

// Windows
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <iphlpapi.h>

#endif

#include <stdio.h>


class NetSocket {
private:
#ifdef _WIN32
    static WSADATA wsaData;

    SOCKET socket;
#endif
#ifdef __linux__
    int socket;
#endif

    bool isBlocking;
public:
    static void init ();
    static void uninit ();

    NetSocket (const char *host, const char *port);
    ~NetSocket ();
    bool hasData ();
    bool isClosed ();
    void write (const char *data, int length);
    int read (char *data, int maxlen, bool peek);
    void setNonBlocking ();
    void shutdown ();
};

#endif
