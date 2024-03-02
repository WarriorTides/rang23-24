#!/bin/bash

# Script to be run at EVERY boot

if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi

raspi-config nonint do_legacy 0

# Start up camera streamer


export ROTATION=180
export WIDTH=1280
export HEIGHT=720
export FPS=22




while true; do
raspivid -n -cd MJPEG -awb auto -ifx none -b 25000000 -br 60 -t 0 -rot ${ROTATION} -w ${WIDTH} -h ${HEIGHT} -fps ${FPS} -o - | ncat -lv4 5000
done