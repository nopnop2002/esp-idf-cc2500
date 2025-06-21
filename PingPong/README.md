# PingPong Example   
Send data from primary to secondary.   
In the secondary, the characters are converted and sent back.   

- ESP32 is Primary   
```
+-----------+           +-----------+             +-----------+           +-----------+
|           |           |           |             |           |           |           |
|  Primary  |===(SPI)==>|  CC2500   |---(Radio)-->|  CC2500   |===(SPI)==>| Secondary |
|   ESP32   |           |           |             |           |           |           |
|           |           |           |             |           |           |           |
|           |<==(SPI)===|           |<--(Radio)---|           |<==(SPI)===|           |
|           |           |           |             |           |           |           |
+-----------+           +-----------+             +-----------+           +-----------+
```

- ESP32 is Secondary   

```
+-----------+           +-----------+             +-----------+           +-----------+
|           |           |           |             |           |           |           |
|  Primary  |===(SPI)==>|  CC2500   |---(Radio)-->|  CC2500   |===(SPI)==>| Secondary |
|           |           |           |             |           |           |   ESP32   |
|           |           |           |             |           |           |           |
|           |<==(SPI)===|           |<--(Radio)---|           |<==(SPI)===|           |
|           |           |           |             |           |           |           |
+-----------+           +-----------+             +-----------+           +-----------+
```

# Configuration   

![config-pingpong-1](https://user-images.githubusercontent.com/6020549/167327687-2eb16aa3-dbf7-4fd7-8dbe-8784c3a23552.jpg)
![config-pingpong-2](https://user-images.githubusercontent.com/6020549/167327690-67151623-071c-443a-8276-cc79dec57667.jpg)

# Communication with the Arduino environment   
- ESP32 is the primary   
I tested it with [this](https://github.com/nopnop2002/esp-idf-cc2500/tree/main/ArduinoCode/CC2500_pong).   

- ESP32 is the secondary   
I tested it with [this](https://github.com/nopnop2002/esp-idf-cc2500/tree/main/ArduinoCode/CC2500_ping).   


# Screen Shot   
![screenShot-pingpong-1](https://user-images.githubusercontent.com/6020549/167327820-efe090da-fe15-4f42-9301-df00e98b86ae.jpg)
![screenShot-pingpong-2](https://user-images.githubusercontent.com/6020549/167327822-bdfb814b-378a-4c51-8ce8-cca5d18c6974.jpg)
