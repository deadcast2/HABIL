#include <bcm2835.h>
#include <RH_RF95.h>
#include <stdio.h>

#define RF_CS_PIN  		RPI_V2_GPIO_P1_24
#define RF_RST_PIN 		RPI_V2_GPIO_P1_15
#define RF_FREQUENCY	868.00
#define RF_GATEWAY_ID	1 
#define RF_NODE_ID		10

void resetRadio()
{
	pinMode(RF_RST_PIN, OUTPUT);
	digitalWrite(RF_RST_PIN, LOW);
	bcm2835_delay(150);
	digitalWrite(RF_RST_PIN, HIGH);
	bcm2835_delay(100);
}

void configureRadio(RH_RF95 handle)
{
	handle.setFrequency(RF_FREQUENCY);
	handle.setThisAddress(RF_NODE_ID);
	handle.setHeaderFrom(RF_NODE_ID);
	handle.setHeaderTo(RF_GATEWAY_ID);
}

int main()
{
	if(!bcm2835_init())
	{
		fprintf(stderr, "Could not initialize BCM2835\n");
		return 1;
	}
	
	RH_RF95 rf95(RF_CS_PIN);
	resetRadio();
	
	if(!rf95.init())
	{
		fprintf(stderr, "RF95 module initialization failed\n");
		return 1;
	}

	configureRadio(rf95);

	uint8_t data[] = "Message from transmitter!!!";
	rf95.send(data, sizeof(data));
	
	bcm2835_close();
	return 0;
}
