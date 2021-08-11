/*
 * Original code from AnalysIR
 * 
 * Notes: Result can be stored in an int array
 */

void start_listening_to_ir(){
  Serial.print("[IR_RX] Listening for IR signal: ");
  Serial.println(selected_ir_signal_address);
  attachInterrupt(digitalPinToInterrupt(IR_RECEIVER_PIN), infrared_receiver_interrupt_handler, CHANGE);
}

void stop_listening_to_ir(){
  Serial.print("[IR_RX] Stopped listening for IR signal: ");
  Serial.println(selected_ir_signal_address);
  detachInterrupt(digitalPinToInterrupt(IR_RECEIVER_PIN));
}

void handle_IR_RX(){

  long current_time = millis();
  if(IR_RX_index && ( ( current_time - IR_RX_last_change) > 1000) ){
    Serial.println("[IR_RX] IR signal finished");
    stop_listening_to_ir();

    int code_length = IR_RX_index;


    // Write code length
    write_int_to_eeprom(selected_ir_signal_address, code_length);

    // Save code to EEPROM
    for (int i = 0; i < code_length; i++) {
      write_int_to_eeprom(selected_ir_signal_address + 2*i + 2, IR_RX_buffer[i]);
    }

    if (EEPROM.commit()) Serial.println("[EEPROM] commit successful");
    else Serial.println("[EEPROM] commit FAILED");


    read_code_from_eeprom(selected_ir_signal_address);
    
    // Reset the index
    IR_RX_index = 0;

  }

}
  
  

ICACHE_RAM_ATTR void infrared_receiver_interrupt_handler() {
  // The IR receiver pin has changed
  // By default, the pin is HIGH
  // At first call, the pin changes from HIGH to LOW
  

  static int last_change_time_micros;
  const int current_time_micros = micros(); 

  if(millis() - IR_RX_last_change < 1000) {
    IR_RX_buffer[IR_RX_index++] = current_time_micros - last_change_time_micros;
  }
  
  last_change_time_micros = current_time_micros; 
  IR_RX_last_change = millis();

}
