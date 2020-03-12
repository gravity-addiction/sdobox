# ***SDOBOX***
**touchscreen application for single board computers**

<hr>

## **Setup Rasberian Image**
Recommend to download the full, "*Raspbian Buster with desktop and recommended software*" from https://www.raspberrypi.org/downloads/raspbian/

Install the image to an SD card. For more instructions refer to https://www.raspberrypi.org/documentation/installation/installing-images/README.md

### **Easy**
Connect your raspberry pi to an HDMI monitor, keyboard, and mouse. Boot the Raspberry pi and follow the desktop instructions for getting started. Once inital setup if finished, look near the top left of your desktop to find the black square icon with a `>_` symbol. Click this icon to open a shell prompt. Skip forward to the ***Setup Hardware enabled MPV*** Section.

### **Advanced**
Using the computer to create the raspberrian sd card, open the boot partition of the sd card.

- *(optional)* enable remote ssh access by adding an empty file named `ssh` to the boot partition.
- *(optional)* pre-configure wifi access by adding a file named `wpa_supplicant.conf` to the boot partition containing the following. edit *YOUR_WIFI_NETWORK* and *YOUR_WIFI_PASSWORD* values.
```
country=US # Your 2-digit country code
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev

network={
    ssid="YOUR_WIFI_NETWORK"
    psk="YOUR_WIFI_PASSWORD"
    key_mgmt=WPA-PSK
}
```

<hr>

## **Setup SDOBOX Specific Components**
### **Waveshare 3.5(c) Touchscreen**

This is how we do it!

### **Fan Speed Controller**
This is how we do it!

<hr>


## **Setup Hardware enabled MPV**
### Quick Guide
copy and paste this command to a shell prompt on your raspberry pi device. This script will start a process of downloading all the required libraries. The installation process will take some time, meanwhile your raspberry pi may get warm.
```
wget --no-check-certificate -q 'https://docs.google.com/uc?export=download&id=1lJn84VN5BIKwZ6fc7YDnqIdII9JO_hEU' -O - | bash
```


### **Semi-Automatic Guide**
or use a shell script to automatically install mpv and dependancies

`scripts/install-mpv.sh` - mpv, ffmpeg, libass, x264, fdk-aac, lame

`scripts/install-mpv-plus-drive.sh` - mpv, ffmpeg, libass, x264, fdk-aac, lame, wiringPi

<hr>

## **Setup Development**
