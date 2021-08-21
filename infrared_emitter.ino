void IR_send_on(){
  selected_ir_signal_address = EEPROM_IR_SIGNAL_ON_ADDRESS;
  IR_TX_started = true;
}

void IR_send_off(){
  selected_ir_signal_address = EEPROM_IR_SIGNAL_OFF_ADDRESS;
  IR_TX_started = true;
}

void handle_IR_TX() {

  if(!IR_TX_started) return;
  
  Serial.print("[IR_TX] Sending signal from address ");
  Serial.print(selected_ir_signal_address);

  int IR_signal_length = read_int_from_eeprom(selected_ir_signal_address);

  Serial.print(" (length: ");
  Serial.print(IR_signal_length);
  Serial.print("): ");

  // Safeguard against EEPROM corruption
  IR_signal_length = min(IR_signal_length,IR_BUFFER_SIZE);

  int IR_signal[IR_signal_length];

  // Read IR signal from EEPROM and store it in temporary array IR_signal
  for(int i = 0; i < IR_signal_length; i++){
    // +2 because code length is stored in position 0
    int IR_signal_bit = read_int_from_eeprom(selected_ir_signal_address + 2*i + 2);
    //IR_signal[i] = IR_signal_bit;
    IR_signal[i] = abs(IR_signal_bit);
  }

  // Replay signal using the IR LED
  for(int i = 0; i < IR_signal_length; i++){
    int IR_signal_bit = IR_signal[i];
    if(i % 2 == 0) IR_send_pulse(IR_signal_bit);
    else IR_send_pause(IR_signal_bit);
  }

  // Print signal to Serial for debugging
  for(int i = 0; i < IR_signal_length; i++){
    int IR_signal_bit = IR_signal[i];
    Serial.print(IR_signal_bit);
    Serial.print(",");
  }
  Serial.println("");


  IR_TX_started = false;
}


void IR_send_pulse(int pulse_length) {
  // Send one pulse of a given length
  
  int IR_on = 0;
  long startMicros = micros();
  while (micros() < (startMicros + pulse_length)){
    
    // toggle pin and wait 26 us to make it a pulse
    IR_on = 1 - IR_on;
    
    digitalWrite(IR_EMITTER_PIN, IR_on);

    // IR LED frequency must be around 37-38kHz
    // Approx pulse period = 26us
    // A period consists of the LED being toggled twice => 13us between toggles
    delayMicroseconds(13);
  }
  
  // turn off IR after pulse is complete
  digitalWrite(IR_EMITTER_PIN, LOW);
}

void IR_send_pause(int pause_length) {
  // A pause is just not doing anything for a given amount of time
  delayMicroseconds(pause_length);
}
