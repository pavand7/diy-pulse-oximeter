/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 7th Aug 2021 with the current ESP8266 Arduino core release 3.0.2
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define REPORTING_PERIOD_MS 1000
const char* ssid = "ACTFIBERNET";
const char* password = "act12345";


#include <OakOLED.h>

#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <CircularBuffer.h>

//Pulse Heart Rate BPM and Oxygen SpO2 Monitor

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include <ESP8266WiFi.h>

 
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "OakOLED.h"

#define REPORTING_PERIOD_MS 1000

OakOLED oled;
 
 
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;
 
float BPM, SpO2;

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
 
void onBeatDetected()
{
    Serial.println("Beat Detected!");
    oled.drawBitmap( 60, 20, bitmap, 28, 28, 1);
    oled.display();
}

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "a2h96oo4v0g553-ats.iot.us-east-2.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key:

// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = 
//-----BEGIN CERTIFICATE-----
R"EOF(MIIDWTCCAkGgAwIBAgIUBh7IBQk0jFyGelVXgaXZsJMgmJswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMTEyMjIwMjgw
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMzVrfI5jO8yPtfCuqtW
w4GpZwvXNLSmoU0SgdvOSdD8i4xQUDoTlxu5Bq+IrpftLzLEmfVoVfMf27EbSBV9
fulVKOzqMgv296+b4d3LIH1yYYPrAGl7TpbCDkVlnQ/SnUvpXzum27hGo69nayLb
dnpG3ikaBzMCc+A0MqS9nFbmPitJvVDp8VRK9W6Pn0zDAsdpmeSfIoPXiL0C9mV1
yl1funNMS7YGV59dB2Ac/Z+L8E+nwnpKZKKaCEN9kkvdUs+9OZuUuXIkDzK9ftSE
y1QX84d/rYSLJliMwS4Q95jlNNdzNMsO8Ji044iTfgp+JaHcO0Jxrb86SzaBP7I+
QN8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUCaInwyhUPRy/NgxId/6zQ/u9vukwHQYD
VR0OBBYEFAGp1SxzmmEBP06heJLEjDxrzThEMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCrKqkjJZF3WBYtlYkErchDepJX
r6uCrqGnfgfivUcrZ0jKmeOVBw2xoPzH6Hx7DyMdOywQO/s71yIJYXNJ/7A1msmr
DqU3cVGLbNn0CSxpeP++okxJzgAVPIx6N9iR05HSNyFFwJ9cJCR19D4XYv8htteG
D9KrsmImc84+ft9jcgUSY6xaXcXOSUcNB5UTfyM8SNG7r+omdPw/zjc1Sq3ArXbQ
rnYgIf6IVMHYm2wa+a4ekK44QJleeqsz2KWeEla+YPepp2Hj1VPLet6fAGac13Op
z1I1XsAxPA7RyTxMjA6PSytUGOM1uHPazYTb00/tH+LN9Ufz1of8B/LMaSnB)EOF";
//-----END CERTIFICATE-----

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = 
// -----BEGIN RSA PRIVATE KEY-----
R"EOF(MIIEpQIBAAKCAQEAzNWt8jmM7zI+18K6q1bDgalnC9c0tKahTRKB285J0PyLjFBQ
OhOXG7kGr4iul+0vMsSZ9WhV8x/bsRtIFX1+6VUo7OoyC/b3r5vh3csgfXJhg+sA
aXtOlsIORWWdD9KdS+lfO6bbuEajr2drItt2ekbeKRoHMwJz4DQypL2cVuY+K0m9
UOnxVEr1bo+fTMMCx2mZ5J8ig9eIvQL2ZXXKXV+6c0xLtgZXn10HYBz9n4vwT6fC
ekpkopoIQ32SS91Sz705m5S5ciQPMr1+1ITLVBfzh3+thIsmWIzBLhD3mOU013M0
yw7wmLTjiJN+Cn4lodw7QnGtvzpLNoE/sj5A3wIDAQABAoIBAQC3oAr67aqxN+rs
Q05SkUpcr3+INtmx5PsO+7tcUDcOWYfENq9lEPDS0nDy5WYXHnLEYUjfnjQG6xVo
VNuvIDyockZo9J3+6dwrGmRGD3XF38cZmo9ZAEh4j+h5P9moOJf4WHKeKDwqm3Db
qqt5qSIyBdvdHs87ZER6xnBsDOrdXpeqfC+ZamkMpqDORtf4Qzmo3CK2QU5s0hnS
q5zLUMPZjUIbTfGmeORVpH5pZsyX5xcHWvKQRB++TgZZYluhKcmpZQeIoviun6ng
vy5FLjhbo2BcblFnbhmLowahpYrAUdIYLFe76dt21BBZiVDaeX+ma4ex+Q6w3H0f
5G8cAMMxAoGBAP4hHtYQap5wgS7bOKE135dOZmX5XEFa4Gd8ns1QG7b5hL2Jwylu
GHk2Kc+kK8x5k0K5t32ysQJj2veAnsf7WhKrRR+p6WDRrB480FTGgEggQ+3SWr5V
S5CJGCB5lMKrejo5PBYH1euHNsGRsBt7MyWOS8MqiWJP740ozpQgbkrXAoGBAM5X
qyCa/KPJ296vcAvd8OXDEkFKQZmQQA5yM+08NnvnlSq5JHl1BXFpzVixmwVGdhr3
haKppvzJQm/6C2se/mLlZKZ8+Ooti7Dnzc3feo9i45UdKjUbwXK1YgeBbdjz3UMo
2zXj8auoCr+o0q+TxDaQ0y8xPgOtiUeLXLQd6UE5AoGBAMwXm2U9r/Lo1cbkxnkq
BBTHIRO6jS3rpxdVjU/Ei02K3OE/t14ZjkwihLe+2HISHYNUFZo8nU4XFe75/AsN
wUwHAq+oTMa1/W4XVxUOik3io8tmyfvcEqDUiWzjV+ZzwujpuGjVCCds4zvYHkAj
1HLP8eaezdEH85JBeI6b8RfnAoGBAKKfX4SUAdcIaVvpArKnmYtLoaCJaIghswpv
oVMwqHQgwWRphEChJMgMrcVgPJ/8t2preAwKD0ydRdpAxC0JwU2qIxg/2GGfwb+M
AZxmrA5Dzr+fu7sdg+nm4DqwHCXnlJOwqIuheb9i8g25fUrZd8PVgktoUMrPQyl6
wXZHOKWpAoGAQ3nF7HY7yF5dE3GdAKhIt+KAe8ui0Ocs5EvAQNfcqfVZ4H7pXKGo
KhDCPUPHqVLZK/HC3d08+byER/sTrguyHys1ZIgc46e1i4LewSMXTgyAuoGvOGO9
BnBBx5OsO4+A56w444wc3pTCH2h2q2efkJU3XWpz7wgQ3JkZE6LJxok=)EOF";
// -----END RSA PRIVATE KEY-----

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200);
  oled.begin();
  WiFi.begin(ssid, password);                                  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());                               //prints local IP address
  
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

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish = 0;
int msgCount;

void loop() {
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  

  pubSubCheckConnect();

  if (millis() - lastPublish > REPORTING_PERIOD_MS) {
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
    String msg = String(BPM);
    pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.println(pubSubClient.state());
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
