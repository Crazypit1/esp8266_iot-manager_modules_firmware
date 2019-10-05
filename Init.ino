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

  HTTP.on("/timers", HTTP_GET, []() {
    jsonWrite(configSetup, "timers", HTTP.arg("status"));
    saveConfig();
    Timer_time_init();
    HTTP.send(200, "text/plain", "OK");
  });

  HTTP.on("/cleanlog", HTTP_GET, []() {

    SPIFFS.remove("/log.analog.txt");
    SPIFFS.remove("/log.dallas.txt");
    SPIFFS.remove("/log.level.txt");
    SPIFFS.remove("/log.txt");

    HTTP.send(200, "text/plain", "OK");
  });

  Device_init();
  Scenario_init();
  Timer_time_init();
  Timer_countdown_init();
}

void Device_init() {

  ts.remove(LEVEL);
  ts.remove(ANALOG);
  ts.remove(DALLAS);

  all_vigets = "";
  txtExecution("config.all.txt");
  outcoming_date();
}
//-------------------------------сценарии-----------------------------------------------------

void Scenario_init() {
  if (jsonRead(configSetup, "scenario") == "1") {
    scenario = readFile("scenario.all.txt", 2048);
  }
}
//------------------------------таймеры------------------------------------------------------
void Timer_time_init() {
  if (jsonRead(configSetup, "timers") == "1") {
    timers = readFile("timers.all.txt", 1024);
    ts.add(TIMERS, 1000, [&](void*) {
      current_time = GetTime();
      Serial.println(current_time);
      static boolean flag = false;
      if (flag) stringExecution(timers);
      flag = true;
    }, nullptr, true);
  } else {
    ts.remove(TIMERS);
  }
}
