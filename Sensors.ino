//===============================================================================================================================
//=========================================Модуль аналогового сенсора============================================================
void analog() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String start_value = sCmd.next();
  String end_value = sCmd.next();
  String start_value_out = sCmd.next();
  String end_value_out = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(optionJson, "analog_values", start_value + " " + end_value + " " + start_value_out + " " + end_value_out);

  createViget (viget_name, page_name, page_number, "analog");

  ts.add(ANALOG, analog_update_int, [&](void*) {

    static int analog_old;

    int analog_in = analogRead(A0);
    jsonWrite(configJson, "analog_in", analog_in);

    String analog_values = jsonRead(optionJson, "analog_values");

    int analog = map(analog_in, selectFromMarkerToMarker(analog_values, " ", 0).toInt(),
                     selectFromMarkerToMarker(analog_values, " ", 1).toInt(),
                     selectFromMarkerToMarker(analog_values, " ", 2).toInt(),
                     selectFromMarkerToMarker(analog_values, " ", 3).toInt());

    jsonWrite(configJson, "analog", analog);

    // if (analog_old != analog) {

    eventGen ("analog", "");
    sendSTATUS("analog", String(analog));
    Serial.println("sensor analog send date " + String(analog));

    // }
    analog_old = analog;
  }, nullptr, true);
}

//===============================================================================================================================
//=========================================Модуль аналогового сенсора============================================================
void ph() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String offset = sCmd.next();
  String page_number = sCmd.next();

  createViget (viget_name, page_name, page_number, "ph");
  jsonWrite(optionJson, "ph_offset", offset);


  ts.add(PH, ph_shooting_interval, [&](void*) {

    static float pHValue_old;
    static int counter;

    float offset = jsonRead(optionJson, "ph_offset").toFloat();

    int analog = analogRead(A0);
    analog = medianFilter.filtered(analog);
    float voltage = analog * 3.2 / 1024;
    float pHValue = 3.5 * voltage + offset;
    String pHValue_str = String(pHValue);

    pHValue_str = selectToMarkerPlus(pHValue_str, "." , 2);

    counter++;

    if (counter > ph_times_to_send) {
      counter = 0;

      jsonWrite(configJson, "ph", pHValue_str);

      //if (pHValue_old != pHValue) {

      eventGen ("ph", "");
      sendSTATUS("ph", pHValue_str);
      Serial.println("sensor ph send date " + pHValue_str);
      Serial.println("voltage " + String(voltage));

      //}
      pHValue_old = pHValue;
    }
  }, nullptr, true);
}
//===================================================================================================================================
//=========================================Модуль измерения уровня в баке============================================================
void level() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String empty_level = sCmd.next();
  String full_level = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(optionJson, "level_values", empty_level + " " + full_level);

  pinMode(14, OUTPUT);
  pinMode(12, INPUT);

  createViget (viget_name, page_name, page_number, "level");

  ts.add(LEVEL, tank_level_shooting_interval, [&](void*) {

    long duration_;
    int distance_cm;
    int level;
    static int level_old; //переменная static сохраняет свое значение между вызовами функции
    static int counter;

    digitalWrite(14, LOW);
    delayMicroseconds(2);
    digitalWrite(14, HIGH);
    delayMicroseconds(10);
    digitalWrite(14, LOW);
    duration_ = pulseIn(12, HIGH, 30000); // 3000 µs = 50cm // 30000 µs = 5 m
    distance_cm = duration_ / 29 / 2;
    distance_cm = medianFilter.filtered(distance_cm);//отсечение промахов медианным фильтром

    counter++;

    if (counter > tank_level_times_to_send) {
      counter = 0;
      jsonWrite(configJson, "level_in", distance_cm);

      String level_values = jsonRead(optionJson, "level_values");
      level = map(distance_cm, selectFromMarkerToMarker(level_values, " ", 0).toInt(), selectFromMarkerToMarker(level_values, " ", 1).toInt(), 0, 100);

      jsonWrite(configJson, "level", level);

      //if (level_old != level) {

      eventGen ("level", "");
      sendSTATUS("level", String(level));
      Serial.println("sensor tank level send date " + String(level));

      //}
      level_old = level;
    }
  }, nullptr, true);
}

//==========================================================================================================================================
//=========================================Модуль температурного сенсора ds18b20============================================================
void dallas() {

  static boolean flag = true;
  String pin = sCmd.next();
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  oneWire = new OneWire((uint8_t) pin.toInt());
  sensors.setOneWire(oneWire);
  sensors.begin();
  sensors.setResolution(12);

  createViget (viget_name, page_name, page_number, "dallas");

  ts.add(DALLAS, temp_update_int, [&](void*) {

    float temp = 0;
    static float temp_old;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);

    jsonWrite(configJson, "dallas", String(temp));

    //if (temp_old != temp) {

    eventGen ("dallas", "");
    sendSTATUS("dallas", String(temp));
    Serial.println("sensor dallas send date " + String(temp));

    //}
    temp_old = temp;
  }, nullptr, true);
}

//======================================================================================================================
//===============================================Создание виджета=======================================================
void createViget (String viget_name, String  page_name, String page_number, String topic) {

  static String viget;
  static boolean flag = true;

  viget_name.replace("#", " ");

  if (viget_name.indexOf("-text") >= 0) {
    viget_name.replace("-text", "");
    if (flag) {
      viget = readFile("viget.alertsm.json", 1024);
      flag = false;
    }
  }
  if (viget_name.indexOf("-gauge") >= 0) {
    viget_name.replace("-gauge", "");
    if (flag) {
      viget = readFile("viget.fillgauge.json", 1024);
      flag = false;
    }
  }
  if (viget_name.indexOf("-term") >= 0) {
    viget_name.replace("-term", "");
    if (flag) {
      viget = readFile("viget.termometr.json", 1024);
      flag = false;
    }
  }
  if (viget_name.indexOf("-chart") >= 0) {
    viget_name.replace("-chart", "");
    if (flag) {
      viget = readFile("viget.chart.json", 1024);
      flag = false;
    }
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/" + topic);
  all_vigets += viget + "\r\n";
}
//======================================================================================================================
//===============================================Логирование============================================================

void logging() {

  static boolean flag = true;

  String sensor_name = sCmd.next();
  String period_min = sCmd.next();
  String maxCount = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  if (sensor_name == "analog") jsonWrite(optionJson, "analog_logging_count", maxCount);
  if (sensor_name == "level") jsonWrite(optionJson, "level_logging_count", maxCount);
  if (sensor_name == "dallas") jsonWrite(optionJson, "dallas_logging_count", maxCount);
  if (sensor_name == "ph") jsonWrite(optionJson, "ph_logging_count", maxCount);

  static String viget;

  if (flag) {
    viget = readFile("viget.chart.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);

  if (sensor_name == "analog") jsonWrite(viget, "topic", prex + "/loganalog");
  if (sensor_name == "level") jsonWrite(viget, "topic", prex + "/loglevel");
  if (sensor_name == "dallas") jsonWrite(viget, "topic", prex + "/logdallas");
  if (sensor_name == "ph") jsonWrite(viget, "topic", prex + "/logph");

  all_vigets += viget + "\r\n";

  if (sensor_name == "analog") {
    flagLoggingAnalog = true;
    ts.remove(ANALOG_LOG);
    ts.add(ANALOG_LOG, period_min.toInt() * 1000 * 60, [&](void*) {
      deleteOldDate("log.analog.txt", jsonReadtoInt(optionJson, "analog_logging_count"), jsonRead(configJson, "analog"), false);
    }, nullptr, true);
  }

  if (sensor_name == "level") {
    flagLoggingLevel = true;
    ts.remove(LEVEL_LOG);
    ts.add(LEVEL_LOG, period_min.toInt() * 1000 * 60, [&](void*) {
      deleteOldDate("log.level.txt", jsonReadtoInt(optionJson, "level_logging_count"), jsonRead(configJson, "level"), false);
    }, nullptr, true);
  }

  if (sensor_name == "dallas") {
    flagLoggingDallas = true;
    ts.remove(DALLAS_LOG);
    ts.add(DALLAS_LOG, period_min.toInt() * 1000 * 60, [&](void*) {
      deleteOldDate("log.dallas.txt", jsonReadtoInt(optionJson, "dallas_logging_count"), jsonRead(configJson, "dallas"), false);
    }, nullptr, true);
  }

  if (sensor_name == "ph") {
    flagLoggingPh = true;
    ts.remove(PH_LOG);
    ts.add(PH_LOG, period_min.toInt() * 1000 * 60, [&](void*) {
      deleteOldDate("log.ph.txt", jsonReadtoInt(optionJson, "ph_logging_count"), jsonRead(configJson, "ph"), false);
    }, nullptr, true);
  }
}

void deleteOldDate(String file, int seted_number_of_lines, String date_to_add, boolean date_time) {

  String current_time;

  if (date_time) {
    current_time = GetDataDigital() + " " + GetTimeWOsec();
    current_time.replace(".", "");
    current_time.replace(":", "");
  } else {
    current_time = "";
  }

  String log_date = readFile(file, 5000);

  //предел количества строк 255

  log_date.replace("\r\n", "\n");
  log_date.replace("\r", "\n");

  int current_number_of_lines = count(log_date, "\n");
  Serial.println("->in log file " + file + " " + current_number_of_lines + " lines");

  if (current_number_of_lines > seted_number_of_lines + 1) {
    SPIFFS.remove("/" + file);
    current_number_of_lines = 0;
  }
  if (current_number_of_lines == 0) {
    SPIFFS.remove("/" + file);
    current_number_of_lines = 0;
  }
  if (current_number_of_lines > seted_number_of_lines) {
    log_date = deleteBeforeDelimiter(log_date, "\n");
    log_date += current_time + " " +  date_to_add + "\n";
    writeFile(file, log_date);

  } else {
    if (date_time) {
      addFile(file, current_time + " " +  date_to_add);
    } else {
      addFile(file, date_to_add);
    }
  }
}
