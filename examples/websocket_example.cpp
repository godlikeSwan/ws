#include <stdio.h>
#include <stdint.h>
#include <exception>

#include <winws/websocket.hpp>

int main (int argc, char *argv[]) {
    try {

    WebSocket::init();
    WebSocket *socket = new WebSocket("localhost", "3000");

    socket->sendMessage("Hello", 5, false);

    bool messageReceived = false;

    while (!messageReceived) {
        if (socket->hasMessages()) {
            printf("got message\n");
            WebSocketMessage *msg = socket->getMessage();
            msg->buf[msg->length] = '\0';
            printf("%s\n", msg->buf);
            printf("\n");
            messageReceived = true;
        }
    }

    socket->close();

    WebSocket::uninit();

    } catch (std::exception& e) {
        printf("%s\n", e.what());
        // throw e;
    }
    return 0;
}

