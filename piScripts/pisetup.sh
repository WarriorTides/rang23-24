#!/bin/bash
if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi


vcgencmd get_camera

sleep 5
# Pings google. if google breaks, we don't have access to start a new camera, sorry. Needs the internet and
# this was the best way I could think of. If you have a better way to for it
# curl -s "https://raw.githubusercontent.com/WarriorTides/rang23-24/main/piScripts/pisetup.sh" | sudo bash
# TODO
if [[ "$(echo -e "GET http://google.com HTTP/1.0\n\n" | nc google.com 80 > /dev/null 2>&1)" == 0 ]]; then
    echo "Please connect to the internet to install libraries"
    exit
fi

# Makes sure the PI is up to date. This could cause things to break in the future

# curl -d "apt updated" ntfy.aayanmaheshwari.com/test
echo "apt updated"
rpi-update -y
# curl -d "rpi updated" ntfy.aayanmaheshwari.com/test
echo "rpi updated"
# Checks if ncat and nmap are installed and then installs them if they are not
# Needed for the raspivid function to stream the camera
# dpkg -l | grep git || apt install git -y
apt install git ncat nmap -y


echo "Packages are installed ang git cloned"
# curl -d "Packages are are installed" ntfy.aayanmaheshwari.com/test



# # Make stream.sh launch on startup
# crontab -l > crontab_new
# echo "@reboot bash /home/pi/rang23-24/piScripts/picam.sh" >> crontab_new
# crontab crontab_new
# rm crontab_new

# enable cameras
raspi-config nonint do_legacy 0
# curl -d "camera enabled" ntfy.aayanmaheshwari.com/test

# give more memory
echo -e "$(sed '/gpu_mem/d' /boot/config.txt)" > /boot/config.txt



echo "gpu_mem=128" >> /boot/config.txt
# curl -d "gpu_mem=128" ntfy.aayanmaheshwari.com/test

# turn off the red light. if you leave it on, it reflects off the glass
# echo "disable_camera_led=1" >> /boot/config.txt


# increase max framebuffers
echo -e "$(sed '/max_framebuffers/d' /boot/config.txt)" > /boot/config.txt
echo "max_framebuffers=4" >> /boot/config.txt
# echo "camera_auto_detect=1" >> /boot/config.txt

#crontab -l > mycron
#echo new cron into cron file

echo "alias camrun='sudo bash /home/pi/rang23-24/piScripts/picam.sh'" >> ~/.bashrc
#echo "@reboot bash /home/pi/rang23-24/piScripts/picam.sh" >> mycron

#echo "@reboot bash $(SCRIPT_DIR)/usbip.sh" >> mycron

#crontab mycron

# curl -d "camera setup complete" ntfy.aayanmaheshwari.com/test
# reboot now
echo "Setup is Complete Please Reboot!!"