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
    Timers_init();
    HTTP.send(200, "text/plain", "OK");
  });

  HTTP.on("/cleanlog", HTTP_GET, []() {

    SPIFFS.remove("/log.analog.txt");
    SPIFFS.remove("/log.dallas.txt");
    SPIFFS.remove("/log.level.txt");
    SPIFFS.remove("/log.txt");

    HTTP.send(200, "text/plain", "OK");
  });

  Modules_init();
  Scenario_init();
  Timers_init();

}

void Modules_init() {

  ts.remove(LEVEL);
  ts.remove(ANALOG);
  ts.remove(DALLAS);

  all_vigets = "";
  txtExecution("config.all.txt");
  outcoming_date();
}

void Scenario_init() {
  //-------------------------------сценарии-----------------------------------------------------
  if (jsonRead(configSetup, "scenario") == "1") {
    scenario = readFile("scenario.all.txt", 2048);
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

void handleScenario() {

  if (jsonRead(optionJson, "scenario_status") != "") {             //читаем файл в который записываются события

    String str = scenario;                                          //читаем файл с сценариями
    str += "\n";

    str.replace("\r\n", "\n");
    str.replace("\r", "\n");

    while (str.length() != 0) {

      String tmp = selectToMarker (str, "end");                    //выделяем первый сценарий из файла вместе с командами

      if (tmp == "") return;

      String condition = selectToMarker (tmp, "\n");               //выделяем первую строку самого сценария

      String param_name = selectFromMarkerToMarker(condition, " " , 0);  //из первой страки берем имя параметра


      String num1 = param_name.substring(param_name.length() - 1);
      String num2 = param_name.substring(param_name.length() - 2, param_name.length() - 1);
      if (isDigitStr(num1) && isDigitStr(num2)) {
        param_name = param_name.substring(0, param_name.length() - 2) + "Set" + num2 + num1;  
      } else {
        if (isDigitStr(num1)) {
          param_name = param_name.substring(0, param_name.length() - 1) + "Set" + num1;
        }
      }

      String order = jsonRead(optionJson, "scenario_status");
      String param = selectToMarker (order, ",");                         //читаем первое имя поступившего параметра из файла событий

      if (param_name == param) {                                          //если поступившее событие равно событию заданному в файле начинаем его обработку

        String sign = selectFromMarkerToMarker(condition, " " , 1);
        String value = selectFromMarkerToMarker(condition, " " , 2);
        boolean flag = false;                                              //если одно из значений совпало то только тогда начинаем выполнять комнады
        if (sign == "=") {
          if (jsonRead(configJson, param_name) == value) flag = true;
        }
        if (sign == "!=") {
          if (jsonRead(configJson, param_name) != value) flag = true;
        }
        if (sign == "<") {
          if (jsonRead(configJson, param_name).toFloat() < value.toFloat()) flag = true;
        }
        if (sign == ">") {
          if (jsonRead(configJson, param_name).toFloat() > value.toFloat()) flag = true;
        }
        if (sign == ">=") {
          if (jsonRead(configJson, param_name).toFloat() >= value.toFloat()) flag = true;
        }
        if (sign == "<=") {
          if (jsonRead(configJson, param_name).toFloat() <= value.toFloat()) flag = true;
        }

        if (flag) {

          tmp = deleteBeforeDelimiter(tmp, "\n");                                           //удаляем строку самого сценария оставляя только команды
          stringExecution(tmp);                                                             //выполняем все команды

        }
      }
      str = deleteBeforeDelimiter(str, "end\n");                                            //удаляем первый сценарий
    }
    String tmp2 = jsonRead(optionJson, "scenario_status");                                  //читаем файл событий
    tmp2 = deleteBeforeDelimiter(tmp2, ",");                                                //удаляем выполненное событие
    jsonWrite(optionJson, "scenario_status", tmp2);                                         //записываем обновленный файл событий
  }
}
