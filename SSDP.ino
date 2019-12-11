void SSDP_init(void) {
  String chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  if (WiFi.status() == WL_CONNECTED) {
    // SSDP дескриптор
    HTTP.on("/description.xml", HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });
    // --------------------Получаем SSDP со страницы
    HTTP.on("/ssdp", HTTP_GET, []() {
      String ssdp = HTTP.arg(ssdpS);
      configJson = jsonWrite(configJson, ssdpS, ssdp);
      configSetup = jsonWrite(configSetup, ssdpS, ssdp);
      SSDP.setName(jsonRead(configSetup, ssdpS));
      saveConfig();                 // Функция сохранения данных во Flash
      HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
    });
    //Если версия  2.0.0 закаментируйте следующую строчку
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName(jsonRead(configSetup, ssdpS));
    SSDP.setSerialNumber(chipID);
    SSDP.setURL("/");
    SSDP.setModelName("tech");
    SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, ssdpS));


    SSDP.setModelURL("https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware");
    SSDP.setManufacturer("Borisenko Dmitry");
    SSDP.setManufacturerURL("https://www.instagram.com/rriissee3");
    SSDP.begin();
  }
}
