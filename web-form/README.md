# Web Form Example   
Data entered through the web form is sent to Radio.   
Data received from Radio is displayed in a web form.   
```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
            |  WebForm  |-(WebSocket)->|   ESP32   |----(SPI)---->|  cc2500   |==(Radio)==>
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+

            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
==(Radio)==>|  cc2500   |----(SPI)---->|   ESP32   |-(WebSocket)->|  WebForm  |
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
It's a great job.   

# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/3875a0f1-306f-452e-be0d-b03b3bb87487" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/0efe2e42-e8a8-40fa-bd8c-5581d2999bd8" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/627512f9-afde-4038-af66-a48f95346053" />

## Radio Setting
Set the wireless communication direction.   

### Web to Radio
Data entered through the web form is sent to Radio.   
```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
            |  WebForm  |-(WebSocket)->|   ESP32   |----(SPI)---->|  cc2500   |==(Radio)==>
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/4ef8fd9a-5268-4c66-89dc-a25c8447b8da" />


### Radio to Web
Data received from Radio is displayed in a web form.   

```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
==(Radio)==>|  cc2500   |----(SPI)---->|   ESP32   |-(WebSocket)->|  WebForm  |
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/8f02ad06-82a4-430b-b8c6-72710e7aa400" />

# Launch a web browser
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32-server.local/
```

<img width="947" height="912" alt="Image" src="https://github.com/user-attachments/assets/26c67783-a351-4a34-a60c-6b40b695d485" />
<img width="947" height="912" alt="Image" src="https://github.com/user-attachments/assets/442f629b-85b9-4e96-b9a6-1fe78f65b4cd" />

### Web to Radio
Enter the data to send in the TextBox and press the Send button.   
<img width="947" height="912" alt="Image" src="https://github.com/user-attachments/assets/b9dba574-9844-4b97-8245-ef9764bce406" />

### Radio to Web
The received data will be displayed in the TextBox.   
The Change button changes the number of lines displayed.   
The Copy button copies the received data to the clipboard.   
<img width="947" height="912" alt="Image" src="https://github.com/user-attachments/assets/2d88933c-07fa-436c-9a9b-1e3543d9f68a" />
<img width="947" height="912" alt="Image" src="https://github.com/user-attachments/assets/24c2bae5-a18c-4a5a-a7bc-c7344d74339e" />

# WEB Pages
WEB Pages are stored in the html folder.   
I used [this](https://bulma.io/) open source css.   
You can change root.html as you like.   


