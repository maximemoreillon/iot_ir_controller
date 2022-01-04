void mqtt_config(){
  iot_kernel.mqtt.setCallback(mqtt_message_callback);
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
      IR_send_on();
    }
    else if( strcmp(command, "off")==0 ) {
      IR_send_off();
    }
    
  }
  else {
    Serial.println("[MQTTT] Payload is NOT JSON with state");
    if(strncmp((char*) payload, "on", payload_length) == 0){
      IR_send_on();
    }
    else if(strncmp((char*) payload, "off", payload_length) == 0){
      IR_send_off();
    }
  }


  if(inbound_JSON_message.containsKey("record")){

    Serial.println("[MQTTT] Payload is RECORD");
    
    // Check what the command is and act accordingly
    String signal_name = inbound_JSON_message["record"].as<String>();  
    
    selected_ir_signal_name = signal_name;
    start_listening_to_ir();
    
  }


}
