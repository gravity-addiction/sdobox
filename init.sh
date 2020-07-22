#!/bin/bash

fan_equip=
disable_swap=

read -p "Fan Equip? Y/n" -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]
then
  fan_equip="18"
fi

# Disable Swap
read -p "Disable Swap? Y/n" -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]
then
  disable_swap="true"
fi

# Init Submodules
git submodule init
git submodule update

# Copy around files
sudo mkdir -p /opt/sdobox
sudo cp -R images /opt/sdobox/
sudo cp -R scripts /opt/sdobox/
sudo cp scripts/overlays/* /boot/overlays/


# Fixup cmdline.txt
echo "dwc_otg.lpm_enable=0 $(cat /boot/cmdline.txt) fbcon=map:10 fbcon=font:VGA8x8 logo.nologo loglevel=3 vt.global_cursor_default=0" | sudo tee /boot/cmdline.txt
sudo sed -i "s/console=serial0,/console=TTYAMA0,/" /boot/cmdline.txt
sudo sed -i "s/console=tty1/console=tty3/" /boot/cmdline.txt

# Setup Boot Config Files
sudo sed -i 's/^dtoverlay=vc4-fkms-v3d/#dtoverlay=vc4-fkms-v3d/g' /boot/config.txt

if ! grep -Fxq "hdmi_force_hotplug=1" /boot/config.txt
then
  echo 'hdmi_force_hotplug=1'| sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "dtparam=spi=on" /boot/config.txt
then
  echo 'dtparam=spi=on'| sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "dtparam=i2c1=on" /boot/config.txt
then
  echo 'dtparam=i2c1=on'| sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "dtparam=12c_arm=on" /boot/config.txt
then
  echo 'dtparam=12c_arm=on'| sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "dtoverlay=waveshare35c:rotate=270" /boot/config.txt
then
  echo 'dtoverlay=waveshare35c:rotate=270'| sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "gpu_mem=256" /boot/config.txt
then
  echo 'gpu_mem=256'| sudo tee -a /boot/config.txt
fi


# Setup RPI4 Pinouts
wget -nc -q --show-progress https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb

if [ ! -z $fan_equip ];
then
  if ! grep -Fxq "gpio=$fan_equip=op,dh" /boot/config.txt
  then
    echo "gpio=$fan_equip=op,dh"| sudo tee -a /boot/config.txt
  fi

  sed -i "/FAN_PIN =/c\FAN_PIN = $fan_equip" scripts/fan_ctrl.py
  sudo cp scripts/fan_ctrl.py /opt/sdobox/

  if [ ! -f "/lib/systemd/system/fanctrl.service" ]; then
    sudo cp scripts/systemctl/fanctrl.service /lib/systemd/system/
    sudo systemctl enable fanctrl.service
    sudo systemctl start fanctrl.service
  else
    sudo cp scripts/systemctl/fanctrl.service /lib/systemd/system/
    sudo systemctl daemon-reload
    sudo systemctl restart fanctrl.service
  fi
fi


# Disable Swap
if [ $disable_swap ]
then
  sudo dphys-swapfile swapoff
  sudo dphys-swapfile uninstall
  sudo update-rc.d dphys-swapfile remove
  sudo apt purge -y dphys-swapfile
fi


# Add & Generate en_US.UTF to system locales
sudo sed -i '/en_US.UTF/s/^# //' /etc/locale.gen
sudo locale-gen

# Update Apt
sudo apt-mark hold raspberrypi-bootloader
sudo apt-mark hold raspberrypi-kernel
sudo apt update
sudo apt dist-upgrade -y

# Add Touchscreen touch support
echo 'SUBSYSTEM=="input", ATTRS{name}=="ADS7846 Touchscreen", ENV{DEVNAME}=="*event*", ENV{LIBINPUT_CALIBRATION_MATRIX}="0 1 0 -1 0 1", SYMLINK+="input/touchscreen"' | sudo tee /etc/udev/rules.d/95-ads7846.rules
sudo chmod 644 /etc/udev/rules.d/95-ads7846.rules

# Compile and install SDL libraries
./sdl.sh -i

# Compile and install TSLib Libraries
./tslib.sh -i

# Install MPV Video Player
wget --no-check-certificate -q 'https://docs.google.com/uc?export=download&id=1lJn84VN5BIKwZ6fc7YDnqIdII9JO_hEU' -O - | bash

if [ ! -f "/lib/systemd/system/mpv.service" ]; then
  sudo cp scripts/systemctl/mpv.service /lib/systemd/system/
  sudo systemctl enable mpv.service
  sudo systemctl start mpv.service
else
  ## Don't overwrite possible updates to mpv service systemctl file
  # sudo cp scripts/systemctl/mpv.service /lib/systemd/system/
  # sudo systemctl daemon-reload
  sudo systemctl restart mpv.service
fi

# Install Extra SDOBOX Libraries
sudo apt install -y autoconf libtool libtool-bin libsdl-ttf2.0-0 libts0 libconfig9 fonts-noto-mono git libulfius2.5 libulfius-dev libxdo-dev libconfig-dev libsdl-ttf2.0-dev libsqlite3-dev libiw-dev libmpv-dev wmctrl

# Install Headmelted version of Code
su -c '. <( wget -O - https://code.headmelted.com/installers/apt.sh )'