# HABIL
High Altitude Balloon Imaging via LoRa

<img align="left" src="https://user-images.githubusercontent.com/45521946/150144502-5fff15da-d471-42c2-a519-86026ab1a955.jpg" />

<img align="left" src="https://user-images.githubusercontent.com/45521946/150144525-8228c1e6-7580-4b02-9c28-e8a1ff2e119d.jpg" />

<img align="left" src="https://user-images.githubusercontent.com/45521946/150144532-78f5ce1d-0179-406c-8c0d-c8223d24423b.jpg" />

<img src="https://user-images.githubusercontent.com/45521946/150144546-0e90a902-17e8-44f2-8686-fa6ff85da95d.jpg" />

<sub>* Actual photos transmitted from our high altitude balloon.</sub>

# Setup

1. Install imagemagick for the image processing/conversion to JPEG 2000:

    `sudo apt-get install imagemagick`

2. Next compile/install the bcm2835 library from http://www.airspayce.com/mikem/bcm2835/

3. Initialize the RadioHead submodule for the RF95 radio:

    `git submodule update --init`

4. Compile the program

    `make`

5. Next paste the line from the cron file into your crontab:

    `sudo crontab -e`

6. Done!
