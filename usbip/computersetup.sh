#!/bin/bash
if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi
 
apt install linux-tools-generic -y
modprobe vhci-hcd
echo "vhci-hcd" >> /etc/modules

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

$SCRIPT_DIR/computer.sh



