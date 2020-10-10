#!/usr/bin/env bash

# Used to create rsyslog omprog triggers
if ! grep -Fq 'template(name="outfmt"' /etc/rsyslog.conf
then
  echo '
template(name="outfmt" type="list" option.jsonf="on") {
         property(outname="@timestamp" name="timereported" dateFormat="rfc3339" format="jsonf")
         property(outname="host" name="hostname" format="jsonf")
         property(outname="severity" name="syslogseverity" caseConversion="upper" format="jsonf" datatype="number")
         property(outname="facility" name="syslogfacility" format="jsonf" datatype="number")
         property(outname="syslog-tag" name="syslogtag" format="jsonf")
         property(outname="source" name="app-name" format="jsonf" onEmpty="null")
         property(outname="message" name="msg" format="jsonf")

 }' | sudo tee -a /etc/rsyslog.conf
fi



if ! grep -Fq 'module(load="omprog")' /etc/rsyslog.conf
then
  echo '
module(load="omprog")' | sudo tee -a /etc/rsyslog.conf
fi


if ! grep -Fq '/opt/sdobox/scripts/rsyslog/rsyslog_event.sh' /etc/rsyslog.conf
then
  echo 'action(type="omprog"
       template="outfmt"
       binary="/opt/sdobox/scripts/rsyslog/rsyslog_event.sh")' | sudo tee -a /etc/rsyslog.conf
fi
# Must reboot for syslog changes


echo 'ACTION=="add", SUBSYSTEMS=="block", ENV{DEVTYPE}=="disk", RUN+="/opt/sdobox/scripts/rsyslog/udev_event.sh"
ACTION=="change", SUBSYSTEMS=="block", RUN+="/opt/sdobox/scripts/rsyslog/udev_event.sh"' | sudo tee /etc/udev/rules.d/80-usbdrive.rules
sudo chmod 644 /etc/udev/rules.d/80-usbdrive.rules
sudo udevadm control --reload

### Deprecated
### Used to create udev actions for usbdrive adding and removing
# echo 'ACTION=="add", KERNEL=="sd[a-z][0-9]", RUN+="/opt/sdobox/scripts/usb_management/udev_event.sh"
# ACTION=="change", SUBSYSTEMS=="usb", RUN+="/opt/sdobox/scripts/usb_management/udev_event.sh"
# ACTION=="remove", ENV{DEVTYPE}=="usb_device", RUN+="/opt/sdobox/scripts/usb_management/udev_event.sh"
# ' | sudo tee /etc/udev/rules.d/80-usbdrive.rules
# sudo chmod 644 /etc/udev/rules.d/80-usbdrive.rules
# sudo udevadm control --reload



