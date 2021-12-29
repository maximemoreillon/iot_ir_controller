void spiffs_setup() {

  Serial.println("[SPIFFS] Retrieving config from SPIFFS");
  
  if (!LittleFS.begin()) {
    Serial.println("[SPIFFS] Failed to mount file system");
  }
  else {
    Serial.println("[SPIFFS] File system mounted");
  }
}

void get_config_from_spiffs(){

  StaticJsonDocument<1024> doc;

  
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("[SPIFFS] Failed to open config file");
    return;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("[SPIFFS] Config file size is too large");
    return;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("[SPIFFS] Failed to parse config file");
    return;
  }

  // export file content to custom config structure
  config.nickname = doc["nickname"].as<String>();
  config.wifi.ssid = doc["wifi"]["ssid"].as<String>();
  config.wifi.password = doc["wifi"]["password"].as<String>();
  config.mqtt.broker.host = doc["mqtt"]["broker"]["host"].as<String>();
  config.mqtt.broker.port = doc["mqtt"]["broker"]["port"].as<int>();
  config.mqtt.username = doc["mqtt"]["username"].as<String>();
  config.mqtt.password = doc["mqtt"]["password"].as<String>();

}

void readFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void readFile_beta(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");

  int values[1024];
  int index = 0;
  
  while ( file.available() ) {
    int value = file.readStringUntil('\n').toInt();
    values[index] = value;
    index ++;
  }

  Serial.print("Stored length: ");
  Serial.println(index);

  
  
  file.close();
  
      
}
