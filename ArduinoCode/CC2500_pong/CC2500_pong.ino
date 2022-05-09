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
  Serial.println("Receiver Start");
}

void loop(){
  uint8_t txBuf[64] = {0};
  uint8_t rxBuf[64] = {0};
  uint8_t rssi;
  uint8_t lqi;
  int rxLen = radio.listenForPacket(rxBuf, 64, &rssi, &lqi);
  if (rxLen) {
    Serial.print("rxLen: ");
    Serial.println(rxLen);
    for (int i=0;i<rxLen;i++) {
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (int i=0;i<rxLen;i++) {
      char c = rxBuf[i];
      if (c > 0x20 && c < 0xFF) {
        Serial.print(c);
      } else {
        Serial.print(" ");
      }
    }
    Serial.println();

    int dbm;
    if (rssi < 0x7F) {
      dbm = (rssi/2) - 72;
    } else {
      dbm = abs(rssi-256);
      dbm = (dbm/2) - 72;
    }

    Serial.print("RSSI(Raw): 0x");
    Serial.print(rssi, HEX);
    Serial.print(" RSSI(dBm): ");
    Serial.print(dbm);
    Serial.print(" LQI: 0x");
    Serial.println(lqi, HEX);

    for (int i=0;i<rxLen;i++) {
      if (islower(rxBuf[i])) {
        txBuf[i] = toupper(rxBuf[i]);
      } else {
        txBuf[i] = tolower(rxBuf[i]);
      }
    }
    radio.sendPacket(txBuf, rxLen);
  }
  
}
