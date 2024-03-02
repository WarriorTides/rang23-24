#!bin/bash

if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi


hostname="bob.local"

# Use the host command to resolve the hostname to an IP address
ip_address=$(host "$hostname" | grep 'has address' | awk '{print $4}')
export PI_PORT "1-1.1"

sudo modprobe vhci-hcd
sudo usbip attach -r ${ip_address} -b 