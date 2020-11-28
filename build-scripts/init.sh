#!/bin/bash

fan_equip=
disable_swap=
do_reboot=

# Get full path to this folder
PWDSRC=`dirname "$(readlink -f "$0")"`
cd "${PWDSRC}"

# Determine Version of Raspberry Pi
RPI3A="Raspberry Pi 3 Model A Plus"
RPI3B="Raspberry Pi 3 Model B Plus"
RPI4B="Raspberry Pi 4 Model B"
RPIVERSION=$(cat /proc/device-tree/model | tr '\0' '\n')

if [[ $RPIVERSION =~ $RPI3A.* || $RPIVERSION =~ $RPI3B.* ]]; then
  echo "RPI3 - $RPIVERSION" | fold -s
elif [[ $RPIVERSION =~ $RPI4B* ]]; then
  echo "RPI4 - $RPIVERSION" | fold -s
fi

# Determine OS Kernel Version
KERNELVERSIONRAW=`(uname -r)`
KERNELVERSION=( ${KERNELVERSIONRAW//./ } )

if [[ $KERNELVERSION -eq 4 ]]; then
  echo "Kernel 4 - $KERNELVERSION" | fold -s
else # [[ $KERNELVERSION -eq 5 ]]; then
  echo "Kernel $KERNELVERSION" | fold -s
fi


# Ask User about Attached Fan
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

read -p "Reboot When Finished? Y/n" -n 1 -r
echo
if [[ $REPLY =~ ^[Nn]$ ]]
then
  do_reboot="true"
fi

# Add & Generate en_US.UTF to system locales
sudo sed -i '/en_US.UTF/s/^# //' /etc/locale.gen
sudo locale-gen

# Init Submodules
cd "${PWDSRC}/.."
git submodule init
git submodule update

# Create Opt folders
if [ -d "/opt/sdobox" ]; then
  sudo mkdir -p /opt/sdobox
  sudo chown -R pi:pi /opt/sdobox
fi
if [ -d "/opt/sdobox/bin" ]; then
  sudo mkdir -p /opt/sdobox/bin
  sudo chown -R pi:pi /opt/sdobox/bin
fi
if [ -d "/opt/sdobox/images" ]; then
  sudo mkdir -p /opt/sdobox/images
  sudo chown -R pi:pi /opt/sdobox/images
fi
if [ -d "/opt/sdobox/scripts" ]; then
  sudo mkdir -p /opt/sdobox/scripts
  sudo chown -R pi:pi /opt/sdobox/scripts
fi
if [ -d "/opt/sdobox/tmp" ]; then
  sudo mkdir -p /opt/sdobox/tmp
  sudo chown -R pi:pi /opt/sdobox/tmp
fi



# Setup RPI4 Pinouts
cd "${PWDSRC}"
./wiringpi.sh
# if ! /usr/bin/gpio -v | grep -Fq "gpio version: 2.52"; then
#   echo 
#   echo
#   echo "Installing GPIO Drivers"
#   echo
#   wget -nc -q --show-progress https://project-downloads.drogon.net/wiringpi-latest.deb
#   sudo dpkg -i wiringpi-latest.deb
# fi



if [ ! -z $fan_equip ];
then
  cd "${PWDSRC}"
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
    cp ../scripts/fan_ctrl.py /opt/sdobox/bin/

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


# Copy overlays to boot folder
cd "${PWDSRC}"
if [[ $KERNELVERSION -eq 4 ]]; then
  sudo cp ../scripts/overlays/kernel4x/* /boot/overlays/
else # [[ $KERNELVERSION -eq 5 ]]; then
  sudo cp ../scripts/overlays/kernel5x/* /boot/overlays/
fi


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

if ! grep -Fq "gpu_mem=" /boot/config.txt
then
  if [[ $RPIVERSION =~ $RPI4B* ]]; then
    echo 'gpu_mem=768'| sudo tee -a /boot/config.txt
  else
    echo 'gpu_mem=256'| sudo tee -a /boot/config.txt
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




# Update Apt

## Holding Back to kernel 4
# sudo apt-mark unhold raspberrypi-bootloader
# sudo apt-mark unhold raspberrypi-kernel

# Update everything to latest
sudo apt update
sudo apt dist-upgrade -y

cd "${PWDSRC}"
./apt-install.sh


# Add Touchscreen touch support
echo 'SUBSYSTEM=="input", ATTRS{name}=="ADS7846 Touchscreen", ENV{DEVNAME}=="*event*", ENV{LIBINPUT_CALIBRATION_MATRIX}="0 1 0 -1 0 1", SYMLINK+="input/touchscreen"' | sudo tee /etc/udev/rules.d/95-ads7846.rules
sudo chmod 644 /etc/udev/rules.d/95-ads7846.rules


# Compile and install SDL libraries
cd "${PWDSRC}"
./sdl.sh -i -a


# Compile and install TSLib Libraries
cd "${PWDSRC}"
./tslib.sh -i

# Compile and install MPV
cd "${PWDSRC}"
./mpv.sh -i


# Activate Startup Idle process for MPV
cd "${PWDSRC}"
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
cd "${PWDSRC}"
./sdobox.sh -a



## Install to system
sudo ln -s /opt/sdobox/bin/sdobox /usr/local/bin/sdobox
# sudo chmod +x /usr/local/bin/sdobox

# Activate Startup Idle process for SDOBOX
cd "${PWDSRC}"
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

# read -p "Install Headmelted VsCode? Y/n" -n 1 -r
# echo
# if [[ $REPLY =~ ^[Nn]$ ]]
# then
#   echo "Skipping VSCode"
# else
#   # Install Headmelted version of Code
#   sudo su -c '. <( wget -O - https://code.headmelted.com/installers/apt.sh )'
#
#   # Open VSCode with SDOBOX as open folder
#   # code-oss .
# fi




if [ $do_reboot ]
then
  echo "Rebooting"
  sudo reboot now
fi

echo
echo "Init Completed!"
