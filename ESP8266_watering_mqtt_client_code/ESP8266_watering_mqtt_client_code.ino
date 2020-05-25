/*******************/
/** Add your wifi network name and password in lines 23 and 24 **/
/** Add optional MQTT user name and password in lines 91 and 123 **/
/** Add optional MQTT port# in line 109 **/
/*******************/
#include <SoftwareSerial.h>  //this is espSoftwareSerial, not Arduino SoftwareSerial
#include <WiFiServerSecure.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <PubSubClient.h>


SoftwareSerial waterSerial(14,12,false,256);  // RX, TX, inverse_logic, buffer size
                                              //GPIO14 = D5, GPIO12 = D6
                                 
  
// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxx";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "xx.x.x.xxx";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espwateringClient;
PubSubClient client(espwateringClient);

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  int i;

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println();
  Serial.print("Message: ");
  String messageTemp;  
  for (i = 0; i < length; i++) {                                                                                         
    messageTemp += (char)message[i];
  }
  delay(10);
  
  // If an MQTT message is received 
 
  if(topic=="Enable_Pump"){
     if(messageTemp == "1"){
        Serial.print("On");
        waterSerial.print("1");
     }
     else if(messageTemp == "0"){
        Serial.print("Off");
        waterSerial.print("0");
      }
   
  }
  //waterSerial.print('\n');
  Serial.println(); 
}


// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espwateringClient", "xxxxxxxxx", "xxxxxxxxx")) {
      Serial.println("connected"); 
      // Subscribe or resubscribe to a topic
      client.subscribe("Enable_Pump"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}  
  
void setup() {
  waterSerial.begin(9600);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  
   char waterData[80];  // max line length is one less than this
   
   if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("espwateringClient", "xxxxxxxx", "xxxxxxxxxx");

  //where to store the data 
    static char read_A0[5]; 
    static char read_A1[5];
    static char read_A2[5];
    static char read_A3[5];
    static char read_pump[2];  
  
//  get data from serial line
  
    if (read_line(waterData, sizeof(waterData)) < 0) {
      Serial.println("Error: line too long");
      return; // skip command processing and try again on next iteration of loop
      }
     
  String myString = waterData; //change type from char to string
  //Serial.println (myString);

// This parses comma delimited string into substring
int Index1 = myString.indexOf(',');
int Index2 = myString.indexOf(',', Index1+1);
int Index3 = myString.indexOf(',', Index2+1);
int Index4 = myString.indexOf(',', Index3+1);
int Index5 = myString.indexOf(',', Index4+1);

String firstValue = myString.substring(0, Index1);
String secondValue = myString.substring(Index1+1, Index2);
String thirdValue = myString.substring(Index2+1, Index3);  
String fourthValue = myString.substring(Index3+1, Index4); 
String fifthValue = myString.substring(Index4+1, Index5);

firstValue.toCharArray(read_A0, 5);   //convert back to 'char' for PubSub
secondValue.toCharArray(read_A1, 5);
thirdValue.toCharArray(read_A2, 5);
fourthValue.toCharArray(read_A3, 5);
fifthValue.toCharArray(read_pump, 2);

//to print results to serial monitor
delay (100);

Serial.println("Readings:");
Serial.print("A0: ");
Serial.println(firstValue);
Serial.print("A1: ");
Serial.println(secondValue);
Serial.print("A2: ");
Serial.println(thirdValue);
Serial.print("A3: ");
Serial.println(fourthValue);
Serial.print("Pump State: ");
if (fifthValue = '0') {
   Serial.println("Off");
   }
else {
   Serial.println("On");
   } 
Serial.println("");


//publish to mqtt
client.publish("A0_moisture", read_A0);
client.publish("A1_moisture", read_A1);
client.publish("A2_moisture", read_A2);
client.publish("A3_moisture", read_A3);
client.publish("Pump State", read_pump); 

}

      
int read_line(char* buffer, int bufsize)
{
  for (int index = 0; index < bufsize; index++) {
    // Wait until characters are available
    while (waterSerial.available() == 0) {
    }

    char ch = waterSerial.read(); // read next character

    if (ch == '\n') {
      buffer[index] = 0; // end of line reached: null terminate string
      return index; // success: return length of string (zero if string is empty)
    }

    buffer[index] = ch; // Append character to buffer
  }

  // Reached end of buffer, but have not seen the end-of-line yet.
  // Discard the rest of the line (safer than returning a partial line).

  char ch;
  do {
    // Wait until characters are available
    while (waterSerial.available() == 0) {
    }
    ch = waterSerial.read(); // read next character (and discard it)
  } while (ch != '\n');
  buffer[0] = 0; // set buffer to empty string even though it should not be used
  return -1; // error: return negative one to indicate the input was too long
}
      

    
    

       
