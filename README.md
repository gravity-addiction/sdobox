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
    - Fast charging circuit / li-po battery
    - *(in development)* Expand storage with nvme
    - *(in development)* External pinouts for attachable mods (DB15 VGA)


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


- build-scripts/


    apt-install.sh - list of apt libraries that are required for development

    init.sh - Goto installer script. Can we re-ran multiple times

    mpv.sh - downloads and installs third party utilities and compiles ffmpeg / mpv

    tslib.sh - downloads and install tslib library

    sdobox.sh - download sdobox development libraries and compiles

    sdl.sh - Downloads modified SDL12 branch and compiles to libSDL for sdobox libs


## Development

Requires compiling on a raspberry pi platform.
```
sudo apt install libiw-dev libsqlite3-dev libulfius-dev libxdo-dev libasound2-dev

### Clone and make repo
```
git clone --recurse-submodules http://github.com/gravity-addiction/sdobox.git
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
