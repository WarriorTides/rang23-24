#!/bin/bash

export PI_PORT "1-1.1"
usbip bind --busid=${PI_PORT}
usbipd
