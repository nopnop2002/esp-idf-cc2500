# SSL Example   
This is cc2500 and SSL gateway application.   
Receive from cc2500 and send to SSL Server.   
ESP32 acts as SSL Client.   

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |--(SSL)--->| SSL Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-cc2500
cd esp-idf-cc2500/ssl/
chmod 777 mkkey.sh
./mkkey.sh
idf.py menuconfig
idf.py flash
```

mkkey.sh creates a server certificate file.   
The server certificate file is associated with the SSL server's IP address.   
mkkey.sh automatically retrieves the IP address of the server on which the script is executed and treats that address as an SSL server.   
To manually configure the SSL server's IP address, modify the script as follows:   
```
IP="192.168.0.123"
openssl req -x509 -new -nodes -key server.key -subj "/CN=${IP}" -days 10000 -out server.crt
```


# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/d283500e-f01d-4582-b775-b14229cede0f" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/c3e56b08-7b02-4468-ab96-5e3195329d41" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/db00d7e0-8bb9-4410-9e94-ddb1cf3f6348" />

## Server Setting
Set the information of your SSL server.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/b299f5dc-4846-42d5-a100-6378f39cbc82" />

# Start the SSL server
- C language
	```
	cd clang-tls-communication
	make
	./server
	```
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/70ff80bb-598a-4505-9982-10885e38779a" />

- python script
	```
	cd python-tls-communication
	python3 server.py
	```
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/799a5196-07a2-4b3e-9b42-d6b36473aa09" />

