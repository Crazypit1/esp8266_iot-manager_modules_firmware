
void WebSoket_init () {

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("web Socket disconnected");
      break;
    case WStype_CONNECTED:
      {


        // send message to client
        webSocket.sendTXT(num, configJson);
        jsonWrite (configJson, "soket", num);
        Serial.println("web Socket connected");
      }
      break;
    case WStype_TEXT:
      //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      //USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}
