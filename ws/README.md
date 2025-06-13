# WebSocket Example   
This is cc2500 and WebSocket gateway application.   
```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  cc2500   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+

            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```



# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/a194da17-eec2-4872-b6c8-a2e6a039e7f5)
![config-app](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/a5a16ab7-beb1-4ec2-9ba7-a66b8c8c207b)

## WiFi Setting
Set the information of your access point.   
![config-wifi](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/321a847f-ec71-4254-b954-babdfd111782)

## Radioi Setting
Set the wireless communication direction.   

### WS to Radio
Subscribe with WebSocket and send to Radio.   
ESP32 acts as WebSocket Server.   
You can use ws-client.py as WS Client.   
```python3 ws-client.py```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  cc2500   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![config-radio-1](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/2dc9aa66-d8e6-46bf-a82d-323a656d948b)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC2500_receive   


### Radio to WS
Receive from Radio and publish as WebSocket.   
ESP32 acts as WebSocket Client.   
Use [this](https://components.espressif.com/components/espressif/esp_websocket_client) component.   
You can use ws-server.py as WS Server.   
```python3 ws-server.py```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![config-radio-2](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/96dd6e01-296c-43c8-85fa-c4b3305934f7)


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC2500_transmitte   


### Specifying an WebSocket Server   
You can specify your WebSocket Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```ws-server.local```   
- Fully Qualified Domain Name   
 ```ws-server.public.io```


