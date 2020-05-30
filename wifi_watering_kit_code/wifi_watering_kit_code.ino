#include <Wire.h>
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);    // I2C
#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;

// set all moisture sensors PIN ID
int moisture1 = A0;
int moisture2 = A1;
int moisture3 = A2;
int moisture4 = A3;

// declare moisture values
int moisture1_value = 0;
int moisture2_value = 0;
int moisture3_value = 0;
int moisture4_value = 0;

// set water relays
int relay1 = 6;
int relay2 = 8;
int relay3 = 9;
int relay4 = 10;

// set water pump
int pump = 4;

// set button
int button = 12;

//pump state    1:open   0:close
int pump_state_flag = 0;

//relay1 state    1:open   0:close
int relay1_state_flag = 0;

//relay2 state   1:open   0:close
int relay2_state_flag = 0;

//relay3 state  1:open   0:close
int relay3_state_flag = 0;

//relay4 state   1:open   0:close
int relay4_state_flag = 0;

//enable pump   1 = enabled   0 = disabled
int enable_pump = 1;

//Measuring water level with ultrasonic sensor HC-SR04
int trig = 1;   //pin D7
int echo = A4;  //pin A4

static unsigned long currentMillis_send = 0;
static unsigned long  Lasttime_send = 0;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat",};
unsigned long nowtime;
unsigned long endtime;
unsigned long nowtimeNext;
unsigned long nowtime1;
unsigned long endtime1;
unsigned long nowtimeNext1;
unsigned long nowtime2;
unsigned long endtime2;
unsigned long nowtimeNext2;
unsigned long nowtime3;
unsigned long endtime3;
unsigned long nowtimeNext3;


// good flower
unsigned char bitmap_good[] U8G_PROGMEM = {

  0x00, 0x42, 0x4C, 0x00, 0x00, 0xE6, 0x6E, 0x00, 0x00, 0xAE, 0x7B, 0x00, 0x00, 0x3A, 0x51, 0x00,
  0x00, 0x12, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x06, 0x40, 0x00, 0x00, 0x06, 0x40, 0x00,
  0x00, 0x04, 0x60, 0x00, 0x00, 0x0C, 0x20, 0x00, 0x00, 0x08, 0x30, 0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0xE0, 0x0F, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0xC1, 0x00, 0x00, 0x0E, 0x61, 0x00,
  0x00, 0x1C, 0x79, 0x00, 0x00, 0x34, 0x29, 0x00, 0x00, 0x28, 0x35, 0x00, 0x00, 0x48, 0x17, 0x00,
  0x00, 0xD8, 0x1B, 0x00, 0x00, 0x90, 0x1B, 0x00, 0x00, 0xB0, 0x09, 0x00, 0x00, 0xA0, 0x05, 0x00,
  0x00, 0xE0, 0x07, 0x00, 0x00, 0xC0, 0x03, 0x00
};

// bad flower
unsigned char bitmap_bad[] U8G_PROGMEM = {
  0x00, 0x80, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xE0, 0x0D, 0x00, 0x00, 0xA0, 0x0F, 0x00,
  0x00, 0x20, 0x69, 0x00, 0x00, 0x10, 0x78, 0x02, 0x00, 0x10, 0xC0, 0x03, 0x00, 0x10, 0xC0, 0x03,
  0x00, 0x10, 0x00, 0x01, 0x00, 0x10, 0x80, 0x00, 0x00, 0x10, 0xC0, 0x00, 0x00, 0x30, 0x60, 0x00,
  0x00, 0x60, 0x30, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x60, 0x07, 0x00, 0x00, 0x60, 0x00, 0x00,
  0x00, 0x60, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xC7, 0x1C, 0x00,
  0x80, 0x68, 0x66, 0x00, 0xC0, 0x33, 0x7B, 0x00, 0x40, 0xB6, 0x4D, 0x00, 0x00, 0xE8, 0x06, 0x00,
  0x00, 0xF0, 0x03, 0x00, 0x00, 0xE0, 0x00, 0x00
};

// Elecrow Logo
  static unsigned char bitmap_logo[] U8G_PROGMEM ={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xE0,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x04,0xF8,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x08,0xFE,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x10,0x1F,0xE0,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xB0,0x07,0x80,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xE0,0x03,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xC0,0x00,0x00,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x80,0x01,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x60,0x23,0x00,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x70,0xC7,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x70,0x9E,0x0F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x70,0x3C,0xFE,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x70,0x78,0xF8,0x7F,0xF0,0x9F,0x07,0xFE,0x83,0x0F,0xFF,0x00,0x77,0x3C,0x18,0x1C,
  0x70,0xF0,0xE1,0x3F,0xF1,0x9F,0x07,0xFE,0xE1,0x1F,0xFF,0xC3,0xF7,0x3C,0x38,0x0C,
  0x70,0xE0,0x87,0x8F,0xF1,0xC0,0x07,0x1E,0x70,0x3C,0xCF,0xE3,0xE1,0x7D,0x3C,0x0E,
  0x70,0xD0,0x1F,0xC0,0xF1,0xC0,0x03,0x1F,0x78,0x3C,0xCF,0xE3,0xE1,0x7D,0x3C,0x06,
  0xF0,0xB0,0xFF,0xF1,0xF0,0xC0,0x03,0x0F,0x78,0x3C,0xCF,0xF3,0xE0,0x7B,0x3E,0x06,
  0xF0,0x60,0xFF,0xFF,0xF0,0xC6,0x03,0xEF,0x3C,0x80,0xEF,0xF1,0xE0,0x7B,0x3E,0x03,
  0xF0,0xE1,0xFC,0xFF,0xF8,0xCF,0x03,0xFF,0x3C,0x80,0xFF,0xF0,0xE0,0x7B,0x7B,0x01,
  0xE0,0xC3,0xF9,0x7F,0x78,0xC0,0x03,0x0F,0x3C,0x80,0xF7,0xF1,0xE0,0xF9,0xF9,0x01,
  0xE0,0x83,0xE3,0x7F,0x78,0xE0,0x03,0x0F,0x3C,0xBC,0xE7,0xF1,0xE0,0xF9,0xF9,0x00,
  0xC0,0x0F,0x8F,0x3F,0x78,0xE0,0x81,0x0F,0x3C,0x9E,0xE7,0xF1,0xE0,0xF1,0xF8,0x00,
  0x80,0x3F,0x1E,0x00,0x78,0xE0,0x81,0x07,0x38,0x9E,0xE7,0xF1,0xF0,0xF0,0x78,0x00,
  0x80,0xFF,0xFF,0x00,0xF8,0xEF,0xBF,0xFF,0xF8,0xCF,0xE7,0xE1,0x7F,0x70,0x70,0x00,
  0x00,0xFF,0xFF,0x0F,0xF8,0xEF,0xBF,0xFF,0xE0,0xC3,0xE3,0x81,0x1F,0x70,0x30,0x00,
  0x00,0xFC,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xF8,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xE0,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };

static unsigned char bitmap_T[] U8G_PROGMEM = {
  0xF7, 0x01, 0x1D, 0x03, 0x0B, 0x02, 0x0C, 0x02, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x08, 0x02,
  0x18, 0x03, 0xF0, 0x01
};

static unsigned char bitmap_H[] U8G_PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F, 0xF8, 0x1F, 0xFC, 0x3F,
  0xFC, 0x3F, 0xFE, 0x7F, 0xEE, 0x7F, 0xB3, 0xF7, 0xBB, 0xFB, 0xBB, 0xFD, 0xBB, 0xFD, 0xC7, 0xFE,
  0x7F, 0xC3, 0x3F, 0xDD, 0xBF, 0xFD, 0xDF, 0xDD, 0xEE, 0x5B, 0xFE, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F,
  0xE0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup()
{
  draw_elecrow();
  delay(2000);
  Wire.begin();
  RTC.begin();
  Serial.begin(9600);
  Serial1.begin(9600); // Serial to ESP8266. Use RX & TX pins of Elecrow relay board
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  // declare relay as output
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  // declare pump as output
  pinMode(pump, OUTPUT);
  // declare switch as input
  pinMode(button, INPUT);
  //pinMode(ROTARY_ANGLE_SENSOR, INPUT);
  // water_flower();
}

void loop()
{ 
  char mqttData[1];   // buffer for MQTT data received (pump enabled) 
  long t = 0, h = 0, hp = 0; //for Measuring water level
         
  /******** Check for MQTT message from ESP8266 ******************/
  /** You can disable the pump by setting enable_pump FALSE **/
  /** The pump can also be disabled remotely by sending mqtt topic **/
  /** "Set_Pump" from UI with payload FALSE **/
  /** The pump is automatically disabled if water tank level low **/

  // read water level and shut off pump is too low
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;
  h = h - 6;  // offset correction (adjust as necessary for your water tank)
  h = 50 - h;  // water height, 0 - 50 cm
  hp = 2 * h;  // distance in %, 0-100 %
  if (Serial1.available() != 0) {
     mqttData[1] = Serial1.read(); // read next character
     }
     if ((mqttData[1] == '1') && (hp >= 0)) {
        enable_pump = 1;
        }
     else if ((mqttData[1] == '0') || (hp < 0)) {
        enable_pump = 0;
        }  

  // read water level and shut off pump is too low
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;
  h = h - 6;  // offset correction  (adjust as necessary for your water tank)
  h = 50 - h;  // water height, 0 - 50 cm
  hp = 2 * h;  // distance in %, 0-100 %
  if (hp < 0) {
    enable_pump = 0;
    }
  else if (hp >= 0) {
    enable_pump = 1;
    }
   
  // read the value from the moisture sensors:
  read_value(); 
  if (enable_pump == 1)  {
     water_flower();  // use existing timer function or RTC to control watering
     }
  int button_state = digitalRead(button);
  if (button_state == 1)
  {
    read_value();
    u8g.firstPage();
    do
    {
      drawTH();
      drawflower();
    } while ( u8g.nextPage() );
  }
  else
  {
    u8g.firstPage();
    do
    {
      drawtime();
      u8g.drawStr(8, 55 , "www.elecrow.com");
    } while (u8g.nextPage());
  }
}
  

//Set moisture value
void read_value()
{
/**************These is for resistor moisture sensor***********
  float value1 = analogRead(A0);
  moisture1_value = (value1 * 120) / 1023; delay(20);
  float value2 = analogRead(A1);
  moisture2_value = (value2 * 120) / 1023; delay(20);
  float value3 = analogRead(A2);
  moisture3_value = (value3 * 120) / 1023; delay(20);
  float value4 = analogRead(A3);
  moisture4_value = (value4 * 120) / 1023; delay(20);
 **********************************************************/
/************These is for capacity moisture sensor*********/
/*********Read and map analog moisture values 0%-100%******/
 float value1 = analogRead(A0);
  moisture1_value =map(value1,590,360,0,100); delay(20);
  if(moisture1_value<0){
    moisture1_value=0;
  }
  float value2 = analogRead(A1);
  moisture2_value =map(value2,600,360,0,100); delay(20);
  if(moisture2_value<0) {
    moisture2_value=0;
  }
  float value3 = analogRead(A2);
  moisture3_value =map(value3,600,360,0,100); delay(20);
  if(moisture3_value<0){
    moisture3_value=0;
  }
  float value4 = analogRead(A3);
  moisture4_value =map(value4,600,360,0,100); delay(20);
  if(moisture4_value<0) {
    moisture4_value=0;
  }
  static char A0_sensor[5]; 
  static char A1_sensor[5];
  static char A2_sensor[5];
  static char A3_sensor[5];
  static char pump_state[2];

  dtostrf(moisture1_value, 4, 0, A0_sensor);
  dtostrf(moisture2_value, 4, 0, A1_sensor);
  dtostrf(moisture3_value, 4, 0, A2_sensor);
  dtostrf(moisture4_value, 4, 0, A3_sensor);
  dtostrf(pump_state_flag, 1, 0, pump_state);

  /*********Output Moisture Sensor values to ESP8266******/
  Serial1.print(A0_sensor);
  Serial1.print(",");
  Serial1.print(A1_sensor);
  Serial1.print(",");
  Serial1.print(A2_sensor);
  Serial1.print(",");
  Serial1.print(A3_sensor);
  Serial1.print(",");
  Serial1.print(pump_state);
  Serial1.print("\n");
  delay(2000);
  
  /* Optional - to display on Arduino serial monitor */
 /*
  Serial.print("A0: ");
  Serial.print(A0_sensor);
  Serial.print(", ");
  Serial.print("A1: ");
  Serial.print(A1_sensor);
  Serial.print(", ");
  Serial.print("A2: ");
  Serial.print(A2_sensor);
  Serial.print(", ");
  Serial.print("A3: ");
  Serial.print(A3_sensor);
  Serial.print(", ");
  Serial.print("A3: ");
  Serial.print("Pump: ");
  Serial.print(pump_state);
  Serial.println();
  delay(10);  
  */
 }
 
void water_flower()
{
  if (moisture1_value < 30)
  {
    digitalWrite(relay1, HIGH);
    relay1_state_flag = 1;
    delay(50);
    if ((pump_state_flag == 0) && (enable_pump == 1))
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture1_value > 55)
  {
    digitalWrite(relay1, LOW);
    relay1_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

  if (moisture2_value < 30)
  {
    digitalWrite(relay2, HIGH);
    relay2_state_flag = 1;
    delay(50);
    if ((pump_state_flag == 0) && (enable_pump == 1))
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture2_value > 55)
  {
    digitalWrite(relay2, LOW);
    relay2_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

  if (moisture3_value < 30)
  {
    digitalWrite(relay3, HIGH);
    relay3_state_flag = 1;
    delay(50);
    if ((pump_state_flag == 0) && (enable_pump == 1))
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture3_value > 55)
  {
    digitalWrite(relay3, LOW);
    relay3_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

  if (moisture4_value < 30)
  {
    digitalWrite(relay4, HIGH);
    relay4_state_flag = 1;
    delay(50);
    if ((pump_state_flag == 0) && (enable_pump == 1))
    {
      digitalWrite(pump, HIGH);
      pump_state_flag = 1;
      delay(50);
    }
  }
  else if (moisture4_value > 55)
  {
    digitalWrite(relay4, LOW);
    relay4_state_flag = 0;
    delay(50);
    if ((relay1_state_flag == 0) && (relay2_state_flag == 0) && (relay3_state_flag == 0) && (relay4_state_flag == 0))
    {
      digitalWrite(pump, LOW);
      pump_state_flag = 0;
      delay(50);
    }
  }

}



  void draw_elecrow(void){

  u8g.setFont(u8g_font_gdr9r);
  u8g.drawStr(8,55 , "www.elecrow.com");
  u8g.drawXBMP(0, 5,128,32, bitmap_logo);
  }


void drawtime(void)
{
  int x = 5;
  float i = 25.00;
  float j = 54;
  DateTime now = RTC.now();
  //Serial.print(now.year(), DEC);
  if (! RTC.isrunning())
  {
    u8g.setFont(u8g_font_6x10);
    u8g.setPrintPos(5, 20);
    u8g.print("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  else
  {
    u8g.setFont(u8g_font_7x13);
    u8g.setPrintPos(x, 11);
    u8g.print(now.year(), DEC);
    u8g.setPrintPos(x + 80, 11);
    u8g.print(daysOfTheWeek[now.dayOfTheWeek()]);
    u8g.setPrintPos(x + 28, 11);
    u8g.print("/");
    u8g.setPrintPos(x + 33, 11);
    u8g.print(now.month(), DEC);
    if (now.month() < 10)
      x -= 7;
    u8g.setPrintPos(x + 47, 11);
    u8g.print("/");
    u8g.setPrintPos(x + 53, 11);
    u8g.print(now.day(), DEC);
    u8g.setFont(u8g_font_8x13);
    int x = 35;
    u8g.setPrintPos(x, 33);
    u8g.print(now.hour(), DEC);
    if (now.hour() < 10)
      x -= 7;
    u8g.setPrintPos(x + 15, 33);
    u8g.print(":");
    u8g.setPrintPos(x + 21, 33);
    u8g.print(now.minute(), DEC);
    if (now.minute() < 10)
      x -= 7;
    u8g.setPrintPos(x + 36, 33);
    u8g.print(":");
    u8g.setPrintPos(x + 42, 33);
    u8g.print(now.second(), DEC);
  }
}

void drawLogo(uint8_t d)
{
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(8 + d, 30 + d, "E");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(30 + d, 30 + d, "l");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(40 + d, 30 + d, "e");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(55 + d, 30 + d, "c");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(70 + d, 30 + d, "r");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(85 + d, 30 + d, "o");
  u8g.setFont(u8g_font_gdr25r);
  u8g.drawStr(100 + d, 30 + d, "w");
}


//Style the flowers     bitmap_bad: bad flowers     bitmap_good:good  flowers
void drawflower(void)
{
  if (moisture1_value < 30)
  {
    u8g.drawXBMP(0, 0, 32, 30, bitmap_bad);
  }
  else
  {
    u8g.drawXBMP(0, 0, 32, 30, bitmap_good);
  }
  if (moisture2_value < 30)
  {
    u8g.drawXBMP(32, 0, 32, 30, bitmap_bad);
  }
  else
  {
    u8g.drawXBMP(32, 0, 32, 30, bitmap_good);
  }
  if (moisture3_value < 30)
  {
    u8g.drawXBMP(64, 0, 32, 30, bitmap_bad);
  }
  else
  {
    u8g.drawXBMP(64, 0, 32, 30, bitmap_good);
  }
  if (moisture4_value < 30)
  {
    u8g.drawXBMP(96, 0, 32, 30, bitmap_bad);
  }
  else
  {
    u8g.drawXBMP(96, 0, 32, 30, bitmap_good);
  }

}


void drawTH(void)
{
  int A = 0;
  int B = 0;
  int C = 64;
  int D = 96;
  char moisture1_value_temp[5] = {0};
  char moisture2_value_temp[5] = {0};
  char moisture3_value_temp[5] = {0};
  char moisture4_value_temp[5] = {0};
  read_value();
  itoa(moisture1_value, moisture1_value_temp, 10);
  itoa(moisture2_value, moisture2_value_temp, 10);
  itoa(moisture3_value, moisture3_value_temp, 10);
  itoa(moisture4_value, moisture4_value_temp, 10);
  u8g.setFont(u8g_font_7x14);
  u8g.setPrintPos(9, 60);
  u8g.print("A0");
  if (moisture1_value < 10)
  {
    //u8g.setPrintPos(A + 14, 45 );
    u8g.drawStr(A + 14, 45, moisture1_value_temp);
    delay(20);
    u8g.drawStr(A + 14, 45, moisture1_value_temp);
    
  }
  else if (moisture1_value < 100)
  {
    //u8g.setPrintPos(A + 7, 45);
    u8g.drawStr(A + 7, 45, moisture1_value_temp);
    delay(20);
    u8g.drawStr(A + 7, 45, moisture1_value_temp);
   
  }
  else
  {
    //u8g.setPrintPos(A + 2, 45 );
    moisture1_value = 100;
    itoa(moisture1_value, moisture1_value_temp, 10);
    u8g.drawStr(A + 2, 45, moisture1_value_temp);
  }
  //u8g.print(moisture1_value);
  u8g.setPrintPos(A + 23, 45 );
  u8g.print("%");
  u8g.setPrintPos(41, 60 );
  u8g.print("A1");
  if (moisture2_value < 10)
  {
    //u8g.setPrintPos(B + 46, 45 );
    u8g.drawStr(B + 46, 45, moisture2_value_temp); 
    delay(20);
    u8g.drawStr(B + 46, 45, moisture2_value_temp); 
  }
  else if (moisture2_value < 100)
  {
    //u8g.setPrintPos(B + 39, 45);
    u8g.drawStr(B + 39, 45, moisture2_value_temp);
    delay(20);
    u8g.drawStr(B + 39, 45, moisture2_value_temp);
  }
  else
  {
    //u8g.setPrintPos(B + 32, 45);
    moisture2_value = 100;
    itoa(moisture2_value, moisture2_value_temp, 10);
    u8g.drawStr(B + 32, 45, moisture2_value_temp);
  }
  // u8g.print(moisture2_value);
  u8g.setPrintPos(B + 54, 45);
  u8g.print("%");
  u8g.setPrintPos(73, 60);
  u8g.print("A2");
  if (moisture3_value < 10)
  {
    //u8g.setPrintPos(C + 14, 45 );
    u8g.drawStr(C + 14, 45, moisture3_value_temp);
    delay(20);
    u8g.drawStr(C + 14, 45, moisture3_value_temp);
    
  }
  else if (moisture3_value < 100)
  {
    // u8g.setPrintPos(C + 7, 45);
   u8g.drawStr(C + 7, 45, moisture3_value_temp);
    delay(20);
    u8g.drawStr(C + 7, 45, moisture3_value_temp);
    
  }
  else
  {
    // u8g.setPrintPos(C + 2, 45);
    moisture3_value = 100;
    itoa(moisture3_value, moisture3_value_temp, 10);
    u8g.drawStr(C + 2, 45, moisture3_value_temp);
  }
  //u8g.print(moisture3_value);
  u8g.setPrintPos(C + 23, 45);
  u8g.print("%");
  u8g.setPrintPos(105, 60);
  u8g.print("A3");
  if (moisture4_value < 10)
  {
    //u8g.setPrintPos(D + 14, 45 );
    u8g.drawStr(D + 14, 45, moisture4_value_temp);
    delay(20);
    u8g.drawStr(D + 14, 45, moisture4_value_temp);
   
  }
  else if (moisture4_value < 100)
  {
    // u8g.setPrintPos(D + 7, 45);
    u8g.drawStr(D + 7, 45, moisture4_value_temp);
    delay(20);
    u8g.drawStr(D + 7, 45, moisture4_value_temp);
  
  }
  else
  {
    //u8g.setPrintPos(D + 2, 45);
    moisture4_value = 100;
    itoa(moisture4_value, moisture4_value_temp, 10);
    u8g.drawStr(D + 2, 45, moisture4_value_temp);
  }
  //u8g.print(moisture4_value);
  u8g.setPrintPos(D + 23, 45);
  u8g.print("%");
}
