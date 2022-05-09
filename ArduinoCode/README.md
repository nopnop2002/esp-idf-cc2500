# Example of Arduino environment   
I use [this](https://github.com/nopnop2002/Arduino-CC2500).   

# Wirering

|CC2500||UNO|MEGA|ESP8266|
|:-:|:-:|:-:|:-:|:-:|
|GND|--|GND|GND|GND|
|CSN|--|D10(*2)|D10(*2)|IO15|
|MISO|--|D12|D50|IO12|
|SCLK|--|D13(*2)|D52(*2)|IO14|
|MOSI|--|D11(*2)|D51(*2)|IO13|
|VCC|--|3.3V(*1)|3.3V|3.3V|

(*1)   
UNO's 3.3V output can only supply 50mA.   
In addition, the output current capacity of UNO-compatible devices is smaller than that of official products.   
__So this module may not work normally when supplied from the on-board 3v3.__   

(*2)   
CC2500 is not 5V tolerant.   
You need level shift from 5V to 3.3V.   
I used [this](https://www.ti.com/lit/ds/symlink/txs0108e.pdf?ts=1647593549503) for a level shift.   

