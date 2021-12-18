#include <ESP8266WiFi.h>
#include <EEPROM.h>
extern "C" {              //see https://embeddedartistry.com/blog/2017/4/4/extern-c
  #include <espnow.h>
}

#define WIFI_CHANNEL 1

#define eepromKeyAddress 0
#define eepromSensor1MacAddress 16
#define eepromSensor2MacAddress 22

//NOTE: Wifi.macAddress() and Wifi.softAPmacAddress are different.
uint8_t s1MacAdd[6];
uint8_t s2MacAdd[6];
uint8_t key[16];

const char *ssid = "ROM Device | Orange Spruce";     //The name of the Wi-Fi network that will be created
const char *password = "Bhad Bhabie";   // The password required to connect to it, leave blank for an open network

volatile int quatSenderEnabled = 0;
volatile bool s1QuatReady = false;
volatile bool s2QuatReady = false;

#define w 0
#define x 1
#define y 2
#define z 3
union SENSOR_DATA{
  uint8_t msgBuffer[16];
  float q[4];           //quat[w] = quat[0] bc of define above
};

SENSOR_DATA sensor1;
SENSOR_DATA sensor2;

void setup() {
  Serial.begin(115200); 
  EEPROM.begin(512);      //Allocate 512 bytes of flash for eeprom mock.

  WiFi.softAP(ssid, password, 1, 0);    //(const char* ssid, const char* passphrase, int channel, int ssid_hidden, int max_connection) https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiAP.cpp
  initEspNowSlave(); 
}


void loop() {
  checkUI();

  if(quatSenderEnabled == 1){
    if(s1QuatReady){
    Serial.print("S1 " + (String) sensor1.q[w] + ":" + (String) sensor1.q[x] + 
            ":" + (String) sensor1.q[y] + ":" + (String) sensor1.q[z] + "\n");
    
    for (int i = 0; i<=4; i++)
      sensor1.q[i] = NULL;

    s1QuatReady = false;
    }
    if(s2QuatReady){
      Serial.print("S2 " + (String) sensor2.q[w] + ":" + (String) sensor2.q[x] + 
              ":" + (String) sensor2.q[y] + ":" + (String) sensor2.q[z] + "\n");
      
      for (int i = 0; i<=4; i++)
        sensor2.q[i] = NULL;
  
      s2QuatReady = false;
    }
  }
}
