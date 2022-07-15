#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


int main() {

    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn == 0) {
        printf("Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("10.1.0.234");
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

    char frame_buffer[1000][17] = {};

    char frame[] = {
            0b10000010, 0b10001011,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            255, 0, 0
        };

    for (int index = 0; index < 1000; index++) {
        memcpy(frame_buffer[index], frame, 17);
    }

    srand(time(NULL));

    Display *display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);
    XImage *image = XGetImage(display, root, 1920, 0, 400, 300, AllPlanes, ZPixmap);

    unsigned long screen[400][300] = {};

    while (1) {
        usleep(100000);

        struct timeval stop, start;
        gettimeofday(&start, NULL);

        image = XGetSubImage(display, root, 1920, 0, 400, 300, AllPlanes, ZPixmap, image, 0, 0);

        gettimeofday(&stop, NULL);
        printf("snapping took %f s\n", ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec) / 1000000.0);

        int frame_index = 0;

        gettimeofday(&start, NULL);
        for (int i = 0; i < 400; i++) {

            unsigned long *buff = screen[i];

            for (int j = 0; j < 300; j++) {

                unsigned long pixel = XGetPixel(image, i, j);

                if (buff[j] != pixel) {
                    buff[j] = pixel;

                    if (frame_index == 1000) {
                        write(conn, frame_buffer, 17*1000);
                        frame_index = 0;
                    }

                    //printf("%d\n", frame_index);

                    char *f = frame_buffer[frame_index];

                    f[0+2+4] = (i & 0xff000000) >> 24;
                    f[1+2+4] = (i & 0x00ff0000) >> 16;
                    f[2+2+4] = (i & 0x0000ff00) >> 8;
                    f[3+2+4] = (i & 0x000000ff);

                    f[4+2+4] = (j & 0xff000000) >> 24;
                    f[5+2+4] = (j & 0x00ff0000) >> 16;
                    f[6+2+4] = (j & 0x0000ff00) >> 8;
                    f[7+2+4] = (j & 0x000000ff);

                    f[14] = (pixel >> 16) & 0xFF;
                    f[15] = (pixel >> 8) & 0xFF;
                    f[16] = (pixel) & 0xFF;

                    frame_index++;

                    //write(conn, f, 17);
                }
            }
        }
        if (frame_index != 0) {
            write(conn, frame_buffer, 17*frame_index);
        }
        gettimeofday(&stop, NULL);
        printf("printing took %f s\n", ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec) / 1000000.0);
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
