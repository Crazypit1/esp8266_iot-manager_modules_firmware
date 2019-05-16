/*
void Push_init() {

  ts.add(PUSH, push_update_int, [&](void*) {

       if (!busy) handle_push();

  }, nullptr, true);

  //  ts.add(TEST, 60000, [&](void*) {

  //    static int count = 0;
  //    count++;
  //    send_push("test", String(count));

  //  }, nullptr, true);

}

void send_push(String title, String body) {

  order_push += title + " " + body + ","; //Аналог >100=361,
  Serial.println(order_push);

}


void handle_push() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connected()) {
      if (order_push != "") {                                                  //test 1,test 2
        String tmp = selectToMarker(order_push, ",");                          //test 1,

        String title = selectFromMarkerToMarker(tmp, " ", 0);                  //test
        String body = selectFromMarkerToMarker(tmp, " ", 1);                   //1,
        body.replace(",", "");                                                 //1

        //-------------------------------------------------------------------------------------------------------------------
        WiFiClientSecure client_push;
        if (!client_push.connect(host, httpsPort)) {
          Serial.println("no connected");
        }

        /*if (client_push.verify(fingerprint, host)) {
          Serial.println("certificate matches");
          } else {
          Serial.println("certificate doesn't match");
          }*/

         /*
        String final_line = "{\"body\":\"";
        final_line += body;
        final_line += "\",\"title\":\"";
        final_line += title;
        final_line += "\",\"type\":\"note\"}";
        String length_ = String(final_line.length());
        String url = "/v2/pushes";
        client_push.print(String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "User-Agent: ESP8266\r\n" +
                          "Access-Token: " + accessToken + "\r\n" +
                          "Content-length: " + length_ + "\r\n"
                          "Content-Type: application/json\r\n" +
                          "Connection: close\r\n\r\n" +
                          final_line
                         );

        while (client_push.connected()) {
          String line = client_push.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client_push.readStringUntil('\n');
        // String line = client_push.readString(); // this line is good for debugging error messages more then a single line
        if (line.startsWith("{\"active\":true")) {
          Serial.println("=>push sended successfull!");

        } else {
          Serial.println("=>push send failed!");
          return;
        }
        //--------------------------------------------------------------------------------------------------------------------------

        order_push = deleteBeforeDelimiter(order_push, ",");
      }
    }
  }
}
*/
