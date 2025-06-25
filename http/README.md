# HTTP Example   
This is cc2500 and HTTP gateway application.   
```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  cc2500   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+

            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```



# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/181c5fc5-3df6-4653-b63e-487aee4fb0aa)
![config-app](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/c84d30e1-d14b-4371-948b-ecd8ec725935)

## WiFi Setting
Set the information of your access point.   
![config-wifi](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/aa47a085-f860-43c3-8f60-666e00b7b709)

## Radioi Setting
Set the wireless communication direction.   

### HTTP to Radio
Subscribe with HTTP and send to Radio.   
ESP32 acts as HTTP Server.   
You can use curl as HTTP Client.   
```sh ./http-client.sh```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  cc2500   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![config-radio-1](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/9918096d-3d41-421f-bf8e-2f3b2b116a34)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-cc2500/tree/main/ArduinoCode/CC2500_receive).   

### Radio to HTTP
Receive from Radio and publish as HTTP.   
ESP32 acts as HTTP Client.   
You can use nc(netcat) as HTTP Server.   
```sh ./http-server.sh```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![config-radio-2](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/e1f366e5-7263-414c-b3f9-d97f4b4a5237)


Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-cc2500/tree/main/ArduinoCode/CC2500_receive).   

### Specifying an HTTP Server   
You can specify your HTTP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   
- Fully Qualified Domain Name   
 ```http-server.public.io```


