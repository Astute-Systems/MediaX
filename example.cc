/* Example RTP packet from wireshark
      Real-Time Transport Protocol
      10.. .... = Version: RFC 1889 Version (2)
      ..0. .... = Padding: False
      ...0 .... = Extension: False
      .... 0000 = Contributing source identifiers count: 0
      0... .... = Marker: False
      Payload type: DynamicRTP-Type-96 (96)
      Sequence number: 34513
      Timestamp: 2999318601
      Synchronization Source identifier: 0xdccae7a8 (3704285096)
      Payload: 000003c000a08000019e00a2000029292929f06e29292929...
*/

/*

Gstreamer1.0 working example UYVY streaming
===========================================
gst-launch-1.0 videotestsrc num_buffers ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue !
rtpvrawpay ! udpsink host=127.0.0.1 port=5004

gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480,
payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false


Use his program to stream data to the udpsc example above on the tegra X1

*/

#define RTP_OUTPUT_IP "127.0.0.1"
//#define RTP_OUTPUT_IP         "255.255.255.255"
#define RTP_OUTPUT_PORT 5004
#define STREAM_HEIGHT 480
#define STREAM_WIDTH 480
#define BUFSIZE (STREAM_WIDTH * STREAM_HEIGHT) * 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "pngget.h"
#include "rtp_stream.h"

void DumpHex(const void *data, size_t size) {
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    printf("%02X ", ((unsigned char *)data)[i]);
    if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~') {
      ascii[i % 16] = ((unsigned char *)data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i + 1) % 8 == 0 || i + 1 == size) {
      printf(" ");
      if ((i + 1) % 16 == 0) {
        printf("|  %s \n", ascii);
      } else if (i + 1 == size) {
        ascii[(i + 1) % 16] = '\0';
        if ((i + 1) % 16 <= 8) {
          printf(" ");
        }
        for (j = (i + 1) % 16; j < 16; ++j) {
          printf("   ");
        }
        printf("|  %s \n", ascii);
      }
    }
  }
}

int main(int argc, char **argv) {
  int c, frame = 0;
  int move = 0;
  char *yuv;
  char packet[BUFSIZE];
  png_bytep *row_pointers = nullptr;

  yuv = (char *)malloc((STREAM_WIDTH * STREAM_HEIGHT) * 2);

  printf("Example RTP streaming\n");

  read_png_file((char *)"lenna-lg.png");
  row_pointers = get_row_pointwes();

  /* setup RTP streaming class */
  RtpStream rtp(STREAM_WIDTH, STREAM_WIDTH);
  rtp.RtpStreamOut((char *)RTP_OUTPUT_IP, RTP_OUTPUT_PORT);
  rtp.Open();

  /* get a message from the user */
  memset(packet, 0, BUFSIZE);

  /* Loop frames forever */
  while (1) {
    int32_t time = 10000;

    /* Convert all the scan lines */
    for (c = 0; c < (STREAM_HEIGHT); c++) {
      png_byte *row = row_pointers[c];
      RgbToYuv(STREAM_HEIGHT, STREAM_WIDTH, &packet[c], (char *)row);
    }

    DumpHex(packet, 40);
    if (rtp.Transmit(packet) < 0) break;

#if 1
    /* move the image (png must have extra byte as the second image is green)  */
    move += 3;
    if (move == STREAM_WIDTH * 3) move = 0;
#endif

    /* approximatly 24 frames a second */
    usleep(1000000 / RTP_FRAMERATE);
    time += (Hz90 / RTP_FRAMERATE);
    printf("Sent frame %d\n", frame++);
  }

  free(yuv);
  printf("Example terminated...\n");

  return 0;
}
