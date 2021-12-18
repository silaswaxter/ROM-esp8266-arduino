#include <ESP8266WiFi.h>
#include <EEPROM.h>
extern "C" {              //see https://embeddedartistry.com/blog/2017/4/4/extern-c
  #include <espnow.h>
}

#define WIFI_CHANNEL 1

#define eepromKeyAddress 0
#define eepromHubMacAddress 16

//NOTE: Wifi.macAddress() and Wifi.softAPmacAddress are different.
uint8_t hubMacAdd[6];
uint8_t key[16];

#define w 0
#define x 1
#define y 2
#define z 3
union SENSOR_DATA{
  uint8_t msgBuffer[16];
  float q[4];               //quat[w] = quat[0] bc of define above
} sens;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);      //Allocate 512 bytes of flash for eeprom mock.

  WiFi.mode(WIFI_STA);    //Station mode for esp-now sensor node
  WiFi.disconnect();
  initEspNowMaster();
  
  sens.q[w] = 9.876; sens.q[x] = 5.432; sens.q[y] = 9.012; sens.q[z] = 3.456;
}

void loop() {
  checkUI();
  
  esp_now_send(hubMacAdd, sens.msgBuffer, sizeof(sens.msgBuffer));
  delay(100);
}
