#include <stdio.h>
#include <exception>

#include <ws/netsocket.hpp>

int main (int argc, char *argv[]) {
    try {

    NetSocket::init();
    NetSocket *socket = new NetSocket("localhost", "3000");

    socket->write("hello\n", 6);

    char buf[4096];

    bool packetReceived = false;

    int sum = 0;

    // while (!packetReceived) {
    while (sum < 50) {
        if (socket->hasData()) {
            int r = socket->read(buf, 4095, false);
            buf[r] = '\0';
            sum += r;
            printf("%s\n", buf);
            // packetReceived = true;
        }
    }

    socket->shutdown(); // Close for sending
    while (socket->hasData()) socket->read(buf, 4095, false);

    delete socket;

    NetSocket::uninit();

    } catch (std::exception& e) {
        printf("%s\n", e.what());
    }
    return 0;
}

