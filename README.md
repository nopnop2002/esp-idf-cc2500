# esp-idf-cc2500
CC2500 Low-Cost Low-Power 2.4 GHz RF Transceiver driver for esp-idf.   

I ported from [this](https://github.com/nopnop2002/Arduino-CC2500).

- 2.00mm pitch External Antena
![CC2500-2mm-EXT-Antena-2](https://user-images.githubusercontent.com/6020549/167327153-d0f6a162-9d2f-448d-b1e2-a591432f435e.JPG)

- 1.27mm pitch PCB Antena
![CC2500-1 27mm-PCB-Antena](https://user-images.githubusercontent.com/6020549/167327177-b06ff78e-08a6-4599-907d-366c79189f1d.JPG)
 Pin out is [here](https://www.rflink.nl/wires.php).   

# Software requirements
esp-idf v4.4 or later.   
This is because this version supports ESP32-C3.   

# Installation

```Shell
git clone https://github.com/nopnop2002/esp-idf-cc2500
cd esp-idf-cc2500/basic
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   


# Configuration for Transceiver   
![config-cc2500-1](https://user-images.githubusercontent.com/6020549/167327207-ae2e51de-b6af-4b0a-9434-1e67dd5c17a5.jpg)
![config-cc2500-2](https://user-images.githubusercontent.com/6020549/167523466-0beb3885-bd7c-4f19-b72d-7f8cf072aae5.jpg)



# SPI BUS selection   
![config-cc2500-3](https://user-images.githubusercontent.com/6020549/167523613-fb3b0b49-982f-4af2-bae6-e3e1b6248fa7.jpg)

The ESP32 series has three SPI BUSs.   
SPI1_HOST is used for communication with Flash memory.   
You can use SPI2_HOST and SPI3_HOST freely.   
When you use SDSPI(SD Card via SPI), SDSPI uses SPI2_HOST BUS.   
When using this module at the same time as SDSPI or other SPI device using SPI2_HOST, it needs to be changed to SPI3_HOST.   
When you don't use SDSPI, both SPI2_HOST and SPI3_HOST will work.   
Previously it was called HSPI_HOST / VSPI_HOST, but now it is called SPI2_HOST / SPI3_HOST.   


# Wirering

|CC2500||ESP32|ESP32-S2/S3|ESP32-C3|
|:-:|:-:|:-:|:-:|:-:|
|MISO|--|GPIO19|GPIO37|GPIO18|
|SCK|--|GPIO18|GPIO36|GPIO10|
|MOSI|--|GPIO23|GPIO35|GPIO19|
|CSN|--|GPIO5|GPIO34|GPIO9|
|GDO0|--|N/C|N/C|N/C|
|GDO2|--|N/C|N/C|N/C|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|

__You can change it to any pin using menuconfig.__   

__Some modules do not have GOD0 and GOD2.__   


# Communication with another library   
Basically, communication between applications with different register values is not guaranteed.   
These settings may differ.   
- Base frequency and channel spacing frequency
- Preamble size
- Combined sync-word qualifier mode
- Packet length configuration
- Manchester encoding configuration
- Wireless modulation method

You need to use [this](https://github.com/nopnop2002/Arduino-CC2500) when communicating with the Arduino environment.

# Comparison of cc2500 and cc1101
||cc2500|cc1101|
|:-:|:-:|:-:|
|Manufacturer|Texas Instrument|Texas Instrument|
|Frequency|2.4GHz|315/433/868/915MHz|
|Maximum Payload|64Byte|64Byte|
|FiFo for reception|64Byte*1|64Byte*1|
|FiFo for transmission|64Byte*1|64Byte*1|
|CRC Length|16bits|16bits|
|Acknowledgement Payload|No|No|
|Available Modulation format|2-FSK/GFSK/OOK/MSK|2-FSK/4-FSK/GFSK/ASK/OOK/MSK|


# Comparison of cc2500 and nRF24L01
||cc2500|nRF24L01|
|:-:|:-:|:-:|
|Manufacturer|Texas Instrument|Nordic Semiconductor|
|Frequency|2.4GHz|2.4GHz|
|Maximum Payload|64Byte|32Byte|
|FiFo for reception|64Byte*1|32Byte*3|
|FiFo for transmission|64Byte*1|32Byte*3|
|CRC Length|16bits|8/16bits|
|Acknowledgement Payload|No|Yes|
|Available Modulation format|2-FSK/GFSK/OOK/MSK|GFSK|

cc2500 cannot tell whether the transmission was successful or unsuccessful on the transmitting side.   
nRF24L01 can tell whether the transmission was successful or unsuccessful on the transmitting side.   

- GFSK  : Gaussian shaped Frequency Shift Keying
- FSK   : Frequency Shift Keying
- 2-FSK : Binary Frequency Shift Keying
- 4-FSK : Quaternary Frequency Shift Keying
- ASK   : Amplitude Shift Keying
- OSK   : On-Off Keying
- MSI   : Minimum Shift Keying

