

[![Action cyclone](https://github.com/DefenceX/vivoe-media-framework/actions/workflows/build-ubuntu-amd64.yaml/badge.svg)](https://github.com/DefenceX/MediaX/actions/workflows/build-ubuntu-amd64.yaml.yaml)
[![Action sonarscan](https://github.com/DefenceX/vivoe-lite/actions/workflows/sonarcloud.yaml/badge.svg)](https://sonarcloud.io/project/overview?id=DefenceX_vivoe-media-framework)


[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_vivoe-media-framework&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_vivoe-media-framework&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_vivoe-media-framework&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_vivoe-media-framework&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_vivoe-media-framework&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-media-framework)


[![License](https://img.shields.io/badge/licence-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
# Real Time Protocol in C++
This code implements RFC4421 RTP (Real Time Protocol) Payload Format for Uncompressed Video but is mandated by the UK MoD as part of DEF STAN 00-082 (VIVOE) uncompressed RTP video streaming protocol for real time video. If you are not familiar with the Generic Vehicle Architecture (DEF STAN 00-23) and VIVOE then you can read more [here](https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture).

Transmit streams emit a SAP/SDP announcement every second as per RFC 2974 and RFC 4566. Also referenced as in DEF STAN 00-082.

## Payloader example
This is a RAW (YUV) Real Time Protocol pay-loader written in C++. This example is send only to receive the data you can use the gstreamer pipeline below.

> **NOTE** : This example has been tested on 64 bit ARM. Target hardware was the Nvidia Jetson TX1/TX2/AGX/Orin. Code is endian swapped. To run on intel set #define ARM  0 in [src/rtpstream.c](src/rtpstream.c). If you see jitter then modify rc.local as per [rc.local](tx1/rc.local)

#Dependencies
The following dependencies need to me installed prior to building this project:
```
sudo apt install libswscale-dev libgflags-dev libgtkmm-3.0-dev
```
libswscale is required for RGB/RGBA to UYVY colour space conversion.
# Installation
Build the example
```
mkdir build
cmake ..
```
Run the transmit example
```
./transmit-example
```
Command line arguments use **--help** and are listed below:
```
    -filename (the PNG file to use as the source of the video stream) type: string default: "testcard.png"
    -height (the height of the image) type: int32 default: 480
    -ipaddr (the IP address of the transmit stream) type: string default: "239.192.1.1"
    -port (the port to use for the transmit stream) type: int32 default: 5004
    -width (the width of the image) type: int32 default: 640
```
The recieve example will display the stream (user **--help** for options):
```
./recieve-example
```

Catch the stream using the gstreamer src pipeline in the section below. Following command line options are supported:

> **NOTE** : This example uses the test image ([images/testcard.png](images/testcard.png)) as the source of the video stream. You can replace the testcard with your own image or use another source for the video data.

## gstreamer YUV streaming examples
The test scripts [rtp-gst-test-rx.sh](example/rtp-gst-test-rx.sh), [rtp-gst-test-tx.sh](example/rtp-gst-test-tx.sh) runs the example program against gstreamer to ensure interoperability.

Use this pipeline as a test payloader to make sure gstreamer is working:

    gst-launch-1.0 videotestsrc ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! rtpvrawpay ! udpsink host=127.0.0.1 port=5004

Use this pipeline to capture the stream:

    gst-launch-1.0 -v udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! videoconvert ! ximagesink 

You can also run the provided examples back to back using the script [rtp-test.sh](example/rtp-test.sh)  

Gstreamer running with test image [images/testcard.png](images/testcard.png) (640x480):

![Test Card image](images/testcard.png)
