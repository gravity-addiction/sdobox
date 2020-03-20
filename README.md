# SDOBOX

## Hardware Components
- Raspberry Pi 3a+, 3b+, 4+
- Waveshare 3.5(c) / 3.5(a) / UCTRONICS 3.5 High Speed
    - The 3.5(c) is the preferred screen that gives a little better framerate. You can also use the Waveshare 3.5(a) screen with a few configuration tweeks.
    - Recently found the UCTRONICS 3.5" Raspberry Pi Touch, High Speed Edition, does work as a replacement for the Waveshare 3.5(c).
- Powersupply with a rated output of 5v/3a .. This is standard for the Raspberry Pi 4+ that uses the USB-C connector. There are powersupplies available with the 3amp output rating to a micro-usb. Standard Raspberry Pi 3, 2.5amp output will produce under-voltage warnings.
- Raspberrian Buster
    - Prefer to use lite only version for Raspberry Pi 3a+
    - Full version optional to provide X11 Windows environment.

    ### Support for Optional Components
    - Push Buttons
    - Rotary encoder
    - 5v Fan, requires a transistor circuit to provide power properly
    - *(in development)* Fast charging circuit / li-po battery
    - *(in development)* Expand storage with nvme
    - *(in development)* External pinouts for attachable mods


## Installation

- Downloading and Install Raspberrian

     this step has been very well documented and described many places on the internet. To be honest, if you're brand new to raspberry pi, this project may not be the best for learning. It's heavy in development right now and it may flustrate new comers. *You've been warned.*

- Preboot config
    - WIFI

        on the new boot partition of the sd card, create a file named,
        `wpa_supplicant.conf`<br>Add The following to the *wpa_supplicant.conf* file.
        ```
        country=US # Your 2-digit country code
        ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev

        network={
            ssid="WIFI_NAME"
            psk="WIFI_PASSWORD"
            key_mgmt=WPA-PSK
        }
        ```

    - SSH

        on the new boot partition of the sd card, touch or create an empty file named, `ssh`


- sdobox installation

    From a shell prompt running the following commands to add the sdobox repository to your `/etc/apt/sources.list` and download sdobox through `sudo apt install sdobox`

    For the same information goto; https://sdobox.skydiveorbust.com/

    ```
    echo "deb http://sdobox.skydiveorbust.com/ sdobox main" | sudo tee -a /etc/apt/sources.list
    wget -qO - http://sdobox.skydiveorbust.com/pubkey.txt | sudo apt-key add -
    sudo apt update
    sudo apt install sdobox
    ```


- mpv installation

    `scripts/install-mpv.sh` - compile several video and audio libraries, including ffmpeg and mpv. downloads from original source locations

    `scripts/install-mpv-plus-drive.sh` - install ffmpeg, mpv and other video / audio libraries. installs wiringpi for RPI4. downloads from google drive mirror.


- touchscreen overlay installation

    - Waveshare 3.5(c) / URTRONICS 3.5 High Speed

        - Copy touchscreen overlay

            `sudo cp scripts/overlays/* /boot/overlays/`

        - Assign overlay in /boot/config.txt<br> :rotate=270 is optional for fixing upside down screens.

            `sudo sh -c 'echo "dtoverlay=waveshare35c:rotate=270" >> /boot/config.txt'`

        - Add Framebuffer mapping and Font to /boot/cmdline.txt

            `sudo sh -c 'echo "$(cat /boot/cmdline.txt) fbcon=map:10 fbcon=font:VGA8x8" > /boot/cmdline.txt'`


    - Waveshare 3.5(a)

        - Copy touchscreen overlay

            `sudo cp scripts/overlays/* /boot/overlays/`

        - Assign overlay in /boot/config.txt<br> :rotate=270 is optional for fixing upside down screens.

            `sudo sh -c 'echo "dtoverlay=waveshare35a:rotate=270" >> /boot/config.txt'`

        - Add Framebuffer mapping and default terminal font to /boot/cmdline.txt

            `sudo sh -c 'echo "$(cat /boot/cmdline.txt) fbcon=map:10 fbcon=font:VGA8x8" > /boot/cmdline.txt'`


- touchscreen driver installation

    - Install libinput link
        - Screens Using :rotate=270 (Upside Down)
            ```
            echo 'SUBSYSTEM=="input", ATTRS{name}=="ADS7846 Touchscreen", ENV{DEVNAME}=="*event*", ENV{LIBINPUT_CALIBRATION_MATRIX}="0 1 0 -1 0 1", SYMLINK+="input/touchscreen"' | sudo tee /etc/udev/rules.d/95-ads7846.rules
            sudo chmod 644 /etc/udev/rules.d/95-ads7846.rules
            ```

        - Screens Not Using :rotate=270 (Standard)
            ```
            echo 'SUBSYSTEM=="input", ATTRS{name}=="ADS7846 Touchscreen", ENV{DEVNAME}=="*event*", SYMLINK+="input/touchscreen"' | sudo tee /etc/udev/rules.d/95-ads7846.rules
            sudo chmod 644 /etc/udev/rules.d/95-ads7846.rules
            ```

    - Install latest tslib from source
        ```
        mkdir ~/Downloads
        cd ~/Downloads
        git clone git://github.com/kergoth/tslib.git --depth 1
        cd tslib
        ./autogen.sh
        ./configure
        make -j4
        sudo make install
        sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
        cd ~/
        ```

    - Reboot
    - Calibrate touchscreen

        `sudo TSLIB_FBDEVICE=/dev/fb1 TSLIB_TSDEVICE=/dev/input/touchscreen ts_calibrate`


## Development

Requires compiling on a raspberry pi platform.


### Clone and make repo
```
git clone --recurse-submodules https://github.com/gravity-addiction/sdobox.git
cd sdobox
make -j4
sudo ./touchapp
```

### Debugging

Install clang tools `sudo apt install clang`

Modify `Makefile` chaging **CC** to use one of the clang options and REM out `CC = gcc`

Require running `make clean` in order to compile all libraries using clang



## Contributors
**Special Thanks to the people contributing to this project**

    - Gary Taylor @gravity-addiction
    - Jim Rees @jimrees


## Licence

SDOBOX - Skydive or Bust Touchscreen Software
Copyright (c) 2019-2020

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Except as contained in this notice, the name of the SDOBOX, Skydive Or Bust, or
Strict Development, Inc. shall not be used in advertising or otherwise to promote
the sale, use or other dealings in this Software without prior written
authorization from the Strict Development, Inc. or Gary Taylor.