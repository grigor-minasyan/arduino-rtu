#!/bin/bash

#a simple process to check for the connection
while true
do
	echo "y" > /dev/udp/192.168.1.103/8888
	sleep 1
done
