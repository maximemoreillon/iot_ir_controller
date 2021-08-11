#define U_PART U_FS

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    String html = apply_html_template(get_homepage());
    request->send(200, "text/html", html);
  }
};

void web_server_setup(){
  Serial.println(F("[Web server] Web server initialization"));
  
  web_server.on("/", HTTP_GET, handle_homepage);
  
  web_server.on("/settings", HTTP_GET, get_settings);
  web_server.on("/settings", HTTP_POST, update_settings);

  web_server.on("/record", HTTP_GET, handle_record_ir);

  
  web_server.on("/update", HTTP_GET, handle_update_form);
  web_server.on("/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
  );

  web_server.onNotFound(handle_not_found);
  web_server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  web_server.begin();
}

void handle_homepage(AsyncWebServerRequest *request) {
  String html = apply_html_template(get_homepage());
    request->send(200, "text/html", html);
}

void get_settings(AsyncWebServerRequest *request){
  String html = apply_html_template(get_settings_form());
  request->send(200, "text/html", html);
}


void save_arg_in_eeprom(AsyncWebServerRequest *request, String arg_name, int address){
  if(!request->hasArg(arg_name.c_str())) return;
  String arg_value = request->arg(arg_name.c_str());
  Serial.println(arg_value);
  write_string_to_eeprom(arg_value, address);
  Serial.println("[EEPROM] Saving " + arg_name + " : " + arg_value);
}
void update_settings(AsyncWebServerRequest *request) {

  // TODO: Check if all arguments are set

  save_arg_in_eeprom(request, "wifi_ssid", EEPROM_WIFI_SSID_ADDRESS);
  save_arg_in_eeprom(request, "wifi_password", EEPROM_WIFI_PASSWORD_ADDRESS);
  save_arg_in_eeprom(request, "mqtt_username", EEPROM_MQTT_USERNAME_ADDRESS);
  save_arg_in_eeprom(request, "mqtt_password", EEPROM_MQTT_PASSWORD_ADDRESS);
  save_arg_in_eeprom(request, "device_nickname", EEPROM_DEVICE_NICKNAME_ADDRESS);

  // Respond to the client
  String html = apply_html_template(wifi_registration_success);
  request->send(200, "text/html", html);

  // Reboot
  // TODO: delay
  ESP.restart();
   
}

void handle_record_ir(AsyncWebServerRequest *request){

  int paramsNr = request->params();
  if(request->hasParam("signal")){
    AsyncWebParameter* p = request->getParam("signal");
    if(p->value() == "on") selected_ir_signal_address = EEPROM_IR_SIGNAL_ON_ADDRESS;
    if(p->value() == "off") selected_ir_signal_address = EEPROM_IR_SIGNAL_OFF_ADDRESS;
  }
  

  start_listening_to_ir();
  String html = apply_html_template(html_ok);
  request->send(404, "text/html", html);
}

void handle_not_found(AsyncWebServerRequest *request){
  String html = apply_html_template(html_not_found);
  request->send(404, "text/html", html);
}

void handle_update_form(AsyncWebServerRequest *request){
  String html = apply_html_template(firmware_update_form);
  request->send(200, "text/html", html);
}

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    Serial.println("Update");
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
  } else {
    Serial.printf("Progress: %d%%\n", (Update.progress()*100)/Update.size());
  }

  if (final) {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    response->addHeader("Refresh", "20");  
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Update complete");
      Serial.flush();
      ESP.restart();
    }
  }
}
