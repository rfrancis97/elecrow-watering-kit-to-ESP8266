# elecrow-watering-kit-to-ESP8266
I wanted to modify the code for the Elecrow Watering kit to send moisture data via MQTT to Node-RED or Home Assistant using an ESP8266 NodeMCU wifi.
It is necessary to have an Elecrow Leonardo Watering Kit relay board that has the correct Arduino uploader to modify the existing code. I had to specifically request a replacement.
I used voltage divider to reduce 5v Leonardo Serial1 output to 3.3v ESP8266 input.
Leonardo RX (Serial1) on 8-pin header should connect to ESP8266 RX pin. I used espSoftSerial library and pins D5(GPIO14) and D6(GPIO12) on esp8266 Node MCU.

Future enhancements to watch out for...
1. Sending status of water valves and water pump.
2. Turning on water valves and water pump from MQTT.
3. Check reserve water tank level so as not to run pump dry.
