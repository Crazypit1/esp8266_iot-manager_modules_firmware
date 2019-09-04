void buttons_init() {
  HTTP.on("/all_modules_init", HTTP_GET, []() {

    Modules_init();

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
    //timers = readFile("timers.all.txt", 1024);
    Timers_init();
    HTTP.send(200, "text/plain", "OK");
  });

  Modules_init();
  Scenario_init();
  Timers_init();

}

void Modules_init() {

  ts.remove(LEVEL);
  ts.remove(ANALOG);
  ts.remove(DS18B20);

  all_vigets = "";
  txtExecution("config.all.txt");
  outcoming_date();
}

void Scenario_init() {
  //-------------------------------сценарии-----------------------------------------------------
  if (jsonRead(configSetup, "scenario") == "1") {
    Serial.println("scenario: ON");
    scenario = readFile("scenario.all.txt", 1024);
    ts.add(SCENARIO, scenario_update_int, [&](void*) {
      if (client.connected()) {
        stringExecution(scenario);
        Serial.println("scenario send date ");
      }
    }, nullptr, true);
  } else {
    Serial.println("scenario: OFF");
    ts.remove(SCENARIO);
  }
}

void Timers_init() {
  //------------------------------таймеры------------------------------------------------------
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


//=======================================================================================================================================

void txtExecution(String file) {

  String command_all = readFile(file, 2048) + "\r\n";  //2048

  command_all.replace("\r\n", "\n");
  command_all.replace("\r", "\n");

  while (command_all.length() != 0) {

    String tmp = selectToMarker (command_all, "\n");
    //if (tmp.indexOf("//") < 0)
    sCmd.readStr(tmp);
    command_all = deleteBeforeDelimiter(command_all, "\n");
  }
}
void stringExecution(String str) {

  String command_all = str + "\r\n";  //"\r\n"

  command_all.replace("\r\n", "\n");
  command_all.replace("\r", "\n");

  while (command_all.length() != 0) {

    String tmp = selectToMarker (command_all, "\n");
    //if (tmp.indexOf("//") < 0)
    sCmd.readStr(tmp);
    command_all = deleteBeforeDelimiter(command_all, "\n");
  }
}
