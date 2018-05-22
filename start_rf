#!/bin/bash

cd interpreter
make
cd ../driver
make
sudo rmmod cdc_acm
cd driver
sudo insmod arduino_driver.ko
sudo chmod 666 /dev/arduino0
