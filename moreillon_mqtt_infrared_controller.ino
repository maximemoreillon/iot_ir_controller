/*
 * IOT INFRARED CONTROLLER
 * Maxime MOREILLON
 * 
 * Board type: Wemos D1 Mini
 */

// Libraries
#include <ESP8266WiFi.h> // Main ESP8266 library
#include <PubSubClient.h> // MQTT
#include <WiFiClientSecure.h> // Wifi client, used by MQTT
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h> // EEPROM for saving settings
#include <DNSServer.h> // DNS server to redirect wifi clients to the web server
#include <ArduinoJson.h> // JSON, used for the formatting of messages sent to the server
#include <Updater.h>
#include <ESP8266mDNS.h>


// Information to identify the device
#define DEVICE_TYPE "ircontroller"
#define DEVICE_FIRMWARE_VERSION "0.0.3"

// Pin mapping
#define RELAY_PIN 12
#define LED_PIN 13

// MQTT settings
#define MQTT_RECONNECT_PERIOD 1000
#define MQTT_BROKER_ADDRESS "mqtt.iot.maximemoreillon.com"
#define MQTT_PORT 30883
#define MQTT_QOS 1
#define MQTT_RETAIN true

// IR
#define IR_RECEIVER_PIN D6
#define IR_EMITTER_PIN D5
#define IR_BUFFER_SIZE 400

// EEPROM
#define EEPROM_WIFI_SSID_ADDRESS 0
#define EEPROM_WIFI_PASSWORD_ADDRESS 50
#define EEPROM_MQTT_USERNAME_ADDRESS 100
#define EEPROM_MQTT_PASSWORD_ADDRESS 150
#define EEPROM_DEVICE_NICKNAME_ADDRESS 200
#define EEPROM_IR_SIGNAL_ON_ADDRESS 250
#define EEPROM_IR_SIGNAL_OFF_ADDRESS 1100

// WIFI settings
#define WIFI_AP_IP IPAddress(192, 168, 4, 1)

// Web server settings
#define WEB_SERVER_PORT 80

// MISC
#define DNS_PORT 53

// Initialize objects
WiFiClientSecure wifi_client; 
PubSubClient MQTT_client(wifi_client);
AsyncWebServer web_server(WEB_SERVER_PORT);
DNSServer dns_server;

// Device
String device_state = "off";
String wifi_mode = "STA";

// IR
int selected_ir_signal_address = EEPROM_IR_SIGNAL_ON_ADDRESS;

// IR RX
volatile unsigned int IR_RX_buffer[IR_BUFFER_SIZE]; //stores timings - volatile because changed by ISR
volatile unsigned int IR_RX_index = 0; //Pointer thru IR_RX_buffer - volatile because changed by ISR
volatile long IR_RX_last_change; // volatile because changed by ISR

// IR TX
boolean IR_TX_started = false;

void setup() {
  delay(10);
  Serial.begin(115200);
  Serial.println("");
  Serial.println(F("Iot IR controller"));
  
  EEPROM.begin(2048); // Webpages get messed up when bigger than 2048
  pinMode(IR_EMITTER_PIN, OUTPUT);
  digitalWrite(IR_EMITTER_PIN, LOW);
  wifi_client.setInsecure();
  wifi_setup();
  MQTT_setup();
  dns_server.start(DNS_PORT, "*", WIFI_AP_IP);
  web_server_setup();
  MDNS.begin(get_device_name().c_str());
}



void loop() {
  wifi_connection_manager();
  MQTT_connection_manager();
  MQTT_client.loop();
  dns_server.processNextRequest();
  MDNS.update();
  
  handle_IR_RX();
  handle_IR_TX();
}
