//добавление команды
void CMD_init() {

  sCmd.addCommand("RELAY",  relayInit);
  sCmd.addCommand("rel",  relayControl);

  sCmd.addCommand("BUTTON",  buttonInit);
  sCmd.addCommand("LEVEL",  tank_levelInit);
  sCmd.addCommand("ANALOG",  analogInit);
  sCmd.addCommand("TEMP_ds18b20",  ds18b20Init);

  sCmd.addCommand("SCENARIO",  Scenario);
}

//==========================================Модуль управления реле===================================================
void relayInit() {

  static boolean flag = true;
  String relay_number = sCmd.next();
  String relay_pin = sCmd.next();
  String button_name = sCmd.next();
  String page_name = sCmd.next();
  String start_state = sCmd.next();
  String page_number = sCmd.next();
  

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

void relayControl() {

  String relay_number = sCmd.next();
  String relay_state = sCmd.next();
  int relay_state_int = relay_state.toInt();

  int pin = jsonReadtoInt(optionJson, "relay" + relay_number);
  digitalWrite(pin, relay_state_int);

  jsonWrite(configJson, "rel" + relay_number, relay_state);
  sendSTATUS("rel" + relay_number, relay_state);
}
//=========================================Модуль физической кнопки================================================================
void buttonInit() {

  static boolean flag = true;
  String button_number = sCmd.next();
  String button_pin = sCmd.next();
  String button_delay = sCmd.next();
  String button_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  // jsonWrite(optionJson, "empty_level", button_number);

  buttons[button_number.toInt()].attach(button_pin.toInt());
  buttons[button_number.toInt()].interval(button_delay.toInt());
  but[button_number.toInt()] = true;

  static String viget;
  if (flag) {
    viget = readFile("blok.button.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);
  jsonWrite(viget, "descr", button_name);
  jsonWrite(viget, "topic", prex + "/led" + button_number);
  all_vigets += viget + "\r\n";
}

void handleButton()  {

  static uint8_t button_number = 0;

  if (but[button_number]) {
    buttons[button_number].update();
    if (buttons[button_number].fell()) {
      Serial.println("Tach on " + String(button_number));
      sendSTATUS("led" + String(button_number), "1", "speech", "button " + String(button_number) + "on");
    }
    if (buttons[button_number].rose()) {
      Serial.println("Tach off " + String(button_number));
      sendSTATUS("led" + String(button_number), "0", "speech", "button " + String(button_number) + "off");
    }
  }
  button_number++;
  if (button_number == NUM_BUTTONS) button_number = 0;
}
//=========================================Модуль измерения уровня в баке============================================================
void tank_levelInit() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String empty_level = sCmd.next();
  String full_level = sCmd.next();
  String page_number = sCmd.next();
  
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
//=========================================Модуль аналогового сенсора============================================================
void analogInit() {

  static boolean flag = true;
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String start_value = sCmd.next();
  String end_value = sCmd.next();
  String start_value_out = sCmd.next();
  String end_value_out = sCmd.next();
  String page_number = sCmd.next();
  
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

//=========================================Модуль температурного сенсора ds18b20============================================================
void ds18b20Init() {

  static boolean flag = true;
  String pin = sCmd.next();
  String viget_name = sCmd.next();
  String page_name = sCmd.next();
  String page_number = sCmd.next();

  oneWire = new OneWire((uint8_t) pin.toInt());
  sensors.setOneWire(oneWire);
  sensors.begin();
  sensors.setResolution(1);


  static String viget;
  if (flag) {
    viget = readFile("blok.termometr.json", 1024);
    flag = false;
  }

  jsonWrite(viget, "page", page_name);
  jsonWrite(viget, "pageId", page_number);

  jsonWrite(viget, "topic", prex + "/DS");
  all_vigets += viget + "\r\n";
}

//=========================================Сценарии для всех модулей============================================================
void Scenario() {

  String module_name = sCmd.next();
  String sign = sCmd.next();
  String value = sCmd.next();

  String id = sCmd.next();
  String topik = sCmd.next();
  String order = sCmd.next();


  if (module_name == "LEVEL") {
    if (jsonRead(configSetup, "module_tank_level") == "1") {
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
  }
  if (module_name == "ANALOG") {
    if (jsonRead(configSetup, "module_analog") == "1") {
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
  if (module_name == "TEMP_ds18b20") {
    if (jsonRead(configSetup, "module_ds18b20") == "1") {
      if (sign == ">") {
        if (jsonReadtoInt(configJson, "DS") > value.toInt()) {
          sendCONTROL(id, topik, order);
        }
      }
      if (sign == "<") {
        if (jsonReadtoInt(configJson, "DS") < value.toInt()) {
          sendCONTROL(id, topik, order);
        }
      }
    }
  }


}















