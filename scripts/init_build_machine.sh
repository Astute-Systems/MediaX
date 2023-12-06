#!/bin/bash -e

# Store arg as linux version or default
if [ -z "$1" ]; then
  # Default if not specified
  LINUX_VERSION="22.04"
else
  LINUX_VERSION=$1
fi

# check is sudo 
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

apt-get --no-install-recommends -y install libv4l-dev libswscale-dev libgflags-dev libgmock-dev libgtkmm-3.0-dev cmake g++ libgtest-dev libva-dev libdrm-dev libx11-dev libxext-dev libxfixes-dev libvdpau-dev cppcheck libunwind-dev libgoogle-glog-dev gcovr lcov swig
apt-get --no-install-recommends -y install libunwind-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav gstreamer1.0-tools
# apt-get --no-install-recommends -y install libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
# Install vaapi gstreamer plugins and device driver for intel
if [[ $(uname -m) == aarch* ]]; then
  pip install cpplint
else
  if [ "$LINUX_VERSION" == "20.04" ]; then
    pip install cpplint 
  fi

  if [ "$LINUX_VERSION" == "22.04" ]; then
    apt-get --no-install-recommends -y install cpplint qt6-base-dev
  fi

  apt-get --no-install-recommends -y install gstreamer1.0-vaapi i965-va-driver 
fi
