void IR_send_on(){
  selected_ir_signal_name = "on";
  IR_TX_started = true;
}

void IR_send_off(){
  selected_ir_signal_name = "off";
  IR_TX_started = true;
}

void handle_IR_TX() {

  if(!IR_TX_started) return;
  
  Serial.print("[IR_TX] Sending signal: ");
  Serial.println(selected_ir_signal_name);

  const String file_name = "/signals/" + selected_ir_signal_name + ".txt";

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    Serial.println("[SPIFFS] Failed to open file for reading");
    IR_TX_started = false;
    return;
  }

  Serial.print("[SPIFFS] Loaded signal from: ");
  Serial.println(file_name);

  int index = 0;
  
  while ( file.available() ) {
    long value = file.readStringUntil('\n').toInt();
    IR_buffer[index] = value;
    index ++;
  }

  file.close();

  

  int IR_signal_length = index;
  Serial.print("[IR_TX] Replaying signal of length ");
  Serial.println(IR_signal_length);

  // Replay signal using the IR LED
  // THIS TRIGGERS THE WATCHDOG
  for(int i = 0; i < IR_signal_length; i++){
    long IR_signal_bit = IR_buffer[i];
    if(i % 2 == 0) IR_send_pulse(IR_signal_bit); // problem here
    else IR_send_pause(IR_signal_bit);
  }

  
  IR_TX_started = false;
  
  Serial.println("[IR_TX] Sending signal complete");
  
}


void IR_send_pulse(long pulse_length) {
  // Send one pulse of a given length

  // Pin must be toggling between ON and OFF at 37-38kHz
  // This variable keeps track of wether the pin is ON or OFF
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

void IR_send_pause(long pause_length) {
  // A pause is just not doing anything for a given amount of time
  digitalWrite(IR_EMITTER_PIN, LOW);
  delayMicroseconds(pause_length);
  
}
