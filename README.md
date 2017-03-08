MICROGEAR SHIELD
========

This is ESP8266 Firmware base on FREERTOS SDK. 
Main objective is to build hardware microgear board to support NETPIE IOT platform
and to liberate microgear from various platform compatible issue.


You can find latest version of FREERTOS SDK here -> https://github.com/espressif/ESP8266_RTOS_SDK
To use this board effectively used it with NETPIE IOT Platform -> https://netpie.io/

Features
========
- Real time base firmware 
- Stand alone base on **AT COMMAND**
- MQTT module: 
    + NETPIE compatible
    + Support subscribing, publishing, authentication (it should be a fully functional client).
    + Support SSL (In FUTURE)
    + Easy to setup and use
- TCP module:
    + Support ALL mandatory functions eg.connect, print, flush 
    + Easy to used API
    + Support SSL (In FUTURE)
    + **Support multiple connection (to multiple hosts).**
    
Installations
========
**1. Clone this project:**
```bash
git clone https://github.com/moun1234/Microgear-Shield-RTOS
cd espduino
```
**2. FLASH TO ESP8266 USING MAKE:**








