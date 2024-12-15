#!/bin/bash
while true; do
    # use CTRL + A -> CTRL + X to quit
		picocom --no-init /dev/ttyACM0 -b 230400
    echo "Device disconnected. Trying to reconnect"
    sleep 1
done
