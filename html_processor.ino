String processor(const String& var){

  
  if(var == "DEVICE_NAME") return get_device_name();
  else if(var == "DEVICE_TYPE") return DEVICE_TYPE;
  else if(var == "DEVICE_FIRMWARE_VERSION") return DEVICE_FIRMWARE_VERSION;
  else if(var == "DEVICE_NICKNAME") return config.nickname;
  else if(var == "MQTT_BROKER_HOST") return config.mqtt.broker.host;
  else if(var == "MQTT_BROKER_PORT") return String(config.mqtt.broker.port);
  else if(var == "MQTT_USERNAME") return config.mqtt.username;
  else if(var == "MQTT_PASSWORD") return config.mqtt.password;
  else if(var == "WIFI_SSID") return config.wifi.ssid;
  else if(var == "WIFI_SSID") return config.wifi.password;
  return String();
}
