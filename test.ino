void test() {
 HTTP.on("/test", HTTP_GET, []() {
       
    sendMQTT("config", "1");
    
    HTTP.send(200, "text/plain", "OK");
  });
}


