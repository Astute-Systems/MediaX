

[![Action cyclone](https://github.com/DefenceX/VivoeX/actions/workflows/build-ubuntu-amd64.yaml/badge.svg)](https://github.com/DefenceX/MediaX/actions/workflows/build-ubuntu-amd64.yaml)
[![Action sonarscan](https://github.com/DefenceX/vivoe-lite/actions/workflows/sonarcloud.yaml/badge.svg)](https://sonarcloud.io/project/overview?id=DefenceX_MediaX)


[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_MediaX&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_MediaX&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_MediaX&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_MediaX&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=DefenceX_vivoe-lite)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=DefenceX_MediaX&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=DefenceX_MediaX)


[![License](https://img.shields.io/badge/licence-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
# Real Time Protocol in C++
This code implements RFC4421 RTP (Real Time Protocol) Payload Format for Uncompressed Video but is mandated by the UK MoD as part of DEF STAN 00-082 (VIVOE) uncompressed RTP video streaming protocol for real time video. If you are not familiar with the Generic Vehicle Architecture (DEF STAN 00-23) and VIVOE then you can read more [here](https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture).

Transmit streams emit a SAP/SDP announcement every second as per RFC 2974 and RFC 4566. Also referenced as in DEF STAN 00-082.

## Documentation
Doxygen documentation is available on [Github Page](https://defencex.github.io/MediaX).

# Licensing

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.


This library is also available as part of VivoeX Pro under a commercial license. This version of the library supports all colour spaces listed in DEF STAN 00-082 and RTP H.264 hardware acceleration when run on Intel and Nvidia hardware. Please visit http://defencex.ai for more information and support.
