raspistill -o photo.jpg --exposure antishake --awb auto --hflip --vflip --nopreview --width 160 --height 120
convert photo.jpg -quality 45 -colorspace gray photo.jp2
rm photo.jpg
sudo ./transmitter