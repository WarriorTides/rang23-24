#!/bin/bash
if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi

# Pings google. if google breaks, we don't have access to start a new camera, sorry. Needs the internet and
# this was the best way I could think of. If you have a better way to for it
# TODO
if [[ "$(echo -e "GET http://google.com HTTP/1.0\n\n" | nc google.com 80 > /dev/null 2>&1)" == 0 ]]; then
    echo "Please connect to the internet to install libraries"
    exit
fi

# Makes sure the PI is up to date. This could cause things to break in the future
apt update -y
rpi-update -y

# Checks if ncat and nmap are installed and then installs them if they are not
# Needed for the raspivid function to stream the camera
dpkg -l | grep ncat || apt install ncat -y
dpkg -l | grep nmap || apt install nmap -y

for arg in "$@"
do
 if [ "$arg" == "-u" ]; then
    dpkg -l | grep usbip || apt install usbip -y
    sudo modprobe usbip_host

    echo -e "usbip_host" >> /etc/modules

    # Place your code here that should run when -u is present
 fi
done

# Make stream.sh launch on startup
# crontab -l > crontab_new
# echo "@reboot bash /home/pi/rang23-24/camStream/stream.sh" >> crontab_new
# crontab crontab_new
# rm crontab_new

# enable cameras
raspi-config nonint do_legacy 0

# give more memory
echo -e "$(sed '/gpu_mem/d' /boot/config.txt)" > /boot/config.txt
echo "gpu_mem=256" >> /boot/config.txt

# turn off the red light. if you leave it on, it reflects off the glass
echo "disable_camera_led=1" >> /boot/config.txt


crontab -l > mycron
#echo new cron into cron file
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"


echo "@reboot bash $(SCRIPT_DIR)/picam.sh" >> mycron

#echo "@reboot bash $(SCRIPT_DIR)/usbip.sh" >> mycron

crontab mycron
rm mycron


reboot now