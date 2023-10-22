\mainpage User Manual
\par Table of Contents

- \ref intro
- \ref dep
  - @ref install
- \ref gst
  - @ref examples
  - @ref code
  - @ref qtcode
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
cmake -DBUILD_CUDA=OFF -DEXAMPLES=ON -DBUILD_TESTING=ON -DGST_SUPPORTED=ON -DBUILD_QT6=ON ..
```

 > NOTE: To enable Intel H.264 acceleration set -DVAAPI to ON, this requires the Intel Media SDK to be installed. To enable CUDA acceleration set -DBUILD_CUDA to ON, examples can als be enabled

\section examples Examples

RAW Real Time Protocol (RTP) payloader and de-payloader samples are written in C++ and can be found in the examples directory. The receive-example.cc and transmit-example.cc send and receive the data and can be ran back to back.

The examples directory also contains helper scripts to run various demos.

> **NOTE** : This example has been tested on 64 bit ARM. Target hardware was the Nvidia Jetson TX1/TX2/AGX/Orin.

```
./rtp-transmit
```

Command line arguments use **--help** and are listed below:

```
    -device (the V4L2 device source (only with -source 1)) type: string
      default: "/dev/video0"
    -filename (the PNG file to use as the source of the video stream (only with
      -source 0)) type: string default: "testcard.png"
    -framerate (the image framerate) type: uint32 default: 25
    -height (the height of the image) type: uint32 default: 480
    -ipaddr (the IP address of the transmit stream) type: string
      default: "127.0.0.1"
    -mode (The video mode (0-4)
        0 - Uncompressed RGB
        1 - Uncompressed YUV
        2 - Mono16
        3 - Mono8
        4 - H.264
        ) type: uint32 default: 0
    -num_frames (The number of frames to send) type: uint32 default: 0
    -port (the port to use for the transmit stream) type: uint32 default: 5004
    -session_name (the SAP/SDP session name) type: string default: "TestVideo1"
    -source (The video source (0-10)
        0 - Use a PNG file (see -filename)
        1 - v4l2src
        2 - Colour bars
        3 - Greyscale bars
        4 - Scaled RGB values
        5 - Checkered test card
        6 - Solid white
        7 - Solid black
        8 - Solid red
        9 - Solid green
        10 - Solid blue
        ) type: uint32 default: 2
    -width (the width of the image) type: uint32 default: 640

```

The receive example will display the stream (user **--help** for options):

```
./rtp-receive
```

Receiver has these additional receive command line options, see **--help** for more info:

```
    -session_name (the SAP/SDP session name) type: string default: "TestVideo1"
    -wait_sap (wait for SAP/SDP announcement) type: bool default: false
```

Catch the stream using the gstreamer src pipeline in the section below. Following command line options are supported:

> **NOTE** : This example uses the test image ./images/testcard.png as the source of the video stream, this has the default resolution of 640x480. You can replace the testcard with your own image or use another source for the video data.

![test card image](testcard.png)

\subsection testcard Test Patterns
You can use the *rtp-transmit* tool to send synthetic video to the recipient. This video can take the form of one or more of the test card functions built into MediaX. There test card samples are shown below:

![EBU Colour Bars](images/testcards/CreateColourBarEbuTestCard.png)

Colour bars EBU (European Broadcast Union) created using CreateColourBarEbuTestCard()

![Colour Bars](images/testcards/CreateColourBarTestCard.png)

Colour bars created using CreateColourBarTestCard()

![Checked test card](testcards/CreateCheckeredTestCard.png)

Checkered test cards created using CreateCheckeredTestCard()

![Grey Bars](images/testcards/CreateGreyScaleBarTestCard.png)

Colour bars created using CreateGreyScaleBarTestCard()

![Quad Colour](images/testcards/CreateQuadTestCard.png)

Colour bars created using CreateQuadTestCard()

![Noise](images/testcards/CreateWhiteNoiseTestCard.png)

White noise created using CreateQuadTestCard()

\section code Code Examples
\subsection code_sap SAP/SDP Announcer
An example of the SAP/SDP announcer as found in sap-announcer.cc:
\snippet sap_announcer/sap-announcer.cc Sap example announcer

And to stop the SAP/SDP announcer:
\snippet sap_announcer/sap-announcer.cc Sap example stop

\subsection code_transmit RTP Transmit

Include the following to setup an uncompressed video stream as shown in the transmit.cc example
\snippet simple/transmit.cc Transmit includes

To start a SAP/SDP announcment and RTP stream:
\snippet simple/transmit.cc Transmit example open

Send a frame
\snippet simple/transmit.cc Transmit example transmit

Finalise the SAP session and RTP stream
\snippet simple/transmit.cc Transmit example close

\subsection code_receive RTP Receive
Include the following to setup an uncompressed video stream as shown in the receive.cc example
\snippet simple/receive.cc Receive includes

To start a SAP/SDP listener and RTP stream using hard coded stream information (basic functionality):
\snippet simple/receive.cc Receive example open

A Better way to set the ::mediax::rtp::StreamInformation is to wait for a SAP/SDP announcment
\snippet simple/receive.cc Receive example sap

Receive a frame using a lambda function. This keeps the video synchronised. It is possible to call the Recieve function with a timeout but this polling method is not recommended as it can drift over time.
\snippet simple/receive.cc Receive example receive

Finalise the SAP session and RTP stream
\snippet simple/receive.cc Receive example close

\subsection other_code RTP Other encoders
To swap to another encoder such as H.264 for video compression simply swap out the namespace for the required hardware accelleration

For NVIDIA NVENC using Gstreamer use:
\snippet simple/receive.cc Receive example nvidia

For Intel's Video Accelleration API (VAAPI)
\snippet simple/receive.cc Receive example vaapi

\section qtcode Qt6 Code Examples

rtp_transmit.h is an example transmitter with a single stream that will get emitted every time the sendFrame() slot is called:
\snippet qt/rtp-transmit/rtp_transmit.h QtTransmit example header

The implementation in rtp_transmit.cpp, just sends a test card and resets the timer
\snippet qt/rtp-transmit/rtp_transmit.cpp QtTransmit example code

A simple transmit example will have the following main. A timer starts the sending of the video frames
\snippet qt/rtp-transmit/main.cpp Create a Qt application

\section gst Gstreamer examples
The test scripts ./example/rtp-gst-raw-rx-\<colourspace>.sh, ./example/rtp-gst-raw-tx-\<colourspace>.sh runs the example program against gstreamer to ensure interoperability.

Use this pipeline as a test payloader to make sure gstreamer is working:

    gst-launch-1.0 videotestsrc ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! rtpvrawpay ! udpsink host=127.0.0.1 port=5004

Use this pipeline to capture the stream:

    gst-launch-1.0 -v udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! videoconvert ! ximagesink 

You can also run the provided examples back to back using the script  ./example/rtp-raw-\<colourspace>.sh

> CAUTION: Gstreamer will number scan lines from 0 whereas DEF-STAN 00-082 will start at 1, this is a known incompatibility and when mixed will appear to be one scan line out on a GVA display.

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
