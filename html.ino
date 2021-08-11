/*
 * This file contains the HTML for the web pages
 */

// Head partials
// Style
const String styles = ""
  "<style>"
  "body { font-family: Arial, sans-serif; }"
  "nav { display: flex; }"
  "nav a { margin: 0.25em; }"
  "nav a::first-child {margin-left: 0; }"
  "label { display: inline-block; width: 250px; }"
  "table { border-collapse: collapse; }"
  "/*tr:not(:last-child) { border-bottom: 1px solid #dddddd; }*/"
  "th { text-align: left; width: 200px; }"
  "</style>";

String get_html_head() {
  return ""
    "<head>"
    "<title>" + get_device_name() + "</title>"
    + styles +
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "</head>";
}

// Body partials
String get_html_header(){
  return ""
    "<header>"
    "  <h1>" + get_device_name() + "</h1>"
    "</header>";
}

const String nav = ""
  "<nav>"
    "<a href='/'>Info</a>"
    "<a href='/settings'>Settings</a>"
    "<a href='/update'>Firmware update</a>"
    "<a href='/record?signal=on'>Record ON</a>"
    "<a href='/record?signal=off'>Record OFF</a>"
  "</nav>";

// Main contents
String get_homepage(){
  return ""
    "<h2>Info</h2>"
    "<h3>Device</h3>"
    "<table>"
      "<tr><th>Device type</th><td>" + String(DEVICE_TYPE) + "</td></tr>"
      //"<tr><th>Device ID</th><td>"+ String(DEVICE_ID) +"</td></tr>"
      "<tr><th>Device name</th><td>" + get_device_name() + "</td></tr>"
      "<tr><th>Device nickname</th><td>" + get_device_nickname() + "</td></tr>"
      "<tr><th>Firmware version</th><td>"+ String(DEVICE_FIRMWARE_VERSION) +"</td></tr>"
      "<tr><th>State</th><td>"+ device_state +"</td></tr>"
    "</table>"
    "<h3>WiFi</h3>"
    "<table>"
      "<tr><th>Mode</th><td>" + wifi_mode + "</td></tr>"
    "</table>"
    "<h3>STA settings</h3>"
    "<table>"
      "<tr><th>SSID</th><td>" + get_wifi_ssid_from_eeprom() + "</td></tr>"
      "<tr><th>IP</th><td>" + WiFi.localIP().toString() + "</td></tr>"
    "</table>"
    "<h3>AP settings</h3>"
    "<table>"
      "<tr><th>SSID</th><td>" + get_softap_ssid() + "</td></tr>"
      "<tr><th>IP</th><td>" + WIFI_AP_IP.toString() + "</td></tr>"
    "</table>"
    "<h3>MQTT</h3>"
    "<table>"
      "<tr><th>Broker URL</th><td>" + String(MQTT_BROKER_ADDRESS) + "</td></tr>"
      "<tr><th>Username</th><td>" + get_mqtt_username() + "</td></tr>"
      "<tr><th>Client ID</th><td>" + get_device_name() + "</td></tr>"
      "<tr><th>Connection</th><td>" + (mqtt_connected() ? "connected" : "disconnected") + "</td></tr>"
      "<tr><th>Status topic</th><td>" + get_mqtt_status_topic() + "</td></tr>"
      "<tr><th>Command topic</th><td>" + get_mqtt_command_topic() + "</td></tr>"
    "</table>"
    ;
}
  
String get_settings_form(){
  return ""
  "<h2>Settings</h2>"
  "<form method='POST' action='/settings' enctype='multipart/form-data'>"
    "<p>"
      "<label>WiFi SSID</label>"
      "<input type='text'  name='wifi_ssid' placeholder='Wifi ssid' value='" + get_wifi_ssid_from_eeprom() + "'>"
    "</p>"
    "<p>"
      "<label>WiFi password</label>"
      "<input type='password' name='wifi_password' placeholder='Wifi password' value='" + get_wifi_password_from_eeprom() + "'>"
    "</p>"
    "<p>"
      "<label>MQTT username</label>"
      "<input type='text' name='mqtt_username' placeholder='MQTT username' value='" + get_mqtt_username() + "'>"
    "</p>"
    "<p>"
      "<label>MQTT password</label>"
      "<input type='password' name='mqtt_password' placeholder='MQTT password' value='" + get_mqtt_password() + "'>"
    "</p>"
    "<p>"
      "<label>Device nickname</label>"
      "<input type='text' name='device_nickname' placeholder='My Device' value='" + get_device_nickname() + "'>"
    "</p>"
    "<p>"
      "<input type='submit' value='Update'>"
    "</p>"
  "</form>";
}

String firmware_update_form = ""
  "<h2>Firmware update</h2>"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
  "</form>";

String wifi_registration_success = ""
  "<h2>Settings</h2>"
  "<p>OK, rebooting...</p>";

String wifi_registration_failure = ""
  "<h2>Settings</h2>"
  "<p>Failed to parse WiFi settings</p>";


String firmware_update_success = ""
  "<h2>Firmware update</h2>"
  "<p>Upload successful, rebooting...</p>";

String html_ok = ""
  "<p>Point your remote at the IR receiver and press the button to record.</p>";

String html_not_found = ""
  "<p>404 Not found</p>";

  
String firmware_update_failure = ""
  "<h2>Firmware update</h2>"
  "<p>Upload failed</p>";
  
// Assembling the page
String apply_html_template(String main_content){
  return ""
    "<html>"
    + get_html_head() 
    + "<body>"
    + get_html_header()
    + nav 
    + "<main>"
    + main_content
    + "</main>"
    "</body>"
    "</html>";
}
