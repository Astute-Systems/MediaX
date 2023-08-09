#!/bin/bash -e

# check is sudo 
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

apt-get update
apt-get --no-install-recommends install libswscale-dev libgflags-dev libgtkmm-3.0-dev cmake g++ libgtest-dev libva-dev libdrm-dev libx11-dev libxext-dev libxfixes-dev libvdpau-dev