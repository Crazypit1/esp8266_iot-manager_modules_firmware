//добавление команды
void CMD_init() {

  sCmd.addCommand("RELAY",  relayInit);
  sCmd.addCommand("rel",  relayControl);

  sCmd.addCommand("LEVEL",  tank_levelInit);

  sCmd.addCommand("ANALOG",  analogInit);



  sCmd.addCommand("SCENARIO",  Scenario);
}

//==========================================МОДУЛЬ РЕЛЕ===================================================
void relayInit() {

  static boolean flag = true;
  String relay_number = sCmd.next();
  String relay_pin = sCmd.next();
  String button_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();
  String start_state = sCmd.next();

  uint8_t relay_pin_int = relay_pin.toInt();
  jsonWrite(optionJson, "relay" + relay_number, relay_pin);
  pinMode(relay_pin_int, OUTPUT);
  digitalWrite(relay_pin_int, start_state.toInt());
  jsonWrite(configJson, "rel" + relay_number, start_state);

  static String viget;
  if (flag) {
    viget = readFile("blok.relay.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", button_name);
  jsonWrite(viget, "topic", prex + "/rel" + relay_number);
  all_vigets += viget + "\r\n";
}
//==========================================КОНТРОЛЬ МОДУЛЯ РЕЛЕ===================================================
void relayControl() {

  String relay_number = sCmd.next();
  String relay_state = sCmd.next();
  int relay_state_int = relay_state.toInt();

  int pin = jsonReadtoInt(optionJson, "relay" + relay_number);
  digitalWrite(pin, relay_state_int);

  jsonWrite(configJson, "rel" + relay_number, relay_state);
  sendSTATUS("rel" + relay_number, relay_state);
}

//=========================================МОДУЛЬ TANK LEVEL============================================================
void tank_levelInit() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();
  String empty_level = sCmd.next();
  String full_level = sCmd.next();
  jsonWrite(optionJson, "empty_level", empty_level);
  jsonWrite(optionJson, "full_level", full_level);

  pinMode(14, OUTPUT);
  pinMode(12, INPUT);

  static String viget;

  if (flag) {
    viget = readFile("blok.tank.level.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/lev");
  all_vigets += viget + "\r\n";
}
//=========================================МОДУЛЬ АНАЛОГОВЫЙ СЕНСОР============================================================
void analogInit() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();
  String start_value = sCmd.next();
  String end_value = sCmd.next();
  String start_value_out = sCmd.next();
  String end_value_out = sCmd.next();
  jsonWrite(optionJson, "start_value", start_value);
  jsonWrite(optionJson, "end_value", end_value);
  jsonWrite(optionJson, "start_value_out", start_value_out);
  jsonWrite(optionJson, "end_value_out", end_value_out);

  pinMode(A0, INPUT);

  static String viget;

  if (flag) {
    viget = readFile("blok.analog.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", viget_name);
  jsonWrite(viget, "topic", prex + "/ana");
  all_vigets += viget + "\r\n";
}

//=========================================СЦЕНАРИИ ДЛЯ ВСЕХ МОДУЛЕЙ============================================================
void Scenario() {

  String module_name = sCmd.next();
  String sign = sCmd.next();
  String value = sCmd.next();

  String id = sCmd.next();
  String topik = sCmd.next();
  String order = sCmd.next();


  if (module_name == "LEVEL") {
    if (sign == ">") {
      if (jsonReadtoInt(configJson, "lev") > value.toInt()) {
        sendCONTROL(id, topik, order);
      }

    }
    if (sign == "<") {
      if (jsonReadtoInt(configJson, "lev") < value.toInt()) {
        sendCONTROL(id, topik, order);
      }
    }
  }

  if (module_name == "ANALOG") {
    if (sign == ">") {
      if (jsonReadtoInt(configJson, "ana") > value.toInt()) {
        sendCONTROL(id, topik, order);
      }

    }
    if (sign == "<") {
      if (jsonReadtoInt(configJson, "ana") < value.toInt()) {
        sendCONTROL(id, topik, order);
      }
    }
  }

}
















