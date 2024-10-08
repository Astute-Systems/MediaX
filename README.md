
[![Action mediax](https://github.com/Astute-Systems/MediaX/actions/workflows/build-ubuntu-22.04-amd64.yaml/badge.svg)](https://github.com/Astute-Systems/MediaX/actions/workflows/build-ubuntu-22.04-amd64.yaml)
[![Action mediax](https://github.com/Astute-Systems/MediaX/actions/workflows/build-ubuntu-24.04-amd64.yaml/badge.svg)](https://github.com/Astute-Systems/MediaX/actions/workflows/build-ubuntu-24.04-amd64.yaml)
[![Action mediax](https://github.com/Astute-Systems/MediaX/actions/workflows/build-raspbian-12-arm64.yaml/badge.svg)](https://github.com/Astute-Systems/MediaX/actions/workflows/build-raspbian-12-arm64.yaml)
[![Action sonarscan](https://github.com/Astute-Systems/MediaX/actions/workflows/sonarcloud.yaml/badge.svg)](https://sonarcloud.io/project/overview?id=Astute-Systems_MediaX)
![Version](https://Astute-Systems.github.io/MediaX/version.svg)

[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=security_rating&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=sqale_rating&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=reliability_rating&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=vulnerabilities&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=code_smells&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=Astute-Systems_MediaX&metric=coverage&token=5c629e538c08ce4ac4d5d7a4cfc6e7e682c680f2)](https://sonarcloud.io/summary/new_code?id=Astute-Systems_MediaX)
# Real Time Protocol in C++

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/80x15.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.

This library (compliant to ISO/IEC 14882 C++17) implements [RFC 4421](https://datatracker.ietf.org/doc/html/rfc4421) RTP (Real Time Protocol) Payload Format for Uncompressed Video and [RFC 3984](https://datatracker.ietf.org/doc/html/rfc3984) for H.264 video as mandated by the UK MoD as part of DEF STAN 00-082 (VIVOE) uncompressed RTP video streaming protocol for real time video. If you are not familiar with the Generic Vehicle Architecture (DEF STAN 00-23) and VIVOE then you can read more [here](https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture).

Transmit streams emit a SAP/SDP announcement every second as per RFC 2974 and RFC 4566. Also referenced as in DEF STAN 00-082. Below is an example of how to stream video using the C++17 template class.

``` .cpp
#include "rtp/rtp.h"
int main(int argc, char *argv[]) {
  mediax::RtpSapTransmit<mediax::rtp::uncompressed::RtpUncompressedPayloader> rtp(
      "238.192.1.1", 5004, "test-session-name", 640, 480, 30, "RGB24");
  std::vector<uint8_t> &data = rtp.GetBufferTestPattern();
  for (int count = 0; count < 1000; count++) rtp.Transmit(data.data(), false);
}
```

To use another RTP payloader just change the template class i.e. To switch to H.264 use:

``` .cpp
  mediax::RtpSapTransmit<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader> rtp(
      "238.192.1.2", 5004, "test-session-name-compressed", 640, 480, 30, "H264");
```
## Documentation

Doxygen documentation is available to view on [Github Page](https://astute-systems.github.io/MediaX).

## Python bindings

Library has Python bindings for video streaming (and SAP/SDP support) with AI frameworks and use inside Jypter notebooks.

Once built and installed import as follows:

``` .py
import sys
sys.path.append('/usr/local/lib/python3/dist-packages/mediax')

import sap_listener
import sap_announcer
import rtpvraw_payloader
import rtpvraw_depayloader
```

# Qt6 bindings

These bindings are production ready and underpin our GUI toolchain. Please find documentation [here](https://astute-systems.github.io/MediaX/namespacemediax_1_1qt6.html) and code examples [here](https://astute-systems.github.io/MediaX/index.html#qtcode)

To build from source add with Qt6 add:

```
cmake -DBUILD_QT6=ON ..
```

# GxA support

All modes supported as defined in DEF-STAN 00-082 (uncompressed, MJPEG and H.264), these apply to Generic Vehicle Architectures (GVA). More recently the Generic Soldier Architecture (GSA) draft was published. MediaX supports for H.265 and AV1 (requires rust gstreamer plugins) encoders.

 > NOTE: You must install the code to enable the Rust gstreamer AV1 RTP element after building. Specify the build option **$ cmake -DBUILD_RUST=ON ..**
 >

## Licensing and Commercial support

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.

This library is also available as part of VivoeX Pro under a commercial license. This version of the library supports all colour spaces listed in DEF STAN 00-082 and RTP H.264 hardware acceleration when run on Intel and Nvidia hardware. Please visit <http://Astute-Systems.ai> for more information and support.

Commercial version also comes with GUI tools for debugging of video streams

![ToolX](images/ToolX_Dark.png)
