- Information Collected From; https://github.com/ccrisan/motioneyeos/wiki/
# Disabling ACT and PWR LEDs

Remount boot as read-write, then set these values in your /boot/config.txt

# Disable the ACT LED.
dtparam=act_led_trigger=none
dtparam=act_led_activelow=off

# Disable the PWR LED.
dtparam=pwr_led_trigger=none
dtparam=pwr_led_activelow=off
For Raspberry Pi Zero:

# Disable the ACT LED.
dtparam=act_led_trigger=none
dtparam=act_led_activelow=on
As mentioned in Issue #184 and Issue #832

Disabling Ethernet LEDs

For Raspberry Pi 3B+, set these values in your /boot/config.txt:

# Disable the Ethernet LEDs.
dtparam=eth_led0=14
dtparam=eth_led1=14
For other Raspberry Pi models, add this command to your /data/etc/userinit.sh:

lan951x-led-ctl --fdx=0 --lnk=0 --spd=0
Please note that lan951x-led-ctl has not made its way to a stable release yet. You will have to use the nightly-dev for now.

More info in Issue #1528.

Remounting boot as writable

Please note that the boot partition is mounted as read-only by default. If you want to make changes to it, you'll have to remount it as read-write first.

To remount boot as read-write: mount -o remount,rw,noatime,sync /boot

When you are done editing the boot file, you can simply reboot your device and it will be mounted as read-only again.
