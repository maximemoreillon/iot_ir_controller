boolean wifi_connected(){
  return WiFi.status() == WL_CONNECTED;
}

String get_softap_ssid(){
  return get_device_name();
}


void attempt_sta(){
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(get_device_name());

  
  // TODO: Test if key exists
  String wifi_sta_ssid = config.wifi.ssid;
  String wifi_sta_password = config.wifi.password;
  
  // Debugging
  Serial.print("[WiFi] Attempting connection to ");
  Serial.print(wifi_sta_ssid);
  Serial.print(", with password: ");
  Serial.println(wifi_sta_password);

  // Use password or not depending of if provided
  if(wifi_sta_password == "") WiFi.begin(wifi_sta_ssid.c_str());
  else WiFi.begin(wifi_sta_ssid.c_str(), wifi_sta_password.c_str());

  // Attempt connection for a given amount of time
  long now = millis();
  while(millis() - now < WIFI_STA_CONNECTION_TIMEOUT && !wifi_connected()){
    delay(10); // Do nothing while connecting
  }
  
}

void wifi_setup() {
  
  Serial.println(F("[WiFi] Wifi starting"));


  attempt_sta();
  

  if(wifi_connected()){
    Serial.print(F("[WIFI] Connected, IP: "));
    Serial.println(WiFi.localIP());
  }

  else {
    /*
     * If the connection to the wifi is not possible,
     * Create a Wifi access point
     * Create a web server
     * Direct clients to the web server upon connection to the Wifi Access point
     */
    
    Serial.println("[Wifi] Cannot connect to provided WiFi, starting access point...");
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    WiFi.persistent(false);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(get_softap_ssid().c_str());

    // Debugging
    Serial.println("[WiFi] Access point initialized");

    // Experiment
    wifi_mode = "AP";
  }

}


void wifi_connection_manager(){
  // Checks for changes in connection status
  // Currently for STA mode

  static boolean last_connection_state = false;

  

  if(wifi_connected() != last_connection_state) {
    last_connection_state = wifi_connected();


    if(wifi_connected()){
      Serial.print(F("[WIFI] Connected, IP: "));
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println(F("[WIFI] Disconnected"));
    }
  }

}
