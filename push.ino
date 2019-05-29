void Push_init() {

  ts.add(PUSH, push_update_int, [&](void*) {

    if (!busy) handle_push();

  }, nullptr, true);


  HTTP.on("/pushDate", HTTP_GET, []() {

    String host_str = HTTP.arg("pushHost");
    int httpsPort = HTTP.arg("pushPort").toInt();
    String fingerprint_str = HTTP.arg("pushFingerprint");
    String accessToken_str = HTTP.arg("pushAccessToken");

    jsonWrite(configSetup, "pushHost", host_str);
    jsonWrite(configSetup, "pushPort", httpsPort);
    jsonWrite(configSetup, "pushFingerprint", fingerprint_str);
    jsonWrite(configSetup, "pushAccessToken", accessToken_str);

    saveConfig();

    const char* host = host_str.c_str();
    const char* fingerprint = fingerprint_str.c_str();
    String result;
    WiFiClientSecure client_push;

    client_push.setFingerprint(fingerprint);

    if (client_push.connect(host, httpsPort)) {
      result = "Connected";
      send_push(jsonRead(configSetup, "SSDP"), "test");
    } else {
      result = "Connection failed";
    }
    String tmp = "{}";
    jsonWrite(tmp, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>" + result);
    jsonWrite(tmp, "class", "pop-up");
    HTTP.send(200, "application/json", tmp);

    /*if (!client_push.verify(fingerprint, host)) {
      result = "Fingerprint error";
      String tmp = "{}";
      jsonWrite(tmp, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>" + result);
      jsonWrite(tmp, "class", "pop-up");
      HTTP.send(200, "application/json", tmp);
      }*/

  });


  HTTP.on("/module_push", HTTP_GET, []() {
    
    jsonWrite(configSetup, "module_push", HTTP.arg("status"));
    saveConfig();

    HTTP.send(200, "text/plain", "OK");
  });

   if (jsonRead(configSetup, "module_push") == "1") send_push(jsonRead(configSetup, "SSDP"), "устройство_включено");

}

void send_push(String title, String body) {
  
  
  order_push += title + " " + body + ","; //Аналог >100=361,
  //Serial.print(order_push);  

}


void handle_push() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connected()) {
      if (order_push != "") {                                                  //test 1,test 2
        String tmp = selectToMarker(order_push, ",");                          //test 1,

        String title = selectFromMarkerToMarker(tmp, " ", 0);                  //test
        String body = selectFromMarkerToMarker(tmp, " ", 1);                   //1,
        body.replace(",", "");                                                 //1

        //------------------------------------------------------------------
        String host_str = jsonRead(configSetup, "pushHost");
        String fingerprint_str = jsonRead(configSetup, "pushFingerprint");
        String accessToken_str = jsonRead(configSetup, "pushAccessToken");

        const char* host = host_str.c_str();
        int httpsPort = jsonRead(configSetup, "pushPort").toInt();
        const char* fingerprint = fingerprint_str.c_str();
        const char* accessToken = accessToken_str.c_str();
        //------------------------------------------------------------------

        // Use WiFiClientSecure class to create TLS connection
        Serial.print("sending push: \"");
        WiFiClientSecure client_push;
        //Serial.print("connecting to ");
        //Serial.println(host);

        client_push.setFingerprint(fingerprint);

        if (!client_push.connect(host, httpsPort)) {
          Serial.print("push connection failed: \"");
          return;
        }

        /*if (client_push.verify(fingerprint, host)) {
          Serial.println("certificate matches");
          } else {
          Serial.println("certificate doesn't match");
          }*/
        String final_line = "{\"body\":\"";
        final_line += body;
        final_line += "\",\"title\":\"";
        final_line += title;
        final_line += "\",\"type\":\"note\"}";

        String length_ = String(final_line.length());

        //Serial.println(length_);

        String url = "/v2/pushes";
        //Serial.print("requesting URL: ");
        //Serial.println(url);

        client_push.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "User-Agent: ESP8266\r\n" +
                          "Access-Token: " + accessToken + "\r\n" +
                          "Content-length: " + length_ + "\r\n"
                          "Content-Type: application/json\r\n" +
                          "Connection: close\r\n\r\n" +
                          final_line
                         );

        /* while (client_push.connected()) {
           String line = client_push.readStringUntil('\n');
           if (line == "\r") {
             Serial.println("headers received");
             break;
           }
          }*/

        /*
          //String line = client_push.readStringUntil('\n');
          String line = client_push.readString(); // this line is good for debugging error messages more then a single line

          Serial.println(line);

          if (line.indexOf("{\"active\":true") > 0) {
          Serial.println("=>sending push successfull!");
          } else {
          Serial.println("=>sending push failed");
          return;
          }
        */
        //--------------------------------------------------------------------------------------------------------------------------
        Serial.print(tmp);
        order_push = deleteBeforeDelimiter(order_push, ",");    
        Serial.println("\" done");
        return;
      }
    }
  }
}

