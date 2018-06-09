#include <bcm2835.h>
#include <RH_RF95.h>
#include <stdio.h>

#define RF_CS_PIN RPI_V2_GPIO_P1_24
#define RF_RST_PIN RPI_V2_GPIO_P1_15
#define RF_FREQUENCY 868.00
#define RF_GATEWAY_ID 1
#define RF_NODE_ID 10

void resetRadio()
{
    pinMode(RF_RST_PIN, OUTPUT);
    digitalWrite(RF_RST_PIN, LOW);
    bcm2835_delay(150);
    digitalWrite(RF_RST_PIN, HIGH);
    bcm2835_delay(100);
}

RH_RF95 *configureRadio(RH_RF95 *radio)
{
    radio->setFrequency(RF_FREQUENCY);
    radio->setTxPower(23, false);
    radio->setThisAddress(RF_NODE_ID);
    radio->setHeaderFrom(RF_NODE_ID);
    radio->setHeaderTo(RF_GATEWAY_ID);
    return radio;
}

RH_RF95 *getRadio()
{
    if(!bcm2835_init())
    {
        fprintf(stderr, "Error loading the BCM2835 library\n");
        return NULL;
    }

    RH_RF95 *radio = new RH_RF95(RF_CS_PIN);
    resetRadio();

    if(!radio->init())
    {
        fprintf(stderr, "Error initializing the radio\n");
        return NULL;
    }

    return configureRadio(radio);
}

int main()
{
    RH_RF95 *radio = getRadio();
    if(radio == NULL) return 1;
    
    FILE *latestPhoto = fopen("photo.jp2", "rb");
    if(!latestPhoto) return 1;

    // Begin transmission
    const uint8_t *beginSignal = "begin";
    radio->send(beginSignal, sizeof(beginSignal));
    bcm2835_delay(5000);
    
    // Send captured photo data
    uint8_t payloadBuffer[RH_RF95_MAX_MESSAGE_LEN];
    while(fread(payloadBuffer, 1, sizeof(payloadBuffer), latestPhoto))
    {
        radio->send(payloadBuffer, sizeof(payloadBuffer));
        bcm2835_delay(5000);
    }

    // End transmission
    const uint8_t *endSignal = "end";
    radio->send(endSignal, sizeof(endSignal));
    bcm2835_delay(5000);
    
    // Cleanup
    fclose(latestPhoto);
    free(radio);
    return !bcm2835_close();
}
