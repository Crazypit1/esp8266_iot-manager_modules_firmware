void CMD_init() {


  sCmd.addCommand("button",  button);
  sCmd.addCommand("buttonSet",  buttonSet);

  sCmd.addCommand("pwm",  pwm);
  sCmd.addCommand("pwmSet",  pwmSet);

  sCmd.addCommand("switch",  switch_);

  sCmd.addCommand("analog",  analog);
  sCmd.addCommand("level",  level);
  sCmd.addCommand("dallas",  dallas);

  sCmd.addCommand("logging",  logging);

  sCmd.addCommand("input",  input);
  sCmd.addCommand("valueUpSet",  valueUpSet);
  sCmd.addCommand("valueDownSet",  valueDownSet);

  sCmd.addCommand("text",  text);
  sCmd.addCommand("textSet",  textSet);

  sCmd.addCommand("timer",  timer);
  sCmd.addCommand("timerStart",  timerStart);
  sCmd.addCommand("timerStop",  timerStop);

  sCmd.addCommand("mqtt",  mqttOrderSend);
  sCmd.addCommand("http",  httpOrderSend);
  sCmd.addCommand("push",  pushControl);

  handleCMD_ticker();
}

//==========================================================================================================
//==========================================Модуль кнопок===================================================
void button() {

  static boolean flag = true;
  String button_number = sCmd.next();
  String button_pin = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String start_state = sCmd.next();
  String page_number = sCmd.next();

  if (button_pin != "na") {
    pinMode(button_pin.toInt(), OUTPUT);
    digitalWrite(button_pin.toInt(), start_state.toInt());
    uint8_t button_pin_int = button_pin.toInt();
  }

  jsonWrite(optionJson, "button_pin" + button_number, button_pin);
  jsonWrite(configJson, "buttonSet" + button_number, start_state);
  static String viget;

  if (flag) {
    viget = readFile("viget.toggle.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/buttonSet" + button_number);
  all_vigets += viget + "\r\n";
}

void buttonSet() {

  String button_number = sCmd.next();
  String button_state = sCmd.next();
  String button_pin = jsonRead(optionJson, "button_pin" + button_number);

  if (button_pin != "na") {
    digitalWrite(button_pin.toInt(), button_state.toInt());
  }

  String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
  jsonWrite(optionJson, "scenario_status", tmp + "buttonSet" + button_number + ",");

  jsonWrite(configJson, "buttonSet" + button_number, button_state);
  sendSTATUS("buttonSet" + button_number, button_state);
}

//==================================================================================================================
//==========================================Модуль управления ШИМ===================================================
void pwm() {

  static boolean flag = true;
  String pwm_number = sCmd.next();
  String pwm_pin = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String start_state = sCmd.next();
  String page_number = sCmd.next();


  uint8_t pwm_pin_int = pwm_pin.toInt();
  jsonWrite(optionJson, "pwm_pin" + pwm_number, pwm_pin);
  pinMode(pwm_pin_int, INPUT);
  analogWrite(pwm_pin_int, start_state.toInt());
  jsonWrite(configJson, "pwmSet" + pwm_number, start_state);

  static String viget;
  if (flag) {
    viget = readFile("viget.range.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/pwmSet" + pwm_number);
  all_vigets += viget + "\r\n";
}

void pwmSet() {

  String pwm_number = sCmd.next();
  String pwm_state = sCmd.next();
  int pwm_state_int = pwm_state.toInt();

  int pin = jsonReadtoInt(optionJson, "pwm_pin" + pwm_number);
  analogWrite(pin, pwm_state_int);

  String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
  jsonWrite(optionJson, "scenario_status", tmp + "pwmSet" + pwm_number + ",");

  jsonWrite(configJson, "pwmSet" + pwm_number, pwm_state);
  sendSTATUS("pwmSet" + pwm_number, pwm_state);
}
//==================================================================================================================
//==========================================Модуль физической кнопки================================================
void switch_ () {

  String switch_number = sCmd.next();
  String switch_pin = sCmd.next();
  String switch_delay = sCmd.next();

  buttons[switch_number.toInt()].attach(switch_pin.toInt());
  buttons[switch_number.toInt()].interval(switch_delay.toInt());
  but[switch_number.toInt()] = true;


}

void handleButton()  {

  static uint8_t switch_number = 1;

  if (but[switch_number]) {
    buttons[switch_number].update();
    if (buttons[switch_number].fell()) {
      String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
      jsonWrite(optionJson, "scenario_status", tmp + "switchSet" + String(switch_number) + ",");
      jsonWrite(configJson, "switchSet" + String(switch_number), "1");
    }
    if (buttons[switch_number].rose()) {
      String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
      jsonWrite(optionJson, "scenario_status", tmp + "switchSet" + String(switch_number) + ",");
      jsonWrite(configJson, "switchSet" + String(switch_number), "0");
    }
  }
  switch_number++;
  if (switch_number == NUM_BUTTONS) switch_number = 0;
}

//===============================================================================================================================
//=========================================Модуль аналогового сенсора============================================================
void analog() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String start_value = sCmd.next();
  String end_value = sCmd.next();
  String start_value_out = sCmd.next();
  String end_value_out = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(optionJson, "analog_values", start_value + " " + end_value + " " + start_value_out + " " + end_value_out);

  static String viget;
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

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/analog");
  all_vigets += viget + "\r\n";

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

    if (analog_old != analog) {

      String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
      jsonWrite(optionJson, "scenario_status", tmp + "analog,");

      sendSTATUS("analog", String(analog));
      Serial.println("sensor analog send date " + String(analog));
    }

    analog_old = analog;

  }, nullptr, true);
}
//===================================================================================================================================
//=========================================Модуль измерения уровня в баке============================================================
void level() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String empty_level = sCmd.next();
  String full_level = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(optionJson, "level_values", empty_level + " " + full_level);

  pinMode(14, OUTPUT);
  pinMode(12, INPUT);

  static String viget;
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

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/level");
  all_vigets += viget + "\r\n";

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
    distance_cm = testFilter.filtered(distance_cm);//отсечение промахов медианным фильтром

    counter++;

    if (counter > tank_level_times_to_send) {
      counter = 0;
      jsonWrite(configJson, "level_in", distance_cm);

      String level_values = jsonRead(optionJson, "level_values");
      level = map(distance_cm, selectFromMarkerToMarker(level_values, " ", 0).toInt(), selectFromMarkerToMarker(level_values, " ", 1).toInt(), 0, 100);
      jsonWrite(configJson, "level", level);

      if (level_old != level) {

        String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
        jsonWrite(optionJson, "scenario_status", tmp + "level,");

        sendSTATUS("level", String(level));
        Serial.println("sensor tank level send date " + String(level));

      }
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
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  oneWire = new OneWire((uint8_t) pin.toInt());
  sensors.setOneWire(oneWire);
  sensors.begin();
  sensors.setResolution(12);


  static String viget;
  if (viget_name.indexOf("-chart") >= 0) {
    viget_name.replace("-chart", "");
    if (flag) {
      viget = readFile("viget.chart.json", 1024);
      flag = false;
    }
  }
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

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/dallas");
  all_vigets += viget + "\r\n";

  ts.add(DALLAS, temp_update_int, [&](void*) {

    float temp = 0;
    static float temp_old;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    jsonWrite(configJson, "dallas", String(temp));

    if (temp_old != temp) {

      String tmp = jsonRead(optionJson, "scenario_status") ;    //генерирование события
      jsonWrite(optionJson, "scenario_status", tmp + "dallas,");

      sendSTATUS("dallas", String(temp));
      Serial.println("sensor ds18b20 send date " + String(temp));

    }

    temp_old = temp;

  }, nullptr, true);
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
    addFile(file, current_time + " " +  date_to_add);
  }
}

//=====================================================================================================================================
//=========================================Добавление окна ввода переменной============================================================
void input() {

  String name_ = sCmd.next();
  String number = name_.substring(5);
  String start_value = sCmd.next();
  String step_ = sCmd.next();
  String value_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  static boolean flag1 = true;
  static String viget1;
  if (flag1) {
    viget1 = readFile("viget.buttondown.json", 1024);
    flag1 = false;
  }
  jsonWrite(viget1, "page", page_name);
  jsonWrite(viget1, "pageId", page_number);
  jsonWrite(viget1, "topic", prex + "/valueDownSet" + number);
  all_vigets += viget1 + "\r\n";

  static boolean flag2 = true;
  static String viget2;
  if (flag2) {
    viget2 = readFile("viget.alertbg.json", 1024);
    flag2 = false;
  }
  jsonWrite(viget2, "page", page_name);
  page_number = String(page_number.toInt() + 1);
  jsonWrite(viget2, "pageId", page_number);
  value_name.replace("#", " ");
  if (value_name != "nil") jsonWrite(viget2, "descr", value_name);
  jsonWrite(viget2, "topic", prex + "/" + name_);
  sendSTATUS(name_, start_value);
  jsonWrite(configJson, name_, start_value);
  jsonWrite(configJson, name_ + "step", step_);
  all_vigets += viget2 + "\r\n";

  static boolean flag3 = true;
  static String viget3;
  if (flag3) {
    viget3 = readFile("viget.buttonup.json", 1024);
    flag3 = false;
  }
  jsonWrite(viget3, "page", page_name);
  page_number = String(page_number.toInt() + 1);
  jsonWrite(viget3, "pageId", page_number);
  jsonWrite(viget3, "topic", prex + "/valueUpSet" + number);
  all_vigets += viget3 + "\r\n";
}

void valueUpSet() {
  String number = sCmd.next();
  float val = jsonRead(configJson, "value" + number).toFloat();
  float step_ = jsonRead(configJson, "value" + number + "step").toFloat();
  val = val + step_;
  String val_str = String(val);
  val_str = selectToMarkerPlus (val_str, ".", 2);
  jsonWrite(configJson, "value" + number, val_str);
  sendSTATUS("value" + number, val_str);
}

void valueDownSet() {
  String number = sCmd.next();
  float val = jsonRead(configJson, "value" + number).toFloat();
  float step_ = jsonRead(configJson, "value" + number + "step").toFloat();
  val = val - step_;
  String val_str = String(val);
  val_str = selectToMarkerPlus (val_str, ".", 2);
  jsonWrite(configJson, "value" + number, val_str);
  sendSTATUS("value" + number, val_str);
}

//=====================================================================================================================================
//=========================================Добавление текстового виджета============================================================
void text() {

  String number = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  static boolean flag = true;
  static String viget;
  if (flag) {
    viget = readFile("viget.alertsm.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  if (viget_name != "nil") jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/textSet" + number);
  all_vigets += viget + "\r\n";
}


void textSet() {

  String number = sCmd.next();
  String text = sCmd.next();
  text.replace("_", " ");

  if (text.indexOf("-time") >= 0) {
    text.replace("-time", "");
    String time = GetTime();
    time.replace(":", ".");
    text = GetDataDigital() + " " + time + " " + text;
  }

  if (text == "countdown-1") {

  }
  if (text == "countdown-2") {

  }

  jsonWrite(configJson, "textSet" + number, text);
  sendSTATUS("textSet" + number, text);

}

//=================================================команды удаленного управления===========================================================

void mqttOrderSend() {   //mqtt 9139530-1458400 rel#1#1

  String id = sCmd.next();
  String order = sCmd.next();

  String topik = selectFromMarkerToMarker(order, "#", 0) + selectFromMarkerToMarker(order, "#", 1);
  String state = selectFromMarkerToMarker(order, "#", 2);

  String  all_line = prefix + "/" + id + "/" + topik + "/control";
  int send_status = client.publish (all_line.c_str(), state.c_str(), false);
}

void httpOrderSend() {

  String ip = sCmd.next();
  String order = sCmd.next();
  order.replace("#", "%20");
  String url = "http://" + ip + "/cmd?command=" + order;
  getURL(url);
}

//=========================================Таймера=================================================================

void timer() {

  String seted_time = sCmd.next();
  String order = sCmd.next();
  order.replace("_", " ");
  // Serial.println(seted_time);
  if (seted_time == current_time) {

    order_loop += order + ",";

  }
}

void timerStart() {

  String number = sCmd.next();
  String period_of_time = sCmd.next();
  String type = sCmd.next();
  String order = sCmd.next();

  flagTimer1 = true;
  flagTimer2 = true;

  if (period_of_time.indexOf("value") >= 0) period_of_time = jsonReadtoInt(configJson, "value" + number);
  jsonWrite(optionJson, "timerDate" + number, order + " " + period_of_time + " " + type);

  if (number == "1") {
    ts.add(21, 1000, [&](void*) {
      static String timerDate;
      static String order;
      static String period_of_time;
      static int period_of_time_int;
      static String type;
      static int type_int;
      if (flagTimer1) {
        timerDate = jsonRead(optionJson, "timerDate1");
        order = selectFromMarkerToMarker(timerDate, " ", 0);
        order.replace("_", " ");
        period_of_time = selectFromMarkerToMarker(timerDate, " ", 1);
        period_of_time_int = period_of_time.toInt();
        type = selectFromMarkerToMarker(timerDate, " ", 2);
        type_int = type.toInt();
        period_of_time_int = period_of_time_int * type_int;
      }
      flagTimer1 = false;
      period_of_time_int--;
      Serial.println(period_of_time_int);
      jsonWrite(optionJson, "period1", period_of_time_int);
      if (period_of_time_int <= 0) {
        order_loop += order + ",";
        Serial.println("done1");
        ts.remove(21);
      }
    }, nullptr, true);
  }

  if (number == "2") {
    ts.add(22, 1000, [&](void*) {
      static String timerDate;
      static String order;
      static String period_of_time;
      static int period_of_time_int;
      static String type;
      static int type_int;
      if (flagTimer2) {
        timerDate = jsonRead(optionJson, "timerDate2");
        order = selectFromMarkerToMarker(timerDate, " ", 0);
        order.replace("_", " ");
        period_of_time = selectFromMarkerToMarker(timerDate, " ", 1);
        period_of_time_int = period_of_time.toInt();
        type = selectFromMarkerToMarker(timerDate, " ", 2);
        type_int = type.toInt();
        period_of_time_int = period_of_time_int * type_int;
      }
      flagTimer2 = false;
      period_of_time_int--;
      Serial.println(period_of_time_int);
      jsonWrite(optionJson, "period2", period_of_time_int);
      if (period_of_time_int <= 0) {
        order_loop += order + ",";
        Serial.println("done2");
        ts.remove(22);
      }
    }, nullptr, true);
  }
}

void timerStop() {

  String number = sCmd.next();
  ts.remove(number.toInt() + 20);

}


//======================выполнение команд (в лупе) по очереди из строки order=======================================
void handleCMD_loop() {

  if (order_loop != "") {

    String tmp = selectToMarker(order_loop, ",");                //выделяем из страки order первую команду rel 5 1,
    if (tmp != "no_command") sCmd.readStr(tmp);                                           //выполняем первую команду
    Serial.println("order_loop => " + order_loop);
    order_loop = deleteBeforeDelimiter(order_loop, ",");         //осекаем выполненную команду
  }
}
//======================выполнение команд (через период) по очереди из строки order=======================================
void handleCMD_ticker() {

  ts.add(CMD, CMD_update_int, [&](void*) {
    if (!busy) {
      if (order_ticker != "") {

        String tmp = selectToMarker(order_ticker, ",");                //выделяем из страки order первую команду pus title body
        if (tmp != "no_command") sCmd.readStr(tmp);                                             //выполняем первую команду
        Serial.println("order_ticker => " + order_ticker);
        order_ticker = deleteBeforeDelimiter(order_ticker, ",");       //осекаем выполненную команду
      }
    }
  }, nullptr, true);
}




//============разное

/*
  void delAlert() {

  String alert_id = sCmd.next();
  delViget(alert_id);
  sendAllWigets();
  }


  void delViget(String text_in_viget) {
  String viget = all_vigets;
  while (viget.length() != 0) {
    String tmp = selectToMarkerPlus (viget, "\r\n", 2);
    if (tmp.indexOf(text_in_viget) > 0) {

      all_vigets.replace(tmp, "");
      //Serial.println(all_vigets);

      viget = deleteBeforeDelimiter(viget, "\r\n");
    } else {
      viget = deleteBeforeDelimiter(viget, "\r\n");
    }
  }
  }
*/
