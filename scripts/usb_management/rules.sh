#!/usr/bin/env bash

### Used to create udev actions for usbdrive adding and removing

echo 'ACTION=="add", KERNEL=="sd[a-z][0-9]", RUN+="/opt/sdobox/scripts/usb_management/added.sh"
ACTION=="remove", ENV{DEVTYPE}=="usb_device", RUN+="/opt/sdobox/scripts/usb_management/remove.sh"
' | sudo tee /etc/udev/rules.d/80-usbdrive.rules
sudo chmod 644 /etc/udev/rules.d/80-usbdrive.rules
sudo udevadm control --reload