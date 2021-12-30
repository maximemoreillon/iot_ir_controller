#define U_PART U_FS

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}
  
    bool canHandle(AsyncWebServerRequest *request){
      return true;
    }
  
    void handleRequest(AsyncWebServerRequest *request) {
      request->send(LittleFS, "/www/index.html", String(), false, processor);
    }
};

String reboot_html = ""
  "Rebooting..."
  "<script>setTimeout(() => window.location.replace('/'), 10000)</script>";


void web_server_setup(){
  Serial.println(F("[Web server] Web server initialization"));

  
  web_server.serveStatic("/", LittleFS, "/www")
    .setDefaultFile("index.html")
    .setTemplateProcessor(processor);

  web_server.serveStatic("/signals/", LittleFS, "/signals");
    
  web_server.on("/settings", HTTP_POST, update_settings);
  web_server.on("/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
  );

  web_server.on("/record", HTTP_GET, handle_record_ir);

  web_server.onNotFound(handle_not_found);

  // Captive portal
  web_server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  
  web_server.begin();
}

void handle_not_found(AsyncWebServerRequest *request){
  request->send(404, "text/html", "Not found");
}

void save_config(AsyncWebServerRequest *request){
  DynamicJsonDocument doc(1024);
  doc["nickname"] = request->arg("nickname");
  
  JsonObject wifi  = doc.createNestedObject("wifi");
  
  wifi["ssid"] = request->arg("wifi_ssid");
  wifi["password"] = request->arg("wifi_password");

  JsonObject mqtt  = doc.createNestedObject("mqtt");
  mqtt["username"] = request->arg("mqtt_username");
  mqtt["password"] = request->arg("mqtt_password");

  JsonObject broker  = mqtt.createNestedObject("broker");
  broker["host"] = request->arg("mqtt_broker_host");
  broker["port"] = request->arg("mqtt_broker_port");

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("[SPIFFS] Failed to open config file for writing");
    return;
  }

  serializeJson(doc, configFile);
  Serial.println("[SPIFFS] Finished writing config file");
}

void update_settings(AsyncWebServerRequest *request) {

  // TODO: Check if all arguments are set

  save_config(request);

  request->send(200, "text/html", reboot_html);

  // Reboot
  delayed_reboot();
   
}

void handle_record_ir(AsyncWebServerRequest *request){

  int paramsNr = request->params();

  // Select if recording ON or OFF signal
  if(request->hasParam("signal")){
    AsyncWebParameter* p = request->getParam("signal");
    selected_ir_signal_name = p->value();
  }

  start_listening_to_ir();
  String html = "<p>Point your remote at the IR receiver and press the button to record. The LED will turn off once the signal is recorded.</p>";
  request->send(200, "text/html", html);
}




void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    Serial.println("[Update] Update started...");
    size_t content_len = request->contentLength();
    // if filename includes spiffs, update the spiffs partition
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    Update.runAsync(true);
    if (!Update.begin(content_len, cmd)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }
//  else {
//    Serial.printf("Progress: %d%%\n", (Update.progress()*100)/Update.size());
//  }

  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
      request->send(500, "text/html", "Firmware update failed");
    }
    else {
      Serial.println("[Update] Update complete");
      Serial.flush();
      request->send(200, "text/html", reboot_html);
      delayed_reboot();
    }
  }
}
