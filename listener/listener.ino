#define RFM69_INT     3
#define RFM69_CS      4
#define RFM69_RST     2

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 radio(RFM69_CS, RFM69_INT);

void resetRadio()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delay(50);
  digitalWrite(2, HIGH);
  delay(50);
}

void initRadio()
{
  Serial.begin(9600);
  while (!Serial);
  if (!radio.init()) Serial.println("Radio init failed");  
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
