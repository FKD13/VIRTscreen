#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


int main() {

    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn == 0) {
        printf("Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("10.1.0.198");
    //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (connect(conn, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect socket\n");
        exit(EXIT_FAILURE);
    }

    char *headers = "GET /set_pixel HTTP/1.1\nUpgrade: websocket\nConnection: Upgrade\nSec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\nSec-WebSocket-Protocol: chat\nSec-WebSocket-Version: 13\n\n";

    send(conn, headers, strlen(headers), 0);

    char bu[1000];
    recv(conn, &bu, 1000, 0);

    printf("%s", bu);

    char frame[] = {
        0b10000010, 0b10001011,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        255, 0, 0
    };

    srand(time(NULL));

    //while (true) {

    uint32_t i = 100;
    uint32_t j = 100;
    uint8_t r = rand();
    uint8_t g = rand();
    uint8_t b = rand();

    printf("%d %d %d\n", r, g, b);

    //uint8_t buffer[11] = {0, 0, 0, 0, 0, 0, 0, 0, r, g, b};

    for (i = 0; i < 400; i++) {
        for (j = 0; j < 300; j++) {
            //sleep(1);
            frame[0+2+4] = (i & 0xff000000) >> 24;
            frame[1+2+4] = (i & 0x00ff0000) >> 16;
            frame[2+2+4] = (i & 0x0000ff00) >> 8;
            frame[3+2+4] = (i & 0x000000ff);
            frame[4+2+4] = (j & 0xff000000) >> 24;
            frame[5+2+4] = (j & 0x00ff0000) >> 16;
            frame[6+2+4] = (j & 0x0000ff00) >> 8;
            frame[7+2+4] = (j & 0x000000ff);
            //printf("%d %d %d %d %d\n", i, j, r, g, b);

            for (int n = 0; n<1000; n++) {
                bu[n] = 0;
            }

            //recv(conn, &bu, 1000, MSG_DONTWAIT);
            //printf("%s", bu);

            write(conn, frame, 17);
        }
    }

    sleep(1);
    close(conn);
    return 0;
}

/*
GET /set_pixel HTTP/1.1
Host: server.example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
Sec-WebSocket-Protocol: chat, superchat
Sec-WebSocket-Version: 13
Origin: http://example.com
*/

/*
GET /set_pixel HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
Sec-WebSocket-Protocol: chat, superchat
Sec-WebSocket-Version: 13
*/

/*
GET /set_pixel HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
Sec-WebSocket-Protocol: chat
Sec-WebSocket-Version: 13
*/
