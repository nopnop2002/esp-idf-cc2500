# HTTPS Example   
This is cc2500 and HTTPS gateway application.   
Receive from cc2500 and send to HTTPS Server.   
ESP32 acts as HTTPS Client.   

```
            +-----------+           +-----------+            +------------+
            |           |           |           |            |            |
==(Radio)==>|  cc2500   |--(SPI)--->|   ESP32   |--(HTTPS)-->|HTTPS Server|
            |           |           |           |            |            |
            +-----------+           +-----------+            +------------+
```

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-cc2500
cd esp-idf-cc2500/https/
chmod 777 mkkey.sh
./mkkey.sh
idf.py menuconfig
idf.py flash
```

mkkey.sh creates a server certificate file.   
The server certificate file is associated with the HTTPS server's IP address.   
mkkey.sh automatically retrieves the IP address of the server on which the script is executed and treats that address as an HTTPS server.   
To manually configure the HTTPS server's IP address, modify the script as follows:   
```
IP="192.168.0.123"
openssl req -x509 -new -nodes -key server.key -subj "/CN=${IP}" -days 10000 -out server.crt
```


# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/ae541cc4-9bee-44ad-a382-e08c982c813e" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/d9016cda-0848-4257-b873-f168588b4ec7" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/0557d20c-2c02-4f29-8750-7212d593d125" />

## Server Setting
Set the information of your HTTPS server.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/6b1df8d6-6934-4988-a9a7-6b197d936077" />

# Start the HTTPS server
```
python3 https-server.py
```
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/7320a359-40a4-42cf-9fc2-d02b1f116fd4" />

