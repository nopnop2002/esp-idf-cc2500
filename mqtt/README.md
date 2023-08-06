# MQTT Example   
This is cc2500 and MQTT gateway application.   
```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
            |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  cc2500  |==(Radio)==>
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+

            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
==(Radio)==>|  cc2500  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```



# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/c3805ad6-4584-45ff-810c-1eeb001af41a)
![config-app](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/823d7340-0069-47f8-821b-53bd36ec6099)


## WiFi Setting

![config-wifi](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/ed00b25b-adc5-4ca7-b5d9-2cc763af0eed)

## Radioi Setting

### MQTT to Radio
Subscribe with MQTT and send to Radio.   
You can use mosquitto_pub as Publisher.   
```sh ./mqtt_pub.sh```

```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
            |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  cc2500  |==(Radio)==>
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```

![config-radio-1](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/a5592b19-735b-4352-939a-a756fba4b601)


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC2500_receive   


### Radio to MQTT
Receive from Radio and publish as MQTT.   
You can use mosquitto_sub as Subscriber.   
```sh ./mqtt_sub.sh```

```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
==(Radio)==>|  cc2500  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```

![config-radio-2](https://github.com/nopnop2002/esp-idf-cc2500/assets/6020549/96639115-b5f2-4c36-9222-7773ede93c4e)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC2500_transmitte   


### Specifying an MQTT Broker   
MQTT broker is specified by one of the following.   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```mqtt-broker.local```   
- Fully Qualified Domain Name   
 ```broker.emqx.io```

