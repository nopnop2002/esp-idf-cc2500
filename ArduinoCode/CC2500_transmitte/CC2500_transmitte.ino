/**
 * CC2500 Sample transceiver code
 * Ported from https://github.com/yasiralijaved/Arduino-CC2500-Library
 * Written by Zohan SonZohan@gmail.com
 *
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCLK/SCK -> 13
 * CSN/SS - > 10
 */
#include "cc2500.h" // https://github.com/nopnop2002/Arduino-CC2500

//cc2500 radio;
cc2500 radio(10); // CS

void setup(){
  Serial.begin(115200);
  Serial.println("Initializing Wireless..");
  uint8_t channel = 0;
  if (radio.begin(channel) == false) { 
    Serial.println("Device Not Installed");
    while(1);
  }
  Serial.println("Trasnmitter Start");
}

void loop(){
  static uint8_t sendData = 0x00;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  uint8_t txBuf[64];
  int txLen = 32;
  long sendInterval = 1000; // in milliseconds

  if(currentMillis - previousMillis > sendInterval) {
    previousMillis = currentMillis;   
    for (int i=0;i<txLen;i++) txBuf[i] = sendData;
    radio.sendPacket(txBuf, txLen);
    sendData++;
  }
  //radio.listenForPacket();
}
