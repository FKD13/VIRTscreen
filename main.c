#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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
  // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(conn, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Failed to connect socket\n");
    exit(EXIT_FAILURE);
  }

  char frame_buffer[1000][7] = {};

  char frame[] = {0, 0, 0, 0, 255, 0, 0};

  for (int index = 0; index < 1000; index++) {
    memcpy(frame_buffer[index], frame, 7);
  }

  srand(time(NULL));

  Display *display = XOpenDisplay(NULL);
  Window root = DefaultRootWindow(display);
  XImage *image = XGetImage(display, root, 1920, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                            AllPlanes, ZPixmap);

  unsigned long screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {};

  while (1) {
    // usleep(75000);
    // usleep(50000);

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    image = XGetSubImage(display, root, 1920, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                         AllPlanes, ZPixmap, image, 0, 0);

    gettimeofday(&stop, NULL);
    printf("snapping took %f s\n", ((stop.tv_sec - start.tv_sec) * 1000000 +
                                    stop.tv_usec - start.tv_usec) /
                                       1000000.0);

    int frame_index = 0;

    gettimeofday(&start, NULL);
    for (int i = 0; i < SCREEN_WIDTH; i++) {

      unsigned long *buff = screen[i];

      for (int j = 0; j < SCREEN_HEIGHT; j++) {

        unsigned long pixel = XGetPixel(image, i, j);

        if (buff[j] != pixel) {
          buff[j] = pixel;

          if (frame_index == 1000) {
            write(conn, frame_buffer, 7 * 1000);
            frame_index = 0;
          }

          char *f = frame_buffer[frame_index];

          f[0] = (i & 0x0000ff00) >> 8;
          f[1] = (i & 0x000000ff);

          f[2] = (j & 0x0000ff00) >> 8;
          f[3] = (j & 0x000000ff);

          f[4] = (pixel >> 16) & 0xFF;
          f[5] = (pixel >> 8) & 0xFF;
          f[6] = (pixel)&0xFF;

          frame_index++;
        }
      }
    }
    if (frame_index != 0) {
      write(conn, frame_buffer, 7 * frame_index);
    }
    gettimeofday(&stop, NULL);
    printf("printing took %f s\n", ((stop.tv_sec - start.tv_sec) * 1000000 +
                                    stop.tv_usec - start.tv_usec) /
                                       1000000.0);
  }

  sleep(1);
  close(conn);
  return 0;
}
