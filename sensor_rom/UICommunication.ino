void checkUI(){
  String message;  
  if (Serial.available())
    message = UIRead();
    
  if (message== "~ISC~"){
    UI_ISC();
    return;
  }
    
  if (message == "~R2Rkey~"){
    UI_Key();
    return;
  }
  
  if (message == "~R2RMac~"){
    UI_MacAdd();
    return;
  }

  if (message == "~restart~"){
    Serial.print("~restarting~\n");
    ESP.restart();
  }
  
}

void UI_ISC(){
  Serial.printf("~SENSOR;MacADD: %s~\n", WiFi.macAddress().c_str());
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
  
  if (message == "~hub~"){
    Serial.print("~hub~\n");
    UIStoreIncomingBytes(hubMacAdd, 6);
    writeEEPROM(hubMacAdd, 6, eepromHubMacAddress);
    EEPROM.commit();
  }
}

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

Init. Serial Com. [DOES NOT USE: UIStoreBytes()]
  COMMAND:    "~ISC~"
  
ONLY ACSESSABLE VIA PAIR COMMAND
  Ready to Recieve aes128 key
    COMMAND:    "~R2Rkey~"
    RESPONSE:   "~R2Rkey~\n"
    UIStoreBytes(key, 16);
  
  Ready to Recieve Hub MacAdd
    COMMAND:    "~R2RMac~"
    RESPONSE:   "~R2RMac~\n"  
    COMMAND:    "~hub~"
    RESPONSE:   "~hub~\n"
    UIStoreBytes(hubMacAdd, 6);

*/
