 /*
 * IOT INFRARED CONTROLLER
 * Maxime MOREILLON
 * 
 * Board type: Wemos D1 Mini
 */

#include "iotKernel.h"


// Information to identify the device
#define DEVICE_TYPE "ircontroller"
#define DEVICE_FIRMWARE_VERSION "0.2.0"

// IO
#define LED_PIN 2 // D4
#define IR_RECEIVER_PIN 12 // D6 
#define IR_EMITTER_PIN 14 // D5


// IR
#define IR_BUFFER_SIZE 1024


IotKernel iot_kernel(DEVICE_TYPE,DEVICE_FIRMWARE_VERSION); 

// IR
String selected_ir_signal_name = "on";

// IR RX
volatile long IR_buffer[IR_BUFFER_SIZE]; //stores timings - volatile because changed by ISR
volatile unsigned int IR_RX_index = 0; //Pointer through IR_buffer - volatile because changed by ISR
volatile long IR_RX_last_change; // volatile because changed by ISR

// IR TX
boolean IR_TX_started = false;




void setup() {

  io_setup();
  
  iot_kernel.init();
  
  http_config();
  mqtt_config();
  
}



void loop() {
  iot_kernel.loop();

  handle_IR_RX();
  handle_IR_TX();
}
