/*******************/
/** Add your wifi network name and password in lines 23 and 24 **/
/** Add optional MQTT user name and pssword in lines 58 and 87 **/
/** Add optional MQTT port# in line 75 **/
/*******************/
#include <SoftwareSerial.h>  //this is espSoftSerial, not Arduino SoftSerial
#include <WiFiServerSecure.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <PubSubClient.h>



SoftwareSerial waterSerial(14,12,false,128);  // RX, TX, inverse_logic, buffer size
                                              //GPIO14 = D5, GPIO12 = D6
  
// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "<wifi network name>";
const char* password = "<wifi password>";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "xxx.xxx.xxx.xxx";

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


// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espwateringClient", "<mqttuse>", "<mqtt password>")) {
      Serial.println("connected");  
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
  client.setServer(mqtt_server, <mqtt server port>);
}


void loop()
{
   char waterData[80];  // max line length is one less than this
   
   if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("espwateringClient", "<mqttuse>", "<mqtt password>");

  //where to store the data 
    static char read_A0[5]; 
    static char read_A1[5];
    static char read_A2[5];
    static char read_A3[5];  
   
   
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

String firstValue = myString.substring(0, Index1);
String secondValue = myString.substring(Index1+1, Index2);
String thirdValue = myString.substring(Index2+1, Index3);  
String fourthValue = myString.substring(Index3+1, Index4); 

firstValue.toCharArray(read_A0, 5);   //convert back to 'char' for PubSub
secondValue.toCharArray(read_A1, 5);
thirdValue.toCharArray(read_A2, 5);
fourthValue.toCharArray(read_A3, 5);


//to print results to serial monitor
Serial.println("Readings:");
Serial.print("A0: ");
Serial.println(firstValue);
Serial.print("A1: ");
Serial.println(secondValue);
Serial.print("A2: ");
Serial.println(thirdValue);
Serial.print("A3: ");
Serial.println(fourthValue);
Serial.println("");

//publish to mqtt
client.publish("A0_moisture", read_A0);
client.publish("A1_moisture", read_A1);
client.publish("A2_moisture", read_A2);
client.publish("A3_moisture", read_A3);  

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
      

    
    

       
