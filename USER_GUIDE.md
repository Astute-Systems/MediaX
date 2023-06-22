\mainpage User Manual
\par Table of Contents

- \ref intro
- \ref dep
     - @ref install
- \ref gst
     - @ref examples
- \ref v4l2
- \ref uncompressed
     - @ref yuv
     - @ref rgb24
     - @ref mono8
- \ref h264
- \ref mjpeg

\section intro Introduction
This library provides functions for streaming video that conforms to DEF STAN 00-082.

An example of a video sequence is shown below:

![SAPDiagram](/images/plant_sap.png)

MediaX implements RFC4421 RTP (Real Time Protocol) Payload Format for Uncompressed Video but is mandated by the UK MoD as part of DEF STAN 00-082 (VIVOE) uncompressed RTP video streaming protocol for real time video. If you are not familiar with the Generic Vehicle Architecture (DEF STAN 00-23) and VIVOE then you can read more [here](https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture).

Transmit streams emit a SAP/SDP announcement every second as per RFC 2974 and RFC 4566. Also referenced as in DEF STAN 00-082.

\section examples Payloader examples

RAW Real Time Protocol (RTP) payloader and de-payloader samples are written in C++ and can be found in the examples directory. The receive-example.cc and transmit-example.cc send and receive the data and can be ran back to back.

The examples directory also contains helper scripts to run various demos.

> **NOTE** : This example has been tested on 64 bit ARM. Target hardware was the Nvidia Jetson TX1/TX2/AGX/Orin.

\section dep Dependencies

The following dependencies need to me installed prior to building this project:
Ubuntu 22.04
```
sudo apt install libswscale-dev libgtest-dev libgflags-dev libgtkmm-3.0-dev
```
CentOS Stream 8
```
sudo dnf install ffmpeg-libs gtest-devel gflags-devel
```
libswscale is required for RGB/RGBA to UYVY colour space conversion.
\section install Installation
Build the example
```
mkdir build
cmake -DBUILD_CUDA=OFF -DEXAMPLES=ON -DBUILD_TESTING=ON ..
```
To enable CUDA acceleration set -DBUILD_CUDA to ON, examples can als be enabled
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
    -pattern (The test pattern (0-4)
        0 - Use a PNG file (see -filename), default: 0
        1 - Colour bars
        2 - Greyscale bars
        3 - Scaled RGB values
        4 - Checkered test card) type: int32 default: 0

```
The receive example will display the stream (user **--help** for options):
```
./receive-example
```
Receiver has these additional receive command line options, see **--help** for more info:
```
    -session_name (the SAP/SDP session name) type: string default: "TestVideo1"
    -wait_sap (wait for SAP/SDP announcement) type: bool default: false
```

Catch the stream using the gstreamer src pipeline in the section below. Following command line options are supported:

> **NOTE** : This example uses the test image ./images/testcard.png as the source of the video stream, this has the default resolution of 640x480. You can replace the testcard with your own image or use another source for the video data.

![test card image](testcard.png)

\section gst Gstreamer examples
The test scripts ./example/rtp-gst-test-rx.sh, ./example/rtp-gst-test-tx.sh runs the example program against gstreamer to ensure interoperability.

Use this pipeline as a test payloader to make sure gstreamer is working:

    gst-launch-1.0 videotestsrc ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! rtpvrawpay ! udpsink host=127.0.0.1 port=5004

Use this pipeline to capture the stream:

    gst-launch-1.0 -v udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! videoconvert ! ximagesink 

You can also run the provided examples back to back using the script  ./example/rtp-test.sh 

\section v4l2 Video 4 Linux 2
A class is provided to grab video from live sources using v4l2 drivers. See v4l2_source.cc.

\section uncompressed RTP uncompressed
There are three RTP methods for streaming video, these are described in the sections below.

\subsection yuv YUV422
YUV422 is a digital video format that represents color using brightness (luma) and two color difference signals (chroma). The YUV color model separates the image into three components: Y (luma) represents the brightness information, while U and V (chroma) represent the color information. In YUV422, the chroma information is sampled at half the horizontal resolution compared to the luma information. It is often used in video encoding and transmission, and it provides a good balance between image quality and data size.

\subsection rgb24 RGB24
RGB24 is a color representation where each pixel is described by three components: red (R), green (G), and blue (B). In RGB24, each component is represented by 8 bits, resulting in 24 bits per pixel. This color model is commonly used in computer graphics, image processing, and display devices. It provides a wide range of colors and high color fidelity, but it can require more storage space compared to other colorspaces due to its higher bit depth.

\subsection mono8 Mono8
Mono8, also known as grayscale or monochrome, represents images using a single channel of intensity information. Each pixel in Mono8 is represented by 8 bits, ranging from 0 (black) to 255 (white). Mono8 is commonly used for black and white images, where color information is not necessary. It is widely used in applications such as document scanning, machine vision, and medical imaging. Mono8 provides a simple representation of grayscale images with relatively low data size requirements.

\section h264 RTP H.264
DEF-STAN 00-082 is a standard that specifies the support for the H.264 video coding standard with Real-Time Transport Protocol (RTP) transport. H.264, also known as Advanced Video Coding (AVC), is a widely used video compression standard that offers efficient compression while maintaining good video quality.

The H.264 standard, supported by DEF-STAN 00-082, provides guidelines and requirements for encoding, decoding, and transmitting video data in a compressed format. It utilizes various techniques, such as predictive coding, motion compensation, and entropy coding, to achieve high compression ratios.

RTP is a protocol used for real-time transmission of multimedia data over IP networks. It provides mechanisms for packetization, transmission, and reassembly of audio and video streams. When combined with the H.264 standard, RTP enables the efficient and timely delivery of compressed video data across networks.

DEF-STAN 00-082 defines the specific implementation and usage requirements for utilizing H.264 with RTP transport. It may include guidelines for packetization, synchronization, error resilience, and other parameters necessary for successful transmission and reception of H.264-encoded video streams using RTP.

By adhering to the DEF-STAN 00-082 standard, organizations and systems can ensure compatibility and interoperability when working with H.264-encoded video streams and RTP transport. This standardization promotes consistency and facilitates the exchange of video data between different systems, devices, or networks that support the specified requirements.

\section mjpeg RTP Motion JPEG
DEF-STAN 00-082 specifies the use of Motion JPEG (M-JPEG) over Real-Time Transport Protocol (RTP) as a video transmission mechanism. M-JPEG is a video compression format where each frame of the video is individually compressed as a separate JPEG image.

In the context of DEF-STAN 00-082, M-JPEG over RTP provides guidelines and requirements for transmitting video data in a compressed format suitable for real-time applications. RTP, on the other hand, is a protocol designed for real-time transport of multimedia data over IP networks.

M-JPEG over RTP allows video frames to be encoded as JPEG images, which can be transmitted as individual packets over the network using RTP. Each packet contains a complete JPEG image, enabling independent decoding and rendering of each frame at the receiving end.

DEF-STAN 00-082 defines the specific implementation and usage requirements for M-JPEG over RTP, including guidelines for packetization, synchronization, payload format, and other parameters necessary for successful transmission and reception of M-JPEG-encoded video streams using RTP.

By adhering to the DEF-STAN 00-082 standard, organizations and systems can ensure compatibility and interoperability when working with M-JPEG-encoded video streams and RTP transport. This standardization promotes consistency and facilitates the exchange of video data between different systems, devices, or networks that support the specified requirements.

It's important to note that the DEF-STAN 00-082 documentation would provide more detailed technical specifications, configuration guidelines, and recommendations for implementing and utilizing M-JPEG over RTP within the defined context.