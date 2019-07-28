void initEspNowSlave() {
  if (esp_now_init()!= 0) {
    Serial.println("`````ESP_Now init failed`````");
    ESP.restart();
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_set_kok(key, 16);
  
  esp_now_add_peer(masterMac, ESP_NOW_ROLE_CONTROLLER, WIFI_CHANNEL, key, 16);
  
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {     //CALLBACK function called when RECV register activated

    /*
    sender1Mac = "";
    sender1Mac += String(mac[0], HEX);
    sender1Mac += String(mac[1], HEX);
    sender1Mac += String(mac[2], HEX);
    sender1Mac += String(mac[3], HEX);
    sender1Mac += String(mac[4], HEX);
    sender1Mac += String(mac[5], HEX);
    Serial.println("Message received from device: "); Serial.printf("%s\n", sender1Mac.c_str());
    */
    
    for (int i = 0; i<len; i++){
      sens.msgBuffer[i] = data[i];
    }
    msgReady = true;    //signal msg ready to report
  });
}
