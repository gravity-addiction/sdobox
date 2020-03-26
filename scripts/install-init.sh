#!/bin/bash

# Disable Swap
sudo dphys-swapfile swapoff
sudo dphys-swapfile uninstall
sudo update-rc.d dphys-swapfile remove
sudo apt purge -y dphys-swapfile


# Add & Generate en_US.UTF to system locales
sudo sed -i '/en_US.UTF/s/^# //' /etc/locale.gen
sudo locale-gen

# Extra Libraries
sudo apt update
sudo apt upgrade -y
sudo apt install -y autoconf libtool libtool-bin libsdl-ttf2.0-0 libts0 libconfig9 fonts-noto-mono git libulfius2.5

# Automount USB Sticks
sudo apt install -y exfat-fuse
sudo apt install -y exfat-utils pmount

echo '#!/bin/bash

PART=$1
FS_LABEL=`lsblk -o name,label | grep ${PART} | awk \\'{print $2}\\'`
FS_TYPE=`lsblk -o name,fstype | grep ${PART} | awk \\'{print $2}\\'`

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


# Add Touchscreen touch support
echo 'SUBSYSTEM=="input", ATTRS{name}=="ADS7846 Touchscreen", ENV{DEVNAME}=="*event*", ENV{LIBINPUT_CALIBRATION_MATRIX}="0 1 0 -1 0 1", SYMLINK+="input/touchscreen"' | sudo tee /etc/udev/rules.d/95-ads7846.rules
sudo chmod 644 /etc/udev/rules.d/95-ads7846.rules

mkdir ~/dev
cd ~/dev
git clone git://github.com/kergoth/tslib.git --depth 1
cd tslib
./autogen.sh
./configure
make -j4
sudo make install
sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
cd ~/




# Add MPV
wget --no-check-certificate -q 'https://raw.githubusercontent.com/gravity-addiction/rpi-mpv/master/install-mpv.sh' -O - | bash

echo "[Unit]
Description=MPV Startup

[Service]
Type=idle
Restart=yes

PIDFile=/run/mpv.pid
User=pi

ExecStart=/usr/bin/mpv --video-sync=display-resample --hr-seek-framedrop=yes --input-ipc-server=/tmp/mpv.socket --no-osc --no-osd-bar --osd-on-seek=no --reset-on-next-file=all --keep-open=always --idle
StandardOutput=null
StandardError=null

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/mpv.service
sudo systemctl enable mpv.service
sudo systemctl start mpv.service




# Add SDOBOX
echo "deb http://sdobox.skydiveorbust.com/ sdobox main" | sudo tee -a /etc/apt/sources.list
wget -qO - http://sdobox.skydiveorbust.com/pubkey.txt | sudo apt-key add -
sudo apt update
sudo apt install sdobox

mkdir -p ~/.config/touchapp
echo 'rotary_pin_a = 24;
rotary_pin_b = 25;
rotary_pin_btn = 23;
right_pin_btn = 22;
left_pin_btn = 21;
debounce_delay = 175;
btn_hold_delay = 800;' | tee ~/.config/touchapp/touchapp.conf

mkdir ~/shared

sudo reboot now

## Post reboot commands
# sudo DISPLAY=:0.0 xinput set-prop --type=float 'ADS7846 Touchscreen' 'Coordinate Transformation Matrix' 1.09867116676776 0.0126685555708521 -0.0712659131294964 0.000421126074768624 1.10129147730563 -0.0511172071924601 0 0 1
# sudo TSLIB_FBDEVICE=/dev/fb1 TSLIB_TSDEVICE=/dev/input/touchscreen ts_calibrate