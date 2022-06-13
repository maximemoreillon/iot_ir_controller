void io_setup(){
  Serial.println("[IO] IO setup");
  pinMode(IR_EMITTER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(IR_EMITTER_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
}
