/*
 * Original code from AnalysIR
 * 
 * Notes: Result can be stored in an int array
 */

void start_listening_to_ir(){
  digitalWrite(LED_PIN, LOW);
  Serial.print("[IR_RX] Listening for IR signal: ");
  Serial.println(selected_ir_signal_name);
  Serial.println("");
  attachInterrupt(digitalPinToInterrupt(IR_RECEIVER_PIN), infrared_receiver_interrupt_handler, CHANGE);
}

void stop_listening_to_ir(){
  digitalWrite(LED_PIN, HIGH);
  Serial.print("[IR_RX] Stopped listening for IR signal: ");
  Serial.println(selected_ir_signal_name);
  Serial.println("");
  detachInterrupt(digitalPinToInterrupt(IR_RECEIVER_PIN));
}

void handle_IR_RX(){

  if(IR_RX_index && ( ( millis() - IR_RX_last_change) > 1000) ){

  
    Serial.println("[IR_RX] IR signal finished");
    stop_listening_to_ir();
  
    unsigned int code_length = IR_RX_index;
  
    Serial.print("[IR_RX] Signal length: ");
    Serial.println(code_length);
  
    const String file_name = "/signals/" + selected_ir_signal_name + ".txt";
  
    File file = LittleFS.open(file_name, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      IR_RX_index = 0;
      return;
    }
  
    // Save code to SPIFFS
    for (int i = 0; i < code_length; i++) {
      file.println(IR_buffer[i]);
    }
    
    file.close();
  
    Serial.print("[SPIFFS] Saved signal as: ");
    Serial.println(file_name);
  
    // Reset the index
    IR_RX_index = 0;

  }
  

}
  
  

ICACHE_RAM_ATTR void infrared_receiver_interrupt_handler() {
  // Interrupt routine checking if The IR receiver pin has changed
  // By default, the pin is HIGH
  // At first call, the pin changes from HIGH to LOW
  // Records the timings in an array
  

  static int last_change_time_micros;
  const int current_time_micros = micros(); 
  const int timeout = 1000;

  // includes prevention of  buffer overflow
  if(millis() - IR_RX_last_change < timeout && IR_RX_index < IR_BUFFER_SIZE) {
    IR_buffer[IR_RX_index] = current_time_micros - last_change_time_micros;
    IR_RX_index ++;
  }
  
  last_change_time_micros = current_time_micros; 
  IR_RX_last_change = millis();

}
