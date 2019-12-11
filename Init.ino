void All_init() {

  HTTP.on("/all_modules_init", HTTP_GET, []() {

    by_button = true;
    Device_init();
    by_button = false;

    HTTP.send(200, "text/plain", "OK");
  });

  HTTP.on("/scenario", HTTP_GET, []() {
    jsonWrite(configSetup, "scenario", HTTP.arg("status"));
    saveConfig();
    Scenario_init();
    HTTP.send(200, "text/plain", "OK");
  });

  /*  HTTP.on("/timers", HTTP_GET, []() {
      jsonWrite(configSetup, "timers", HTTP.arg("status"));
      saveConfig();
      Timer_time_init();
      HTTP.send(200, "text/plain", "OK");
    });*/

  HTTP.on("/cleanlog", HTTP_GET, []() {

    SPIFFS.remove("/log.analog.txt");
    SPIFFS.remove("/log.dallas.txt");
    SPIFFS.remove("/log.level.txt");
    SPIFFS.remove("/log.ph.txt");
    SPIFFS.remove("/log.txt");

    HTTP.send(200, "text/plain", "OK");
  });

  Device_init();
  Scenario_init();
  Timer_countdown_init();
}

void Device_init() {

  ts.remove(LEVEL);
  ts.remove(ANALOG);
  ts.remove(PH);
  ts.remove(DALLAS);

  all_vigets = "";
  txtExecution("config.all.txt");
  static boolean flag = false;
  if (flag) outcoming_date();
  flag = true;
}
//-------------------------------сценарии-----------------------------------------------------

void Scenario_init() {
  if (jsonRead(configSetup, "scenario") == "1") {
    scenario = readFile(scenarioFileNameS, 2048);
  }
}
