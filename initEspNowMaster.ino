//https://www.espressif.com/sites/default/files/documentation/2c-esp8266_non_os_sdk_api_reference_en.pdf

void initEspNowMaster(){
  if (esp_now_init() == 0){

    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {     //CALLBACK function called when SEND register activated
      if (sendStatus == 0) Serial.print("send_cb, send done, status = SUCCESS \n");
      else Serial.print("send_cb, send done, status = FAILED \n\n");
    });
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    
    esp_now_set_kok(key, 16);                     //set local key
    esp_now_add_peer(hubMacAdd, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, key, 16);
  }
  else{
    Serial.println("`````ESP_Now init failed`````");
    ESP.restart();
  }
}
