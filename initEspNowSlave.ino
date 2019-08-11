void initEspNowSlave() {
  if (esp_now_init() == 0){
    
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {     //CALLBACK function called when RECV register activated
      String macComparer1 = "";
      String macComparer2 = "";
      
      for (int i = 0; i < 6; i++) {
//        Serial.printf("mac[%i] = ", i);
//        Serial.println(mac[i], HEX);
        if (mac[i] == s1MacAdd[i]) {
          macComparer1 += "1";
        }
        if (mac[i] == s2MacAdd[i]) {
          macComparer2 += "1";
        }
      }
  
//      Serial.println("macComparer1: " + macComparer1);
//      Serial.println("macComparer2: " + macComparer1);
  
      //if true, msg is from sensor1
      if (macComparer1 == "111111") {
        for (int i = 0; i < len; i++) {
          sensor1.msgBuffer[i] = data[i];
        }
        msgReadySensor1 = true;    //signal msg ready to report
      }
      
      //if true, msg is from sensor2
      if (macComparer2 == "111111") {
        for (int i = 0; i < len; i++) {
          sensor2.msgBuffer[i] = data[i];
        }
        msgReadySensor2 = true;    //signal msg ready to report
      }
    });
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

    //read the eeprom for key and Hub's macAdd.
    readEEPROM(key, 16, eepromKeyAddress);
    readEEPROM(s1MacAdd, 6, eepromSensor1MacAddress);
    readEEPROM(s2MacAdd, 6, eepromSensor2MacAddress);

//    Serial.println(s1MacAdd[1]);
//    Serial.println(s2MacAdd[1]);
//    Serial.println(key[1]);
    
    esp_now_set_kok(key, 16);                     //set local key
    esp_now_add_peer(s1MacAdd, ESP_NOW_ROLE_CONTROLLER, WIFI_CHANNEL, key, 16);
    esp_now_add_peer(s2MacAdd, ESP_NOW_ROLE_CONTROLLER, WIFI_CHANNEL, key, 16);
  }
  else{
    Serial.println("`````ESP_Now init failed`````");
    ESP.restart();
  }
}
