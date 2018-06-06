#!/bin/bash

cd /home/pi/Workspace/HABIL/
raspistill -o photo.jpg --exposure antishake --awb auto --nopreview --width 160 --height 120
convert photo.jpg -quality 45 -colorspace gray photo.jp2
./transmit_photo
rm *.jpg *.jp2
