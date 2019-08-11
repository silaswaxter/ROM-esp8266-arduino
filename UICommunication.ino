String checkUI(){
  String message;  
  if (Serial.available())
    message = UIRead();
    
  if (message== "~ISC~"){
    UI_ISC();
    return "ISC";
  }

  if (message == "~getS1Quat~"){
    UI_GetQuat(1);
    return "";
  }

  if (message == "~getS2Quat~"){
    UI_GetQuat(2);
    return "";
  }
  
  if (message == "~R2Rkey~"){
    UI_Key();
    return "pairing";
  }
  
  if (message == "~R2RMac~"){
    UI_MacAdd();
    return "pairing";
  }

  if (message == "~restart~"){
    Serial.print("~restarting~\n");
    ESP.restart();
  }
  
  return "";
}

void UI_ISC(){
  //NOTE: Wifi.macAddress() and Wifi.softAPmacAddress are different.
  Serial.printf("~HUB;MacADD: %s~\n", WiFi.softAPmacAddress().c_str());
}

bool UI_GetQuat(int sensorSpecifier){
  if (sensorSpecifier == 1){
    if(msgReadySensor1){
      String s1Quat = ("~S1 " + (String) sensor1.q[w] + ":" + (String) sensor1.q[x] + 
                  ":" + (String) sensor1.q[y] + ":" + (String) sensor1.q[z] + "~\n");

      for (int i = 0; i<=4; i++){
        sensor1.q[i] = NULL;
      }
      Serial.print(s1Quat);
  
      //if(UIRead() == sensData){
      //    successStatus = true;
      //}
      return true;
    }
  }

  if (sensorSpecifier == 2){
    if(msgReadySensor2){
      String s2Quat = ("~S2 " + (String) sensor2.q[w] + ":" + (String) sensor2.q[x] + 
                        ":" + (String) sensor2.q[y] + ":" + (String) sensor2.q[z] + "~\n");
      
      for (int i = 0; i<=4; i++){
        sensor2.q[i] = NULL;
      }
      Serial.print(s2Quat);
  
      //if(UIRead() == sensData){
      //    successStatus = true;
      //}
      return true;
    }
  }
  return false;
}

void UI_Key(){
  Serial.print("~R2Rkey~\n");       //confirm ready 2 recieve key
  UIStoreIncomingBytes(key, 16);
  writeEEPROM( key, 16, eepromKeyAddress);
  EEPROM.commit();
}

void UI_MacAdd(){
  Serial.print("~R2RMac~\n");                     //Tell UI R2RMac

  //Wait for UI message about MacAdd's device
  String message = UIRead();
    
  if (message == "~s1~"){
    Serial.print("~s1~\n");
    UIStoreIncomingBytes(s1MacAdd, 6);
    writeEEPROM(s1MacAdd, 6, eepromSensor1MacAddress);
    EEPROM.commit();
    return;
  }
      
  if (message == "~s2~"){
    Serial.print("~s2~\n");
    UIStoreIncomingBytes(s2MacAdd, 6);
    writeEEPROM(s2MacAdd, 6, eepromSensor2MacAddress);
    EEPROM.commit();
    return;
  }
}


//...................UI Data Manipulations and Receivers......................

//Stores incoming bytes from UI in storeLocation
void UIStoreIncomingBytes(uint8_t storeLocation[], int lengthOfStoreLocation){
  //for pointer explaination see http://www.cplusplus.com/doc/tutorial/pointers/
  uint8_t * ptr = storeLocation;
  String strByte;
  for(int i = 0; i < lengthOfStoreLocation; i++){
    strByte = UIRead();

    //if the UI returns incorrect byte message, go back to the last byte and retry.
    if(strByte == "~IncByteRetry~"){  
      i = i - 1;                      //go back to last byte
      strByte = UIRead();                //read new byte
    }
    
    //while message has a format issue, try to read again.
    while(strByte == "ERROR:\nBefore~" || strByte == "ERROR:ByteBefore~"){
      strByte = UIRead();
    }
      
    strByte = RemoveUIMessageFormat(strByte);     //removes '~' from front and back of message
    *(ptr + i) = (uint8_t) strByte.toInt();       //dereference (&ptr + index of lengthStoreLocation) = cast(uint8_t) strByte.toInt();

    //RESPOND: send byte back to ui.  Insures byte is correct.
    Serial.print("~");
    Serial.print(*(ptr + i));
    Serial.print("~\n");
  }
}

//Removes first and last character.  '~' if used with UIRead().
String RemoveUIMessageFormat(String UIMessage){
  UIMessage.remove(0, 1);   //remove first character.  remove(index, number of characters)
  UIMessage.remove(UIMessage.length() - 1, 1);    //remove last character.
  return UIMessage;
}

//Waits for Serial.available().  Checks if message is in UIcommunication format and returns message or error.
String UIRead(){
  String message;
  char readByte;
  
  while(!Serial.available())yield();           //wait for Serial.Available()
  readByte = Serial.read();                    //read first byte of message

  //if '~' is NOT first character, return ""
  if(readByte != '~'){
    //Serial.print("UIRead: '~' was not first byte");
    return "ERROR:ByteBefore~";
  }

  message = String(readByte);           //add '~' to message

  while(readByte != '\n'){              //while '\n' not read,
    while(!Serial.available())yield();  //wait for Serial.Available()
    readByte = Serial.read();           //store the read byte
    message += String(readByte);        //add byte to message
  
    if (readByte == '~'){               //if '~' is read, return message.
      //Serial.println("received: " + message);
      return message;
    }
  }
  return "ERROR:\nBefore~";                            //if '\n' is read before '~', return "".
}

/*.........................UI_COMMANDS & RESPONSES...........................
NOTE: All communication sent between the UI and devices are formated ("~message~").

UIStoreBytes(UI Array Transfers):
  -Transfers during out of the box pair, which "creates" the esp_now network use 
   UIStoreBytes(uint8_t storeLocation[], int lengthOfStoreLocation), which recieves
   a byte ("~byte~"), stores the Byte, and replies with the byte from the stored location.
    -Contains error system that allows UI to say the byte was incorrectly transfered.
     The function goes to the index (previous index) and recieves a new message of what
     the byte should be.  Error system also looks for '~' on start and end of message.
     If the format is incorrect, the byte replied will be wrong forcing the UI to resend
     the byte.
     
Init. Serial Com. [DOES NOT USE: UIStoreBytes()]
  COMMAND:    "~ISC~"
  RESPONSE:   "~HUB;MacADD: %s~\n", WiFi.macAddress().c_str()
    
Ready to Recieve aes128 key
  COMMAND:    "~R2Rkey~"
  RESPONSE:   "~R2Rkey~\n"
  UIStoreBytes(key, 16);

Ready to Recieve Sensor1 MacAdd
  COMMAND:    "~R2Rs1Mac~"
  RESPONSE:   "~R2Rs1Mac~\n"
  COMMAND:    "~s1~"
  RESPONSE:   "~s1~\n"
  UIStoreBytes(s1MacAdd, 6);
  
Ready to Recieve Sensor2 MacAdd
  COMMAND:    "~R2Rs2Mac~"
  RESPONSE:   "~R2Rs2Mac~\n"
  COMMAND:    "~s2~"
  RESPONSE:   "~s2~\n"
  UIStoreBytes(s2MacAdd, 6);
*/
