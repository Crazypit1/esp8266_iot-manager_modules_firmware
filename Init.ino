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
  outcoming_date();
}
//-------------------------------сценарии-----------------------------------------------------

void Scenario_init() {
  if (jsonRead(configSetup, "scenario") == "1") {
    scenario = readFile("scenario.all.txt", 2048);
  }
}
//------------------------------таймеры------------------------------------------------------
void time() {

  String time_number = sCmd.next();
  String time = sCmd.next();

  String time_to_add = time_number + "-" + time;

  String replace_line = jsonRead(optionJson, "times") ;
  int psn1 = replace_line.indexOf(time_number + "-") ;  //ищем позицию времени которое надо заменить

  if (psn1 != -1) {                              //если оно есть

    int psn2 = replace_line.indexOf(",", psn1);           //от этой позиции находим позицию запятой

    String timer = replace_line.substring(psn1, psn2);    //выделяем таймер который надо заменить
    ///tmp.replace(timer, new_timer);                     //заменяем таймер на новый (во всей стороке)
    replace_line.replace(timer + ",", "");
    replace_line += time_to_add + ",";

  } else {                                              //если его нет
    replace_line += time_to_add + ",";
  }


  jsonWrite(optionJson, "times", replace_line);

  jsonWrite(configJson, "timeSet" + time_number, "1");

  ts.add(TIMERS, 1000, [&](void*) {

    current_time = GetTime();
    Serial.println(current_time);

    String seted_times = jsonRead(optionJson, "times");

    while (seted_times.length() != 0) {
      String tmp = selectToMarker (seted_times, ",");

      String time_number = selectToMarker(tmp, "-");
      String seted_time = deleteBeforeDelimiter(tmp, "-");

      Serial.println(seted_time);

      if (current_time == seted_time) {
        jsonWrite(configJson, "timeSet" + time_number, "0");
        eventGen ("timeSet", time_number);
      }

      seted_times = deleteBeforeDelimiter(seted_times, ",");
    }
  }, nullptr, true);
}
