for device in /sys/block/*
do
    if udevadm info --query=property --path=$device | grep -q ^ID_BUS=usb
    then
        echo $device
    fi
done
