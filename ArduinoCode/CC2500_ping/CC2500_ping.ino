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
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  uint8_t txBuf[64];
  uint8_t rxBuf[64];
  int txLen;
  uint8_t rssi;
  uint8_t lqi;
  long sendInterval = 1000; // in milliseconds

  if(currentMillis - previousMillis > sendInterval) {
    previousMillis = currentMillis;
    txLen = sprintf((char *)txBuf, "Hello World %ld", currentMillis);
    Serial.print("txLen: ");
    Serial.println(txLen);
    radio.sendPacket(txBuf, txLen);

    // Wait for a response from the other party 
    bool waiting = true;
    unsigned long startMillis = millis();

    while(waiting) {
      int rxLen = radio.listenForPacket(rxBuf, 64, &rssi, &lqi);
      if (rxLen) {
        rxBuf[rxLen] = 0;
        Serial.print("rxLen: ");
        Serial.println(rxLen);

        for (int i=0;i<txLen;i++) {
          char c = txBuf[i];
          Serial.print(c);
        }
        
        Serial.print(" --> ");
        
        for (int i=0;i<rxLen;i++) {
          char c = rxBuf[i];
          Serial.print(c);
        }
        Serial.println();
        waiting = false;
      }
      unsigned long diffMillis = millis() - startMillis;
      if (diffMillis > 1000) {
        Serial.println("No responce from others");
        waiting = false;
      }
    } // end while
  } // end if
  //radio.listenForPacket();
}
