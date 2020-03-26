#!/bin/bash

# boot/config.txt additions (not re-runable)
echo 'dtparam=spi=on
dtparam=i2c1=on
dtparam=12c_arm=on
dtoverlay=waveshare35c:rotate=270
gpu_mem=256
gpio=4=op,dh
gpio=12=op,dh' | sudo tee -a /boot/config.txt

# boot/cmdline.txt wrap existing content with additions (not re-runable)
echo "dwc_otg.lpm_enable=0 $(cat /boot/cmdline.txt) fbcon=map:10 fbcon=font:VGA8x8 logo.nologo loglevel=3 vt.global_cursor_default=0" | sudo tee /boot/cmdline.txt
sudo wget -O /boot/overlays/waveshare35c.dtbo https://raw.githubusercontent.com/gravity-addiction/sdobox/master/scripts/overlays/waveshare35c-overlay.dtb 

