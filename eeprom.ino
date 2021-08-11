/*
 * Idea: Could store the length as the first int
 */

#define EEPROM_END_BYTE 0x0A // Line feed character to mark end of string
#define EEPROM_MAX_ITEM_LENGTH 49

void write_string_to_eeprom(String string_to_write, int start_position){
  
  for(int byte_index=0; byte_index<string_to_write.length(); byte_index++){
    
    int position_to_write = start_position + byte_index;
    byte value_to_write = string_to_write[byte_index];
    
    EEPROM.write(position_to_write, value_to_write);
  }

  // Add a line feed character to mark the end of the string
  int position_to_write = string_to_write.length() + start_position;
  EEPROM.write(position_to_write, EEPROM_END_BYTE);

  // Save what's been written
  if (EEPROM.commit()) Serial.println("[EEPROM] commit successful");
  else Serial.println("[EEPROM] commit FAILED");
}

String read_string_from_eeprom(int start_position){
  
  String output = "";
  
  for(int byte_index=0; byte_index<EEPROM_MAX_ITEM_LENGTH; byte_index++){
    
    int position_to_read = start_position + byte_index;
    
    byte value = EEPROM.read(position_to_read);
    
    if(value == EEPROM_END_BYTE) break;

    char character = (char) value;

    if(isAlphaNumeric(character) 
      || character == '-' 
      || character == '_'
      || character == ' '
    ){
      output.concat(character);
    }
    
  }

  // dodgy
  if(output.length() == EEPROM_MAX_ITEM_LENGTH) return "invalid";

  return output;
  
}

void save_wifi_ssid_in_eeprom(String wifi_ssid) {
  write_string_to_eeprom(wifi_ssid, EEPROM_WIFI_SSID_ADDRESS);
}

void save_wifi_password_in_eeprom(String wifi_password) {
  write_string_to_eeprom(wifi_password, EEPROM_WIFI_PASSWORD_ADDRESS);
}

String get_wifi_ssid_from_eeprom() {
  return read_string_from_eeprom(EEPROM_WIFI_SSID_ADDRESS);
}

String get_wifi_password_from_eeprom() {
  return read_string_from_eeprom(EEPROM_WIFI_PASSWORD_ADDRESS);
}


void write_int_to_eeprom(int address, int input){

  // stored little endian (I think)
  // Warning: This does not commit!  
  
  byte byte_high = input >> 8;
  byte byte_low = input & 0xFF;
  
  EEPROM.write(address, byte_high);
  EEPROM.write(address + 1, byte_low);
}

int read_int_from_eeprom(int address){

  // stored little endian (I think)

  byte byte_high = EEPROM.read(address);
  byte byte_low = EEPROM.read(address + 1);

  return (byte_high << 8) + byte_low;
}



void read_code_from_eeprom(int address){
  // An int is 2 bytes
  // code_length is the number of ints in the code
  
  unsigned int code_length = read_int_from_eeprom(address);

  unsigned int code[code_length];

  for(int i = 0; i < code_length; i++){
    code[i] = read_int_from_eeprom(address + 2*i + 2);
  }
  
  Serial.print("Code from EEPROM: (");
  Serial.print(code_length);
  Serial.print("),");

  for(int i = 0; i < code_length; i++){
    Serial.print(code[i]);
    Serial.print(",");
  }

  Serial.println();

}
