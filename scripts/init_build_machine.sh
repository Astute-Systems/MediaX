#!/bin/bash -e

# check is sudo 
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

apt-get --no-install-recommends -y install libswscale-dev libgflags-dev libgtkmm-3.0-dev cmake g++ libgtest-dev libva-dev libdrm-dev libx11-dev libxext-dev libxfixes-dev libvdpau-dev cppcheck cpplint libunwind-dev libgoogle-glog-dev
apt-get --no-install-recommends -y install libunwind-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav gstreamer1.0-tools
# apt-get --no-install-recommends -y install libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly