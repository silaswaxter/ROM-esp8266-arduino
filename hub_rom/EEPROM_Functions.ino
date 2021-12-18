//writes data from dataToWrite[].  dataToWriteCount is number of ints/bytes.  address is the address where the read starts.
//use EEPROM.commit() to save data.  # of commits should be minimized to prolong life of flash.
void writeEEPROM(uint8_t dataToWrite[], int dataToWriteCount, int address){
  for(int i = 0; i < dataToWriteCount; i++){
    EEPROM.write(address, dataToWrite[i]);
    address++;
  }
}

//reads data into dataRead[].  dataReadCount determines the number of bytes read.  address is the address where the read starts.
void readEEPROM(uint8_t dataRead[], int dataReadCount, int address){
  for(int i = 0; i < dataReadCount; i++){
    dataRead[i] = EEPROM.read(address);
    address++;
  }
}

/*.........................EEPROM Map...........................
#_OF_BYTES      OBJECT_STORED

16              Aes128Key
6               Sensor 1 MacAdd
6               Sensor 2 MacAdd

*/
