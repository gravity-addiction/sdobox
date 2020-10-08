#!/bin/bash

# Automount USB Sticks
sudo apt install -y exfat-fuse
sudo apt install -y exfat-utils pmount

echo '#!/bin/bash

PART=$1
FS_LABEL=`lsblk -o name,label | grep ${PART} | awk '\''{print $2}'\''`
FS_TYPE=`lsblk -o name,fstype | grep ${PART} | awk '\''{print $2}'\''`

if [ -z ${FS_LABEL} ]
then
  if [ ${FS_TYPE} = "exfat" ]
  then
    /bin/mkdir -p /media/${PART}
    /bin/mount -t exfat /dev/${PART} /media/${PART}
  else
    /usr/bin/pmount --umask 000 --noatime -w --sync /dev/${PART} /media/${PART}
  fi
else
  if [ ${FS_TYPE} = "exfat" ]
  then
    /bin/mkdir -p /media/${FS_LABEL}_${PART}
    /bin/mount -t exfat /dev/${PART} /media/${FS_LABEL}_${PART}
  else
    /usr/bin/pmount --umask 000 --noatime -w --sync /dev/${PART} /media/${FS_LABEL}_${PART}
  fi
fi' | sudo tee /usr/local/bin/automount
sudo chmod 755 /usr/local/bin/automount


echo "ACTION==\"add\", KERNEL==\"sd[a-z][0-9]\", TAG+=\"systemd\", ENV{SYSTEMD_WANTS}=\"usbstick-handler@%k\"'" | sudo tee /etc/udev/rules.d/usbstick.rules
sudo chmod 644 /etc/udev/rules.d/usbstick.rules

echo "[Unit]
Description=Mount USB sticks
BindsTo=dev-%i.device
After=dev-%i.device

[Service]
Type=oneshot
RemainAfterExit=yes
ExecStart=/usr/local/bin/automount %I
ExecStop=/usr/bin/pumount /dev/%I" | sudo tee /lib/systemd/system/usbstick-handler@.service
sudo chmod 644 /lib/systemd/system/usbstick-handler@.service
