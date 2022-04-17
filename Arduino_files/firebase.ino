
#include <OakOLED.h>
//#include <SoftwareSerial.h>

#include<FirebaseArduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <CircularBuffer.h>
#include <ESP8266HTTPClient.h>
#include "ESP8266WebServer.h"

//Pulse Heart Rate BPM and Oxygen SpO2 Monitor

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
//#define BLYNK_PRINT Serial
//#include <Blynk.h>
#include <ESP8266WiFi.h>
//#include<DNSServer.h>
//#include<ESP8266WebServer.h>
//#include<WiFiManager.h>

//#include <BlynkSimpleEsp8266.h>
 
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "OakOLED.h"

#define REPORTING_PERIOD_MS 1000
//#define D0(16)
OakOLED oled;
 
#define FIREBASE_HOST "iot1-424e6-default-rtdb.firebaseio.com"      
#define FIREBASE_AUTH "Ht9ZehcVxSRH06a8nib7Uf8VW6RN9Rcuzcl4PR9x"            
#define WIFI_SSID "ACTFIBERNET"                                  
#define WIFI_PASSWORD "act12345"
 
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox; 
float BPM, SpO2;
uint32_t tsLastReport = 0;
//SoftwareSerial BTserial(3,1); 
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
volatile boolean heartBeatDetected = false;
void onBeatDetected()
{
    heartBeatDetected = true;
    Serial.println("Beat Detected!");
    oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
    oled.display();
}
 
void setup()
{
    Serial.begin(115200);
//    WiFiManager m;
    oled.begin();
//    WiFi.mode(WIFI_STA);
//    delay(100);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
    Serial.print("Connecting to ");
//    m.autoConnect("xyz","123");
//    Serial.println("connected");
    Serial.print(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
   
    Serial.println();
    Serial.print("Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());                               //prints local IP address
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    
//    BTserial.begin(9600); 
//    Serial.println("BTserial started at 38400");
//    Serial.println(" ");
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.setCursor(0, 0);
    
    oled.println("Initializing pulse oximeter..");
    oled.display();
    
    pinMode(16, OUTPUT);
//    Blynk.begin(auth, ssid, pass);
 
    Serial.print("Initializing Pulse Oximeter..");
 
    if (!pox.begin())
    {
         Serial.println("FAILED");
         oled.clearDisplay();
         oled.setTextSize(1);
         oled.setTextColor(1);
         oled.setCursor(0, 0);
         oled.println("FAILED");
         oled.display();
//         for(;;);
         Serial.println("hi");
    }
    
    else
    {
         oled.clearDisplay();
         oled.setTextSize(1);
         oled.setTextColor(1);
         oled.setCursor(0, 0);
         oled.println("SUCCESS");
         oled.display();
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
 
    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
}
void leerDatos(){
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  Serial.print("Heart rate:");
  Serial.print(BPM);
  Serial.print(" SpO2:");
  Serial.print(SpO2);
  Serial.println(" %");
  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0,16);
  oled.println(BPM);
  
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0, 0);
  oled.println("Heart BPM");

  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0, 30);
  oled.println("Spo2");

  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0,45);
  oled.println(SpO2);
  oled.display();
  
  if( heartBeatDetected && BPM !=0) {
  
    if(SpO2>0){
    
      // here you put the variables what you have in ur db
      
      Firebase.setFloat("trial", 72.0);
      Firebase.setFloat("trial", 98.0);
//      if(Firebase.failed()){
//        Serial.print("absent");
//      }
      
      // you can delete this line below, it’s just to know it send the datas in the serial monitor
      Serial.print("hi");

    }

  }

}
 
void loop()
{
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    { 
        pox.shutdown();
        leerDatos();
        pox.resume();

        tsLastReport = millis();
    }
}
