#include <stdio.h>
#include <exception>

#include <winws/netsocket.hpp>

int main (int argc, char *argv[]) {
    try {

    NetSocket::init();
    NetSocket *socket = new NetSocket("localhost", "3000");

    socket->write("hello", 6);

    char buf[4096];

    bool packetReceived = false;

    while (!packetReceived) {
        if (socket->hasData()) {
            int r = socket->read(buf, 4095, false);
            buf[r] = '\0';
            printf("%s\n", buf);
            packetReceived = true;
        }
    }

    socket->close(); // Close for sending
    while (socket->hasData()) socket->read(buf, 4095, false);

    NetSocket::uninit();

    } catch (std::exception& e) {
        printf("%s\n", e.what());
    }
    return 0;
}

