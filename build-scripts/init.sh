#!/bin/bash

fan_equip=
disable_swap=

read -p "Fan Equip? Y/n" -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]
then
  fan_equip="-1"
fi

# Disable Swap
read -p "Disable Swap? Y/n" -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]
then
  disable_swap="true"
fi



# Setup RPI4 Pinouts
if ! /usr/bin/gpio -v | grep -Fq "gpio version: 2.52"; then
  echo 
  echo
  echo "Installing GPIO Drivers"
  echo
  wget -nc -q --show-progress https://project-downloads.drogon.net/wiringpi-latest.deb
  sudo dpkg -i wiringpi-latest.deb
fi

if [ ! -z $fan_equip ];
then

  echo 
  echo
  echo "Onboard Fan Detection"
  echo
  #  Find Fan
  if [[ $fan_equip = -1 ]];
  then
    /usr/bin/gpio mode 1 out
    /usr/bin/gpio write 1 1
    read -p "Check to hear or see if the Fan Turn On? y/N" -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]];
    then
      fan_equip="18"
    else
      /usr/bin/gpio write 1 0
      /usr/bin/gpio mode 1 in
    fi
  fi

  if [[ $fan_equip = -1 ]];
  then
    /usr/bin/gpio mode 7 out
    /usr/bin/gpio write 7 1
    read -p "Did Fan On This Time? y/N" -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]];
    then
      fan_equip="4"
    else
      /usr/bin/gpio write 7 0
      /usr/bin/gpio mode 7 in
    fi
  fi

  if [[ $fan_equip = -1 ]];
  then
    /usr/bin/gpio mode 26 out
    /usr/bin/gpio write 26 1
    read -p "Last Chance, Did The Fan On? y/N" -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]];
    then
      fan_equip="12"
    else
      /usr/bin/gpio write 26 0
      /usr/bin/gpio mode 26 in
    fi
  fi


  if [[ ! $fan_equip = -1 ]];
  then

    if ! grep -Fxq "gpio=$fan_equip=op,dh" /boot/config.txt
    then
      echo "gpio=$fan_equip=op,dh"| sudo tee -a /boot/config.txt
    fi

    sed -i "/FAN_PIN =/c\FAN_PIN = $fan_equip" ../scripts/fan_ctrl.py
    sudo cp ../scripts/fan_ctrl.py /opt/sdobox/

    if [ ! -f "/lib/systemd/system/fanctrl.service" ]; then
      sudo cp ../scripts/systemctl/fanctrl.service /lib/systemd/system/
      sudo systemctl enable fanctrl.service
      sudo systemctl start fanctrl.service
    else
      sudo cp ../scripts/systemctl/fanctrl.service /lib/systemd/system/
      sudo systemctl daemon-reload
      sudo systemctl restart fanctrl.service
    fi
  fi
fi


cd ..

# Init Submodules
git submodule init
git submodule update

# Copy around files
sudo mkdir -p /opt/sdobox
sudo cp -R images /opt/sdobox/
sudo cp -R scripts /opt/sdobox/
sudo cp scripts/overlays/* /boot/overlays/

cd "$(dirname "$0")"


# Fixup cmdline.txt
if ! grep -Fq "dwc_otg.lpm_enable" /boot/cmdline.txt
then
  echo "dwc_otg.lpm_enable=0 $(cat /boot/cmdline.txt)" | sudo tee /boot/cmdline.txt
fi

if ! grep -Fwq "fbcon=map:10" /boot/cmdline.txt
then
  sudo sed -i '$ s/$/ fbcon=map:10/' /boot/cmdline.txt
fi

if ! grep -Fwq "fbcon=font:VGA8x8" /boot/cmdline.txt
then
  sudo sed -i '$ s/$/ fbcon=font:VGA8x8/' /boot/cmdline.txt
fi

if ! grep -Fwq "logo.nologo" /boot/cmdline.txt
then
  sudo sed -i '$ s/$/ logo.nologo/' /boot/cmdline.txt
fi

if ! grep -Fq "loglevel" /boot/cmdline.txt
then
  sudo sed -i '$ s/$/ loglevel=3/' /boot/cmdline.txt
fi

if ! grep -Fq "vt.global_cursor_default" /boot/cmdline.txt
then
  sudo sed -i '$ s/$/ vt.global_cursor_default=0/' /boot/cmdline.txt
fi

sudo sed -i "s/console=serial0,/console=ttyAMA0,/" /boot/cmdline.txt
# sudo sed -i "s/console=tty1/console=tty3/" /boot/cmdline.txt

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
./sdl.sh -i -a

# Compile and install TSLib Libraries
./tslib.sh -i

# Compile and install MPV
./mpv.sh -i

# Activate Startup Idle process for MPV
if [ ! -f "/lib/systemd/system/mpv.service" ]; then
  sudo cp ../scripts/systemctl/mpv.service /lib/systemd/system/
  sudo systemctl enable mpv.service
  sudo systemctl start mpv.service
else
  ## Don't overwrite possible updates to mpv service systemctl file
  # sudo cp scripts/systemctl/mpv.service /lib/systemd/system/
  # sudo systemctl daemon-reload
  sudo systemctl restart mpv.service
fi


# Compile SDOBOX Project
./sdobox.sh -a

## Install to system
sudo ln -s /opt/sdobox/bin/sdobox /usr/local/bin/sdobox
# sudo chmod +x /usr/local/bin/sdobox

# Activate Startup Idle process for SDOBOX
if [ ! -f "/lib/systemd/system/sdobox.service" ]; then
  sudo cp ../scripts/systemctl/sdobox.service /lib/systemd/system/
  sudo systemctl enable sdobox.service
  sudo systemctl start sdobox.service
else
  ## Don't overwrite possible updates to sdobox service systemctl file
  # sudo cp scripts/systemctl/sdobox.service /lib/systemd/system/
  # sudo systemctl daemon-reload
  sudo systemctl restart sdobox.service
fi

# Install Headmelted version of Code
sudo su -c '. <( wget -O - https://code.headmelted.com/installers/apt.sh )'

# Open VSCode with SDOBOX as open folder
# code-oss .

sudo reboot now
