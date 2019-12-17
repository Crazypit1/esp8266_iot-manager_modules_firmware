void initUpgrade() {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);

  HTTP.on("/upgradeSave", HTTP_GET, []() {
    webUpgrade(true);
  });

  HTTP.on("/upgradeDefault", HTTP_GET, []() {
    webUpgrade(false);
  });
}
// ----------------------- Обновление с сайта
void webUpgrade(boolean save_settings) {
  String spiffsData = "http://91.204.228.124:1002/update/esp8266_iot-manager_modules_firmware.spiffs.bin"; // Получим путь к файловой системе
  String buildData = "http://91.204.228.124:1002/update/esp8266_iot-manager_modules_firmware.ino.generic.bin";  // Получим путь к файлу прошивки
  if (spiffsData != "") { // Если нужно прошить FS
    String scenario_for_update;
    String config_for_update;
    String configSetup_for_update;
    Serial.println(spiffsData);
    if (save_settings) {
      scenario_for_update = readFile(scenarioFileNameS, 2048);
      config_for_update = readFile("config.all.txt", 2048);
      configSetup_for_update = configSetup;
    }
    ESPhttpUpdate.rebootOnUpdate(false); // Отключим перезагрузку после обновления
    updateHTTP(spiffsData, true);
    if (save_settings) {
      writeFile(scenarioFileNameS, scenario_for_update);
      writeFile("config.all.txt", config_for_update);
      writeFile("config.json", configSetup_for_update);
      saveConfig();
    }
  }
  if (buildData != "") { // Если нужно прошить build
    Serial.println(buildData);
    ESPhttpUpdate.rebootOnUpdate(true); // Включим перезагрузку после обновления
    updateHTTP(buildData, false);
  }
}
// ------------------ Обновление по url
void updateHTTP(String url, boolean mode) {
  if (url == "") return;
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  if (mode) {
    Serial.println("Update Spiffs...");
    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(url);
    UpdateStatus(ret , "Spiffs");
  } else {
    Serial.println("Update Build...");
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    UpdateStatus(ret , "build");
  }
}
void UpdateStatus(t_httpUpdate_return set, String mode) {

  switch (set) {

    case HTTP_UPDATE_FAILED:
      Serial.println(mode + "_FAILED");
      var = "{}";
      jsonWrite(var, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>_FAILED");
      jsonWrite(var, "class", "pop-up");
      HTTP.send(200, "application/json", var);
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println(mode + "_NO_UPDATES");
      var = "{}";
      jsonWrite(var, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>_NO_UPDATES");
      jsonWrite(var, "class", "pop-up");
      HTTP.send(200, "application/json", var);
      break;

    case HTTP_UPDATE_OK:
      Serial.println(mode + "_UPDATE_OK");
      var = "{}";
      jsonWrite(var, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>_UPDATE_OK");
      jsonWrite(var, "class", "pop-up");
      HTTP.send(200, "application/json", var);
      break;
  }
}
