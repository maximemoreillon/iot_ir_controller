void http_config(){
  
  iot_kernel.http.serveStatic("/signals/", LittleFS, "/signals");
  iot_kernel.http.on("/record", HTTP_GET, handle_record_ir);
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
