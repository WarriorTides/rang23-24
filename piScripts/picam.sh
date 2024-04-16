#!/bin/bash

# Script to be run at EVERY boot

if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi

raspi-config nonint do_legacy 0

# Start up camera streamer
function handle_sigint() {
    echo "Ctrl+C pressed, exiting gracefully..."
    exit 0
}

# Use trap to catch SIGINT and call handle_sigint when it's received
kilall raspivid
kilall ncat

trap handle_sigint SIGINT
#also handle other signals
trap handle_sigint SIGTERM


export ROTATION=180
export WIDTH=1280
export HEIGHT=720
export FPS=22




while true; do
raspivid -n -cd MJPEG -awb auto -ifx none -b 25000000 -br 60 -t 0 -rot ${ROTATION} -w ${WIDTH} -h ${HEIGHT} -fps ${FPS} -o - | ncat -lv4 5000
done