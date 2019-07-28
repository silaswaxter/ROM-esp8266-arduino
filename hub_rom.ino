#include <ESP8266WiFi.h>
extern "C" {              //see https://embeddedartistry.com/blog/2017/4/4/extern-c
  #include <espnow.h>
}
#define WIFI_CHANNEL 1

uint8_t masterMac[] = {0x84, 0x0D, 0x8E, 0x8A, 0x92, 0xCF};
uint8_t slaveMac[] = {0x86, 0x0D, 0x8E, 0x8F, 0x46, 0x76};
uint8_t key[16] = {0x77, 0x39, 0x7A, 0x24, 0x43, 0x26, 0x46, 0x29, 0x4A, 0x40, 0x4D, 0x63, 0x51, 0x66, 0x54, 0x6A};

const char *ssid = "ROM Device | Orange Spruce";     //The name of the Wi-Fi network that will be created
const char *password = "Bhad Bhabie";   // The password required to connect to it, leave blank for an open network

volatile bool msgReady = false;

#define w 0
#define x 1
#define y 2
#define z 3
union SENSOR_DATA{
  uint8_t msgBuffer[16];
  float q[4];           //quat[w] = quat[0] bc of define above
} sens;

void setup() {
  Serial.begin(115200); 
  Serial.println("\n");
  Serial.println("ESP_Now Slave\n");

  //Advertises network for 5 seconds then hides network.  Allows masters to connect for 5 seconds then diables pairing with more devices.
  WiFi.softAP(ssid, password, 1, 0);    //(const char* ssid, const char* passphrase, int channel, int ssid_hidden, int max_connection) https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiAP.cpp
  initEspNowSlave(); 
  //delay(5000);
  //WiFi.softAP(ssid, password, 1, 1);
  
  Serial.printf("This mac: %s \n", WiFi.softAPmacAddress().c_str()); 
  Serial.println("Setup done \n\n");
}


void loop() {
  if (msgReady){
    Serial.printf("qw: %f qx: %f qy: %f qz: %f\n", sens.q[w], sens.q[x], sens.q[y], sens.q[z]);
    for (int i = 0; i<=4; i++){
      sens.q[i] = NULL;
    }
    msgReady = false;
  }
}
