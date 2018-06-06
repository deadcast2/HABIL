# HABIL
High Altitude Balloon Imaging via LoRa

# Setup

1. Install imagemagick for the image processing/conversion to JPEG 2000:

`sudo apt-get install imagemagick`

2. Next compile/install the bcm2835 library from http://www.airspayce.com/mikem/bcm2835/

3. Initialize the RadioHead submodule for the RF95 radio:

`git submodule update --init`

3. Compile the program

`make`

4. Next paste the line from the cron file into your crontab:

`sudo crontab -e`

5. Done!
