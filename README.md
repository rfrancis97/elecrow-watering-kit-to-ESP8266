# elecrow-watering-kit-to-ESP8266
I wanted to modify the code for the Elecrow Watering kit to read moisture data and control pump via MQTT to Node-RED or Home Assistant using an ESP8266 NodeMCU wifi.
It is necessary to have an Elecrow Leonardo Watering Kit relay board that has the correct Arduino uploader to modify the existing code. I had to specifically request a replacement.
I used voltage divider to reduce 5v Leonardo Serial1 output to 3.3v ESP8266 input.
Leonardo RX (Serial1) on 8-pin header should connect to ESP8266 RX pin. I used espSoftSerial library and pins D5(GPIO14) and D6(GPIO12) on esp8266 Node MCU. Modifying the Leonardo code and attaching esp8266 accomplishes the following:

1. Read status of (4) moisure sensors and pump via mqtt.
2. Ability to enable/disable pump via mqtt.
3. Using ultrasonic sensor HC-SR04, check reserve water tank level so pump does not run when dry.
