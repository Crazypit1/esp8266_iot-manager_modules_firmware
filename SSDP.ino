void SSDP_init(void) {
  String chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  if (WiFi.status() == WL_CONNECTED) {
    // SSDP дескриптор
    HTTP.on("/description.xml", HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });
    // --------------------Получаем SSDP со страницы
    HTTP.on("/ssdp", HTTP_GET, []() {
      String ssdp = HTTP.arg("ssdp");
      configJson = jsonWrite(configJson, "SSDP", ssdp);
      configSetup = jsonWrite(configSetup, "SSDP", ssdp);
      SSDP.setName(jsonRead(configSetup, "SSDP"));
      saveConfig();                 // Функция сохранения данных во Flash
      HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
    });
    //Если версия  2.0.0 закаментируйте следующую строчку
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName(jsonRead(configSetup, "SSDP"));
    SSDP.setSerialNumber(chipID);
    SSDP.setURL("/");
    SSDP.setModelName("tech");
    SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, "SSDP"));


    SSDP.setModelURL("http://esp8266-arduinoide.ru/step12-graf-dht/");
    SSDP.setManufacturer("Tretyakov Sergey");
    SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
    SSDP.begin();
  }
}
