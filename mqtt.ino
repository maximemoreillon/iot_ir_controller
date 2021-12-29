void MQTT_setup(){
  Serial.print(F("[MQTT] Initializing MQTT with broker "));
  Serial.print(config.mqtt.broker.host);
  Serial.print(":");
  Serial.println(config.mqtt.broker.port);
  MQTT_client.setServer(config.mqtt.broker.host.c_str(), config.mqtt.broker.port);
  MQTT_client.setCallback(mqtt_message_callback);
}

boolean mqtt_connected(){
  return MQTT_client.connected();
}

String get_mqtt_username(){
  return config.mqtt.username;
}

String get_mqtt_password(){
  return config.mqtt.password;
}

String get_mqtt_base_topic(){
  return "/" + get_mqtt_username() + "/" + get_device_name();
}

String get_mqtt_status_topic(){
  return get_mqtt_base_topic() + "/status";
}

String get_mqtt_command_topic(){
  return get_mqtt_base_topic() + "/command";
}




void MQTT_connection_manager(){

  static boolean last_connection_state = false;
  
  static long last_connection_attempt;
  
  if(mqtt_connected() != last_connection_state) {
    last_connection_state = mqtt_connected();

    if(mqtt_connected()){
      // Changed from disconnected to connected
      Serial.println(F("[MQTT] Connected"));
      
      Serial.println("[MQTT] Subscribing to topic " + get_mqtt_command_topic());
      MQTT_client.subscribe(get_mqtt_command_topic().c_str());

      mqtt_publish_state();
    }
    else {
      // Changed from connected to disconnected
      Serial.print(F("[MQTT] Disconnected: "));
      Serial.println(MQTT_client.state());

      
    }
  }

  // Kind of similar to the pubsubclient example, one connection attempt every 5 seconds
  if(!mqtt_connected()){
    if(millis() - last_connection_attempt > MQTT_RECONNECT_PERIOD){
      last_connection_attempt = millis();

      // No need to do anything if not connected to WiFi
      if(!wifi_connected()) return;
      
      Serial.println(F("[MQTT] Connecting"));


      // Last will
      StaticJsonDocument<MQTT_MAX_PACKET_SIZE> outbound_JSON_message;
  
      outbound_JSON_message["connected"] = false;
      outbound_JSON_message["type"] = DEVICE_TYPE;
      outbound_JSON_message["firmware_version"] = DEVICE_FIRMWARE_VERSION;
      outbound_JSON_message["nickname"] = get_device_nickname();
      outbound_JSON_message["address"] = WiFi.localIP().toString();

      char mqtt_last_will[MQTT_MAX_PACKET_SIZE];
      serializeJson(outbound_JSON_message, mqtt_last_will, sizeof(mqtt_last_will));
      
      MQTT_client.connect(
        get_device_name().c_str(),
        get_mqtt_username().c_str(), 
        get_mqtt_password().c_str(),
        get_mqtt_status_topic().c_str(),
        MQTT_QOS,
        MQTT_RETAIN,
        mqtt_last_will
      );
    }
  }
  
}

void mqtt_message_callback(char* topic, byte* payload, unsigned int payload_length) {
  
  Serial.print(F("[MQTT] message received on "));
  Serial.print(topic);
  Serial.print(F(", payload: "));
  for (int i = 0; i < payload_length; i++) Serial.print((char)payload[i]);
  Serial.println("");

  // Create a JSON object to hold the message
  // Note: size is limited by MQTT library
  StaticJsonDocument<MQTT_MAX_PACKET_SIZE> inbound_JSON_message;

  // Copy the message into the JSON object
  deserializeJson(inbound_JSON_message, payload);

  if(inbound_JSON_message.containsKey("state")){

    Serial.println("[MQTTT] Payload is JSON with state");
    
    // Check what the command is and act accordingly
    const char* command = inbound_JSON_message["state"];  
    
    if( strcmp(command, "on")==0 ) {
      device_state = "on";
      IR_send_on();
      mqtt_publish_state();
    }
    else if( strcmp(command, "off")==0 ) {
      device_state = "off";
      IR_send_off();
      mqtt_publish_state();
    }
    
  }
  else {
    Serial.println("[MQTTT] Payload is NOT JSON with state");
    if(strncmp((char*) payload, "on", payload_length) == 0){
      device_state = "on";
      IR_send_on();
      mqtt_publish_state();
    }
    else if(strncmp((char*) payload, "off", payload_length) == 0){
      device_state = "off";
      IR_send_off();
      mqtt_publish_state();
    }
  }

  
  

  

  
}

void mqtt_publish_state(){
  Serial.println("[MQTT] State published");


  StaticJsonDocument<MQTT_MAX_PACKET_SIZE> outbound_JSON_message;
  
  outbound_JSON_message["connected"] = true;
  outbound_JSON_message["state"] = device_state;
  outbound_JSON_message["type"] = DEVICE_TYPE;
  outbound_JSON_message["nickname"] = get_device_nickname();
  outbound_JSON_message["firmware_version"] = DEVICE_FIRMWARE_VERSION;
  outbound_JSON_message["address"] = WiFi.localIP().toString();
  
  char mqtt_payload[MQTT_MAX_PACKET_SIZE];
  serializeJson(outbound_JSON_message, mqtt_payload, sizeof(mqtt_payload));
  MQTT_client.publish(get_mqtt_status_topic().c_str(), mqtt_payload, MQTT_RETAIN);
  
}
