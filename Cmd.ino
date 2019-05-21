//добавление команды
void CMD_init() {

  sCmd.addCommand("RELAY",  relayInit);
  sCmd.addCommand("rel",  relayControl);

  sCmd.addCommand("SWITCH",  switchInit);

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

  //send_push("Дом", "Реле" + relay_number);

  jsonWrite(configJson, "rel" + relay_number, relay_state);
  sendSTATUS("rel" + relay_number, relay_state);
}
//=========================================Модуль физической кнопки================================================================
void switchInit() {

  static boolean flag = true;
  String switch_number = sCmd.next();  //номер кнопки на этом устройстве
  String switch_pin = sCmd.next();     //пин кнопки на этом
  String switch_delay = sCmd.next();

  String id = sCmd.next();             //id другого устройства
  String relay_number = sCmd.next();   //номер реле на др устройстве

  String switch_name = sCmd.next();
  String page_name = sCmd.next();
  String push_msg_on = sCmd.next();
  String push_msg_off = sCmd.next();
  String page_number = sCmd.next();

  jsonWrite(configJson, "switch" + switch_number, id + " " + relay_number + " " + push_msg_on + " " + push_msg_off + " " + switch_name + " " + page_number); //"switch1":"2768820-1458190 1 on off name 5"
  //"switch3":"NA NA 5"

  buttons[switch_number.toInt()].attach(switch_pin.toInt());
  buttons[switch_number.toInt()].interval(switch_delay.toInt());
  but[switch_number.toInt()] = true;

  if (page_number != "NA") {

    static String viget;
    if (flag) {
      viget = readFile("blok.switch.json", 1024);
      flag = false;
    }

    jsonWrite(viget, "page", page_name);
    jsonWrite(viget, "pageId", page_number);
    jsonWrite(viget, "descr", switch_name);
    jsonWrite(viget, "topic", prex + "/led" + switch_number);
    all_vigets += viget + "\r\n";
  }
}

void handleButton()  {

  static uint8_t switch_number = 1;

  if (but[switch_number]) {
    buttons[switch_number].update();

    if (buttons[switch_number].fell()) {
      //Serial.println("Tach on " + String(switch_number));
      //"switch3":"NA NA on off 5"
      String tmp = jsonRead(configJson, "switch" + String(switch_number));

      String id = selectFromMarkerToMarker(tmp, " ", 0);
      String relay_number = selectFromMarkerToMarker(tmp, " ", 1);
      //order
      String push_msg_on = selectFromMarkerToMarker(tmp, " ", 2);
      String push_msg_off = selectFromMarkerToMarker(tmp, " ", 3);
      String switch_name = selectFromMarkerToMarker(tmp, " ", 4);
      String page_number = selectFromMarkerToMarker(tmp, " ", 5);

      order_switch += String(switch_number) + " " + id + " " + relay_number + " " + "1" + " " + push_msg_on + " " + push_msg_off + " " + switch_name + " " + page_number + ","; //3 NA NA 1 on off name 5,
      //Serial.println(order_switch);
    }

    if (buttons[switch_number].rose()) {
      //Serial.println("Tach off " + String(switch_number));
      String tmp = jsonRead(configJson, "switch" + String(switch_number));

      String id = selectFromMarkerToMarker(tmp, " ", 0);
      String relay_number = selectFromMarkerToMarker(tmp, " ", 1);
      //order
      String push_msg_on = selectFromMarkerToMarker(tmp, " ", 2);
      String push_msg_off = selectFromMarkerToMarker(tmp, " ", 3);
      String switch_name = selectFromMarkerToMarker(tmp, " ", 4);
      String page_number = selectFromMarkerToMarker(tmp, " ", 5);

      order_switch += String(switch_number) + " " + id + " " + relay_number + " " + "0" + " " + push_msg_on + " " + push_msg_off + " " + switch_name + " " + page_number + ","; //3 NA NA 0 on off name 5,
      //Serial.println(order_switch);
    }
  }
  switch_number++;
  if (switch_number == NUM_BUTTONS) switch_number = 0;

}

void handleSwitch() {

  if (order_switch != "") {

    String tmp = selectToMarker(order_switch, ",");

    //3 NA NA 0 on off name 5,
    String switch_number = selectFromMarkerToMarker(order_switch, " ", 0);
    String id = selectFromMarkerToMarker(order_switch, " ", 1);
    String relay_number = selectFromMarkerToMarker(order_switch, " ", 2);
    String order = selectFromMarkerToMarker(order_switch, " ", 3);
    String push_msg_on = selectFromMarkerToMarker(order_switch, " ", 4);
    String push_msg_off = selectFromMarkerToMarker(order_switch, " ", 5);
    String switch_name = selectFromMarkerToMarker(order_switch, " ", 6);
    String page_number = selectFromMarkerToMarker(order_switch, " ", 7);

    if (id != "NA") sendCONTROL(id, "rel" + relay_number, order);
    if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
    if (page_number != "NA") sendSTATUS("led" + switch_number, order);

    if (push_msg_on != "NA" && order == "1") send_push(switch_name, push_msg_on);
    if (push_msg_off != "NA" && order == "0") send_push(switch_name, push_msg_off);

    order_switch = deleteBeforeDelimiter(order_switch, ",");

  }
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
    viget = readFile("viget.gauge.json", 1024);//blok.analog.json
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
  String topik = sCmd.next();  //rel1
  String relay_number = deleteBeforeDelimiter(topik, "l");
  Serial.println(relay_number);
  String order = sCmd.next();
  String push = sCmd.next();

  static boolean flag_level_1 = true;
  static boolean flag_level_2 = true;

  static boolean flag_analog_1 = true;
  static boolean flag_analog_2 = true;

  static boolean flag_temp_1 = true;
  static boolean flag_temp_2 = true;


  if (module_name == "LEVEL") {
    if (jsonRead(configSetup, "module_tank_level") == "1") {
      if (sign == ">") {
        if (jsonReadtoInt(configJson, "lev") > value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_level_1) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "lev"));
              flag_level_1 = false;
              flag_level_2 = true;
            }
          }
        }
      }
      if (sign == "<") {
        if (jsonReadtoInt(configJson, "lev") < value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_level_2) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "lev"));
              flag_level_1 = true;
              flag_level_2 = false;
            }
          }
        }
      }
    }
  }
  if (module_name == "ANALOG") {
    if (jsonRead(configSetup, "module_analog") == "1") {
      if (sign == ">") {
        if (jsonReadtoInt(configJson, "ana") > value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_analog_1) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "ana"));
              flag_analog_1 = false;
              flag_analog_2 = true;
            }
          }
        }
      }
      if (sign == "<") {
        if (jsonReadtoInt(configJson, "ana") < value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_analog_2) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "ana"));
              flag_analog_1 = true;
              flag_analog_2 = false;
            }
          }
        }
      }
    }
  }
  if (module_name == "TEMP_ds18b20") {
    if (jsonRead(configSetup, "module_ds18b20") == "1") {
      if (sign == ">") {
        if (jsonReadtoInt(configJson, "DS") > value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_temp_1) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "DS"));
              flag_temp_1 = false;
              flag_temp_2 = true;
            }
          }
        }
      }
      if (sign == "<") {
        if (jsonReadtoInt(configJson, "DS") < value.toInt()) {
          if (id != "NA") sendCONTROL(id, topik, order);
          if (id == "LOCAL") order_relays += "rel " + relay_number + " " + order + ",";
          if (push != "NA") {
            if (flag_temp_2) {
              send_push(push, sign + value + "=" + jsonReadtoInt(configJson, "DS"));
              flag_temp_1 = true;
              flag_temp_2 = false;
            }
          }
        }
      }
    }
  }

}















