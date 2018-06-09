#define RF95_INT     3
#define RF95_CS      4
#define RF95_RST     2

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 radio(RF95_CS, RF95_INT);

void resetRadio()
{
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);

  digitalWrite(RF95_RST, LOW);
  delayMicroseconds(100);
  digitalWrite(RF95_RST, HIGH);
  delay(5);
}

void initRadio()
{
  radio.init(); 
  radio.setFrequency(868.0);
  radio.setThisAddress(1);
}

void setup() 
{
  resetRadio();
  initRadio();
}

void loop()
{
  if (!radio.available()) return;   
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (radio.recv(buf, &len)) Serial.write(buf, len);
}
