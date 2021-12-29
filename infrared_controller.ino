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
#include <DNSServer.h> // DNS server to redirect wifi clients to the web server
#include <ArduinoJson.h> // JSON, used for the formatting of messages sent to the server
#include <Updater.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <LittleFS.h>


// Information to identify the device
#define DEVICE_TYPE "ircontroller"
#define DEVICE_FIRMWARE_VERSION "0.2.0"

// IO
#define LED_PIN 2 // D4
#define IR_RECEIVER_PIN 12 // D6 
#define IR_EMITTER_PIN 14 // D5

// MQTT settings
#define MQTT_RECONNECT_PERIOD 1000
#define MQTT_QOS 1
#define MQTT_RETAIN true

// IR
#define IR_BUFFER_SIZE 1024


// WIFI settings
#define WIFI_STA_CONNECTION_TIMEOUT 20000
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
boolean reboot = false;

// IR
//int selected_ir_signal_address = EEPROM_IR_SIGNAL_ON_ADDRESS;
String selected_ir_signal_name = "on";

// IR RX
volatile long IR_buffer[IR_BUFFER_SIZE]; //stores timings - volatile because changed by ISR
volatile unsigned int IR_RX_index = 0; //Pointer through IR_buffer - volatile because changed by ISR
volatile long IR_RX_last_change; // volatile because changed by ISR

// IR TX
boolean IR_TX_started = false;

// Need a custom structure to hold device config
struct WifiConfig {
  String ssid;
  String password;
};

struct MqttBrokerConfig {
  String host;
  int port;
};

struct MqttConfig {
  MqttBrokerConfig broker;
  String username;
  String password;
};

struct DeviceConfig {
  String nickname;
  WifiConfig wifi;
  MqttConfig mqtt;
};

DeviceConfig config;



void setup() {
  delay(10);
  Serial.begin(115200);
  Serial.println("");
  Serial.println(F("Iot IR controller"));

  io_setup();
  spiffs_setup();
  get_config_from_spiffs();
  wifi_client.setInsecure();
  wifi_setup();
  MQTT_setup();
  dns_server.start(DNS_PORT, "*", WIFI_AP_IP);
  MDNS.begin(get_device_name().c_str());
  web_server_setup();
  
}



void loop() {
  wifi_connection_manager();
  MQTT_connection_manager();
  MQTT_client.loop();
  //dns_server.processNextRequest();
  //MDNS.update();
  
  handle_IR_RX();
  handle_IR_TX();

  

  handle_reboot();
}
