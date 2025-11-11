/*
* Name: ovningsprojekt
* Author: Edvin Andersson
* Date: 2025-10-14
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display, 
* Further, it measures temprature with ds3231 and displays a mapped value to a 9g-servo-motor,
* It also takes the time and maps it to a 24 pixel neopixel to simulate a analog clock
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include <Servo.h>
#include <U8glib.h>
#include <Adafruit_NeoPixel.h>

int ThermistorPin = 2;

// Init constants

// Init global variables

// Construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;

#define PIN 6
#define NUMPIXELS 24 

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



void setup() {

  myservo.attach(5);
  u8g.setFont(u8g_font_helvB08);

  Serial.begin(9600);
  Wire.begin();

  rtc.begin();

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  strip.begin();
}

void loop() {
  DateTime now = rtc.now();
  float temp = rtc.getTemperature();

  oledWrite(getTime(now), getDate(now));
  servoWrite(getTemp());
  neopixel_clock(now);

  Serial.print(getTime(now));
  Serial.print("      ");
  Serial.println(getDate(now));
  Serial.println(temp);

  delay(200);
}



//This function reads time from an ds3231 module and package the time as a String
//Parameters: Void
//Returns: time in hh:mm:ss as String
String getTime(DateTime n) {
  String timeStr = "";

  if (n.hour() < 10) timeStr += "0";
  timeStr += String(n.hour());
  timeStr += ":";

  if (n.minute() < 10) timeStr += "0";
  timeStr += String(n.minute());
  timeStr += ":";

  if (n.second() < 10) timeStr += "0";
  timeStr += String(n.second());


  return timeStr;
}

String getDate(DateTime n) {
  return String(n.year()) + ":" + String(n.month()) + ":" + String(n.day());
}


/*
* This function takes temprature from ds3231 and returns as a float
*Parameters: Void
*Returns: temperature mapped for the servo 
*/
float getTemp() {
  return map(rtc.getTemperature(), 15, 30, 0, 180);
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String getTime, String getDate) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 20, ("Time: " + (getTime)).c_str());
    u8g.drawStr(0, 40, ("Date: " + (getDate)).c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: mapped temperature value
*Returns: void
*/
void servoWrite(float value) {
  myservo.write(value);
}

/*
* takes the time in seconds, minute and hour and maps it to the correct pixel
*Parameters: time
*Returns: void
*/
void neopixel_clock(DateTime n) {
  float pix_s = n.second() * 0.4;
  int pix_m = n.minute() * 0.4;
  int pix_h = n.hour();
  if (pix_h > 12) {
    pix_h -= 12;
  }
  pix_h *= 2;

  strip.clear();
  strip.setPixelColor(pix_s, strip.Color(255, 0, 0));
  strip.setPixelColor(pix_m, strip.Color(0, 255, 0));
  strip.setPixelColor(pix_h, strip.Color(0, 0, 255));
  strip.show();
}