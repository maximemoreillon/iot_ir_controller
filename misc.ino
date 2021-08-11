String get_device_name(){
  //return String(DEVICE_TYPE) + "-" + String(DEVICE_ID);
  //String mac_address_fromatted = WiFi.macAddress();
  String chip_id = String(ESP.getChipId(), HEX);
  //mac_address_fromatted.replace(":","");
  //mac_address_fromatted.toLowerCase();
  return String(DEVICE_TYPE) + "-" + chip_id;
}

String get_device_nickname(){
  return read_string_from_eeprom(EEPROM_DEVICE_NICKNAME_ADDRESS);
}
