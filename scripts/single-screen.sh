#!/bin/bash
sudo sed -e '/dtoverlay=vc4-fkms-v3d/ s/^#*/#/' -i /boot/config.txt