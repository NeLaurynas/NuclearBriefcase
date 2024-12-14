#!/bin/bash
while true; do
    # use CTRL + A -> Q to quit
		minicom -D /dev/ttyACM0 -b 230400
    echo "Device disconnected. Trying to reconnect"
    sleep 1
done
