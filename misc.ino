String get_device_name(){
  String chip_id = String(ESP.getChipId(), HEX);
  return String(DEVICE_TYPE) + "-" + chip_id;
}

String get_device_nickname(){
  return read_string_from_eeprom(EEPROM_DEVICE_NICKNAME_ADDRESS);
}

void delayed_reboot(){
  reboot = true;
}

void handle_reboot(){
  static boolean reboot_request_acknowledged = false;
  static long reboot_request_time = 0;
  
  if(reboot != reboot_request_acknowledged && reboot){
    Serial.println("[Reboot] Reboot request acknowledged");
    reboot_request_acknowledged = true;
    reboot_request_time = millis();
  }

  if(reboot_request_acknowledged && millis() - reboot_request_time > 1000){
    Serial.println("[Reboot] Rebooting now");
    ESP.restart();
  }
}
