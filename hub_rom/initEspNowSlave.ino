void initEspNowSlave() {
  if (esp_now_init() == 0){
    
    esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {     //CALLBACK function called when RECV register activated     
      String macComparer1 = "", macComparer2 = "";

      //create string that signals which mac addresss sent the message
      for (int i = 0; i < 6; i++) {
        if (mac[i] == s1MacAdd[i]) {
          macComparer1 += "1";
        }
        if (mac[i] == s2MacAdd[i]) {
          macComparer2 += "1";
        }
      }
  
//      Serial.println("macComparer1: " + macComparer1);
//      Serial.println("macComparer2: " + macComparer1);
  
    //-------if MSG FROM S,----------
      if (macComparer1 == "111111") {
        //record incoming data in sensor1.msgBuffer
        for (int i = 0; i < len; i++)
          sensor1.msgBuffer[i] = data[i];
          
        s1QuatReady = true;
      }
      
    //-------if MSG FROM S2,----------
      if (macComparer2 == "111111") {
        //record incoming data in sensor2.msgBuffer
        for (int i = 0; i < len; i++)
          sensor2.msgBuffer[i] = data[i];
          
        s2QuatReady = true;
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

    //set local key
    esp_now_set_kok(key, 16);       
    //add s1 and s2              
    esp_now_add_peer(s1MacAdd, ESP_NOW_ROLE_CONTROLLER, WIFI_CHANNEL, key, 16);
    esp_now_add_peer(s2MacAdd, ESP_NOW_ROLE_CONTROLLER, WIFI_CHANNEL, key, 16);
  }
  else{
    Serial.println("`````ESP_Now init failed`````");
    ESP.restart();
  }
}
