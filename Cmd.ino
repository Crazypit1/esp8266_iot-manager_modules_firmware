void CMD_init() {

  sCmd.addCommand("RELAY",  relayInit);
  sCmd.addCommand("rel",  relayControl);

  sCmd.addCommand("PWM",  pwmInit);
  sCmd.addCommand("pwm",  pwmControl);

  sCmd.addCommand("SWITCH",  switch_Init);
  sCmd.addCommand("swi",  switchControl);

  sCmd.addCommand("LEVEL",  tank_levelInit);

  sCmd.addCommand("ANALOG",  analogInit);

  sCmd.addCommand("TEMP_ds18b20",  ds18b20Init);

  sCmd.addCommand("mqtt",  mqttOrderSend);
  sCmd.addCommand("http",  httpOrderSend);
  sCmd.addCommand("push",  pushControl);

  sCmd.addCommand("addViget",  addViget);
  sCmd.addCommand("fillViget",  fillViget);

  sCmd.addCommand("setValue",  setValue_);
  sCmd.addCommand("Bup",  Bup);
  sCmd.addCommand("Bdw",  Bdw);


  sCmd.addCommand("SCENARIO",  handleScenario);
  sCmd.addCommand("TIMER",  handleTimers);

}

//==========================================Модуль управления реле===================================================
void relayInit() {

  static boolean flag = true;
  String relay_number = sCmd.next();
  String relay_pin = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String start_state = sCmd.next();
  String page_number = sCmd.next();


  uint8_t relay_pin_int = relay_pin.toInt();
  jsonWrite(optionJson, "relay_pin" + relay_number, relay_pin);
  pinMode(relay_pin_int, OUTPUT);
  digitalWrite(relay_pin_int, start_state.toInt());
  jsonWrite(configJson, "rel" + relay_number, start_state);

  static String viget;
  if (flag) {
    viget = readFile("viget.toggle.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/rel" + relay_number);
  all_vigets += viget + "\r\n";
}

void relayControl() {

  String relay_number = sCmd.next();
  String relay_state = sCmd.next();
  int relay_state_int = relay_state.toInt();

  int pin = jsonReadtoInt(optionJson, "relay_pin" + relay_number);
  digitalWrite(pin, relay_state_int);

  //send_push("Дом", "Реле" + relay_number);

  jsonWrite(configJson, "rel" + relay_number, relay_state);
  sendSTATUS("rel" + relay_number, relay_state);
}
//=========================================Модуль физической кнопки(2 команды)================================================================
void switch_Init() {

  static boolean flag = true;

  String switch_number = sCmd.next();
  String switch_pin = sCmd.next();
  String switch_delay = sCmd.next();

  String on_off_1 = sCmd.next();
  String order_cmd_1 = sCmd.next();

  String on_off_2 = sCmd.next();
  String order_cmd_2 = sCmd.next();

  jsonWrite(optionJson, "switch" + switch_number, on_off_1 + " " + order_cmd_1 + " " + on_off_2 + " " + order_cmd_2);

  buttons[switch_number.toInt()].attach(switch_pin.toInt());
  buttons[switch_number.toInt()].interval(switch_delay.toInt());
  but[switch_number.toInt()] = true;

}

void switchControl() {

  String switch_number = sCmd.next();
  String order = sCmd.next();

  String all_line = jsonRead(optionJson, "switch" + String(switch_number));

  String on_off_1 = selectFromMarkerToMarker(all_line, " ", 0);
  String order_cmd_1 = selectFromMarkerToMarker(all_line, " ", 1);

  String on_off_2 = selectFromMarkerToMarker(all_line, " ", 2);
  String order_cmd_2 = selectFromMarkerToMarker(all_line, " ", 3);

  order_cmd_1.replace("_", " ");
  order_cmd_2.replace("_", " ");

  if (order == on_off_1) {
    if (order_cmd_1.indexOf("ush") > 0) {
      order_ticker += order_cmd_1 + ",";
    } else {
      order_loop += order_cmd_1 + ",";
    }
  }
  if (order == on_off_2) {
    if (order_cmd_2.indexOf("ush") > 0) {
      order_ticker += order_cmd_2 + ",";
    } else {
      order_loop += order_cmd_2 + ",";
    }
  }
}

void handleButton()  {

  static uint8_t switch_number = 1;

  if (but[switch_number]) {
    buttons[switch_number].update();

    if (buttons[switch_number].fell()) {
      order_loop += "swi " + String(switch_number) + " 1,";   //swi 1 1,
      jsonWrite(configJson, "swi" + String(switch_number), "1");
      //Serial.println("ON");
    }

    if (buttons[switch_number].rose()) {
      order_loop += "swi " + String(switch_number) + " 0,";   //swi 1 0,
      jsonWrite(configJson, "swi" + String(switch_number), "0");
      //Serial.println("OFF");
    }
  }
  switch_number++;
  if (switch_number == NUM_BUTTONS) switch_number = 0;
}


//=========================================Модуль измерения уровня в баке============================================================
void tank_levelInit() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String empty_level = sCmd.next();
  String full_level = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(optionJson, "levels", empty_level + " " + full_level);

  pinMode(14, OUTPUT);
  pinMode(12, INPUT);

  static String viget;
  if (flag) {
    viget = readFile("viget.fillgauge.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/lev_out");
  all_vigets += viget + "\r\n";

  ts.add(LEVEL, tank_level_shooting_interval, [&](void*) {

    long duration_;
    int distance_cm;
    int level_persent;
    static int level_persent_old; //переменная static сохраняет свое значение между вызовами функции
    static int counter;

    digitalWrite(14, LOW);
    delayMicroseconds(2);
    digitalWrite(14, HIGH);
    delayMicroseconds(10);
    digitalWrite(14, LOW);
    duration_ = pulseIn(12, HIGH, 30000); // 3000 µs = 50cm // 30000 µs = 5 m
    distance_cm = duration_ / 29 / 2;
    distance_cm = testFilter.filtered(distance_cm);//отсечение промахов медианным фильтром
#ifdef debug_mode_web_sokets
    SoketData("module_tank_level_s", distance_cm, 1);
#endif

    counter++;

    if (counter > tank_level_times_to_send) {
      counter = 0;
      jsonWrite(configJson, "lev_in", distance_cm);

      String levels = jsonRead(optionJson, "levels");
      level_persent = map(distance_cm, selectFromMarkerToMarker(levels, " ", 0).toInt(), selectFromMarkerToMarker(levels, " ", 1).toInt(), 0, 100);
      jsonWrite(configJson, "lev_out", level_persent);

      if (level_persent_old != level_persent) {

        sendSTATUS("lev_out", String(level_persent));
        Serial.println("sensor tank level send date " + String(level_persent));

      }
      level_persent_old = level_persent;
    }
  }, nullptr, true);
}

//=========================================Модуль аналогового сенсора============================================================
void analogInit() {

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
  if (flag) {
    viget = readFile("viget.fillgauge.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/ana_out");
  all_vigets += viget + "\r\n";

  ts.add(ANALOG, analog_update_int, [&](void*) {

    static int analog_out_old;
    int analog = analogRead(A0);
    jsonWrite(configJson, "ana_in", analog);

#ifdef debug_mode_web_sokets
    SoketData("module_analog_s", analog, 1);
#endif

    String analog_values = jsonRead(optionJson, "analog_values");
    int analog_out = map(analog, selectFromMarkerToMarker(analog_values, " ", 0).toInt(), selectFromMarkerToMarker(analog_values, " ", 1).toInt(), selectFromMarkerToMarker(analog_values, " ", 2).toInt(), selectFromMarkerToMarker(analog_values, " ", 3).toInt());
    jsonWrite(configJson, "ana_out", analog_out);

    if (analog_out_old != analog_out) {

      sendSTATUS("ana_out", String(analog_out));
      Serial.println("sensor analog send date " + String(analog_out));

    }

    analog_out_old = analog_out;

  }, nullptr, true);
}
//=========================================Модуль температурного сенсора ds18b20============================================================
void ds18b20Init() {

  static boolean flag = true;
  String pin = sCmd.next();
  String viget_name = sCmd.next();
  viget_name.replace("#", " ");
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  oneWire = new OneWire((uint8_t) pin.toInt());
  sensors.setOneWire(oneWire);
  sensors.begin();
  sensors.setResolution(1);


  static String viget;
  if (flag) {
    viget = readFile("viget.termometr.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);

  jsonWrite(viget, "topic", prex + "/ds_out");
  all_vigets += viget + "\r\n";

  ts.add(DS18B20, temp_update_int, [&](void*) {

    float temp = 0;
    static float temp_old;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    jsonWrite(configJson, "ds_out", String(temp));

#ifdef debug_mode_web_sokets
    SoketData("module_ds18b20_s", String(temp), "1");
#endif

    if (temp_old != temp) {

      sendSTATUS("ds_out", String(temp));
      Serial.println("sensor ds18b20 send date " + String(temp));

    }

    temp_old = temp;

  }, nullptr, true);
}
//==========================================Модуль управления ШИМ===================================================
void pwmInit() {

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
  jsonWrite(configJson, "pwm" + pwm_number, start_state);

  static String viget;
  if (flag) {
    viget = readFile("viget.range.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/pwm" + pwm_number);
  all_vigets += viget + "\r\n";
}

void pwmControl() {

  String pwm_number = sCmd.next();
  String pwm_state = sCmd.next();
  int pwm_state_int = pwm_state.toInt();

  int pin = jsonReadtoInt(optionJson, "pwm_pin" + pwm_number);
  analogWrite(pin, pwm_state_int);

  jsonWrite(configJson, "pwm" + pwm_number, pwm_state);
  sendSTATUS("pwm" + pwm_number, pwm_state);
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

}

//================================================команды отображения виджетов==========================================
void addViget() {

  static boolean flag = true;

  String number = sCmd.next();
  String viget_text_color = sCmd.next();
  viget_text_color.replace("#", " ");
  String viget_text_topic = sCmd.next();
  viget_text_topic.replace("#", " ");
  String page_name = sCmd.next();
  String page_number = sCmd.next();


  static String viget;
  if (flag) {
    viget = readFile("viget.alertsm.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "class2", viget_text_color);      //"calm-bg"  "assertive-bg"
  jsonWrite(viget, "descr", viget_text_topic);
  jsonWrite(viget, "topic", prex + "/swi" + number);
  all_vigets += viget + "\r\n";

}

void fillViget() {

  String number = sCmd.next();
  String viget_text = sCmd.next();
  viget_text.replace("#", " ");
  String date_time = sCmd.next();
  String time_ = GetTime();
  time_.replace(":", ".");
  if (date_time == "yes") viget_text = viget_text + " " + GetDataDigital() + " " + time_;
  sendSTATUS("swi" + number, viget_text);
  jsonWrite(configJson, "swi" + number , viget_text);

}

void setValue_() {

  String name_ = sCmd.next();   //val1
  String number = name_.substring(3);
  String start_value = sCmd.next();
  String step_ = sCmd.next();
  String value_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  static boolean flag1 = true;
  static String viget1;
  if (flag1) {
    viget1 = readFile("viget.buttonup.json", 1024);
    flag1 = false;
  }
  jsonWrite(viget1, "page", page_name);
  jsonWrite(viget1, "pageId", page_number);
  jsonWrite(viget1, "topic", prex + "/Bup" + number);
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
  jsonWrite(viget2, "descr", value_name);
  jsonWrite(viget2, "topic", prex + "/" + name_);
  sendSTATUS(name_, start_value);
  jsonWrite(configJson, name_, start_value);
  all_vigets += viget2 + "\r\n";

  static boolean flag3 = true;
  static String viget3;
  if (flag3) {
    viget3 = readFile("viget.buttondown.json", 1024);
    flag3 = false;
  }
  jsonWrite(viget3, "page", page_name);
  page_number = String(page_number.toInt() + 1);
  jsonWrite(viget3, "pageId", page_number);
  jsonWrite(viget3, "topic", prex + "/Bdw" + number);
  all_vigets += viget3 + "\r\n";
}

void Bup() {
  String number = sCmd.next();
  int val = jsonReadtoInt(configJson, "val" + number);
  val++;
  jsonWrite(configJson, "val" + number, String(val));
  sendSTATUS("val" + number, String(val));
}

void Bdw() {
  String number = sCmd.next();
  int val = jsonReadtoInt(configJson, "val" + number);
  val--;
  jsonWrite(configJson, "val" + number, String(val));
  sendSTATUS("val" + number, String(val));
}



//=========================================Сценарии для всех модулей============================================================

void handleScenario() {

  String module_name = sCmd.next();
  String sign = sCmd.next();
  String value = sCmd.next();
  String order_cmd = sCmd.next();
  String type = sCmd.next();
  calculateScenario(module_name, sign, value, order_cmd, type);

}

void calculateScenario(String module_name, String sign, String value, String order_cmd, String type) {

  order_cmd.replace("_", " ");
  static boolean flag_ana_1 = true;
  static boolean flag_ana_2 = true;
  int value_new;
  String value_name;

  if (module_name == "ANALOG") value_name = "ana_out";
  if (module_name == "LEVEL") value_name = "lev_out";

  if (sign == ">") {
    if (value.indexOf("val") >= 0) {
      String nubmer = value;
      nubmer.replace("val", "");
      value_new = jsonReadtoInt(configJson, "val" + nubmer);
    } else {
      value_new = value.toInt();
    }
    if (jsonReadtoInt(configJson, value_name) > value_new) {
      if (type == "repeat-no") {
        if (flag_ana_1) {
          order_loop += order_cmd + ",";
          flag_ana_1 = false;
          flag_ana_2 = true;
        }
      } else {
        order_loop += order_cmd + ",";
      }
    }
  }
  if (sign == "<") {
    if (value.indexOf("val") >= 0) {
      String nubmer = value;
      nubmer.replace("val", "");
      value_new = jsonReadtoInt(configJson, "val" + nubmer);
    } else {
      value_new = value.toInt();
    }
    if (jsonReadtoInt(configJson, value_name) < value_new) {
      if (type == "repeat-no") {
        if (flag_ana_2) {
          order_loop += order_cmd + ",";
          flag_ana_1 = true;
          flag_ana_2 = false;
        }
      } else {
        order_loop += order_cmd + ",";
      }
    }
  }
}
//=========================================Таймера=================================================================

void handleTimers() {

  String seted_time = sCmd.next();
  String order = sCmd.next();
  order.replace("_", " ");

  // Serial.println(seted_time);

  if (seted_time == current_time) {

    if (order.indexOf("ush") > 0) {
      order_ticker += order + ",";
    } else {
      order_loop += order + ",";
    }
  }
}



//======================выполнение команд (в лупе) по очереди из строки order=======================================
void handleCMD_loop() {

  if (order_loop != "") {

    String tmp = selectToMarker(order_loop, ",");                //выделяем из страки order первую команду rel 5 1,
    sCmd.readStr(tmp);                                           //выполняем первую команду
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
        sCmd.readStr(tmp);                                             //выполняем первую команду
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
