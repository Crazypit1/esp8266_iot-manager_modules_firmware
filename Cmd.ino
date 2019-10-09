void CMD_init() {

  sCmd.addCommand("button",  button);
  sCmd.addCommand("buttonSet",  buttonSet);

  sCmd.addCommand("pwm",  pwm);
  sCmd.addCommand("pwmSet",  pwmSet);

  sCmd.addCommand("switch",  switch_);

  sCmd.addCommand("analog",  analog);
  sCmd.addCommand("ph",  ph);
  sCmd.addCommand("level",  level);
  sCmd.addCommand("dallas",  dallas);

  sCmd.addCommand("logging",  logging);

  sCmd.addCommand("input",  input);
  sCmd.addCommand("valueUpSet",  valueUpSet);
  sCmd.addCommand("valueDownSet",  valueDownSet);

  sCmd.addCommand("text",  text);
  sCmd.addCommand("textSet",  textSet);

  sCmd.addCommand("time",  time);
  // sCmd.addCommand("timeSet",  timeSet);

  sCmd.addCommand("timerStart",  timerStart);
  sCmd.addCommand("timerStop",  timerStop);

  sCmd.addCommand("mqtt",  mqttOrderSend);
  sCmd.addCommand("http",  httpOrderSend);
  sCmd.addCommand("push",  pushControl);

  //handleCMD_ticker();
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

  if (button_pin != "na"  || button_pin != "scenario" || button_pin.indexOf("line") != -1) {
    pinMode(button_pin.toInt(), OUTPUT);
    digitalWrite(button_pin.toInt(), start_state.toInt());
    uint8_t button_pin_int = button_pin.toInt();
  }


  jsonWrite(optionJson, "button_pin" + button_number, button_pin);
  jsonWrite(configJson, "buttonSet" + button_number, start_state);

  if  (button_pin == "scenario") {
    jsonWrite(configSetup, "scenario", start_state);
    Scenario_init();
    saveConfig();
  }

  if  (button_pin.indexOf("line") != -1) {

    String str = button_pin;


    while (str.length() != 0) {
      String tmp = selectToMarker (str, ",");  //line1,

      String number = deleteBeforeDelimiter(tmp, "e");   //1,
      number.replace(",", "");
      Serial.println(number);
      int number_int = number.toInt();
      scenario_line_status[number_int] = start_state.toInt();

      str = deleteBeforeDelimiter(str, ",");
    }
  }

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

  if (button_pin != "na" || button_pin != "scenario" || button_pin.indexOf("line") != -1) {
    digitalWrite(button_pin.toInt(), button_state.toInt());
  }
  if  (button_pin == "scenario") {
    jsonWrite(configSetup, "scenario", button_state);
    Scenario_init();
    saveConfig();
  }

  if  (button_pin.indexOf("line") != -1) {

    String str = button_pin;

    while (str.length() != 0) {
      String tmp = selectToMarker (str, ",");  //line1,

      String number = deleteBeforeDelimiter(tmp, "e");   //1,
      number.replace(",", "");
      Serial.println(number);
      int number_int = number.toInt();
      scenario_line_status[number_int] = button_state.toInt();

      str = deleteBeforeDelimiter(str, ",");
    }
  }

  eventGen ("buttonSet", button_number);

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

  eventGen ("pwmSet", pwm_number);

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

      eventGen ("switchSet", String(switch_number));

      jsonWrite(configJson, "switchSet" + String(switch_number), "1");
    }
    if (buttons[switch_number].rose()) {

      eventGen ("switchSet", String(switch_number));

      jsonWrite(configJson, "switchSet" + String(switch_number), "0");
    }
  }
  switch_number++;
  if (switch_number == NUM_BUTTONS) switch_number = 0;
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

  int psn1 = start_value.indexOf(".");  //ищем позицию запятой

  int digits = 0;

  if (psn1 != -1) {                     //если она есть
    String last_part = deleteBeforeDelimiter(start_value, ".");
    digits = last_part.length() + 1;
  }

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

  if (by_button) jsonWrite(configSetup, name_, start_value);

  jsonWrite(configJson, name_, jsonRead(configSetup, name_));

  jsonWrite(configJson, name_ + "step", step_);
  jsonWrite(configJson, name_ + "digits", digits);


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
  int digits = jsonRead(configJson, "value" + number + "digits").toInt();
  val = val + step_;
  String val_str = String(val);
  val_str = selectToMarkerPlus (val_str, ".", digits);
  jsonWrite(configJson, "value" + number, val_str);
  jsonWrite(configSetup, "value" + number, val_str);
  saveConfig();
  sendSTATUS("value" + number, val_str);
}

void valueDownSet() {
  String number = sCmd.next();
  float val = jsonRead(configJson, "value" + number).toFloat();
  float step_ = jsonRead(configJson, "value" + number + "step").toFloat();
  int digits = jsonRead(configJson, "value" + number + "digits").toInt();
  val = val - step_;
  String val_str = String(val);
  val_str = selectToMarkerPlus (val_str, ".", digits);
  jsonWrite(configJson, "value" + number, val_str);
  jsonWrite(configSetup, "value" + number, val_str);
  saveConfig();
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

  jsonWrite(configJson, "textSet" + number, text);
  sendSTATUS("textSet" + number, text);
}

//=================================================Глобальные команды удаленного управления===========================================================

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



//==============================================================================================================================
//============================выполнение команд (в лупе) по очереди из строки order=============================================
void handleCMD_loop() {

  if (order_loop != "") {

    String tmp = selectToMarker(order_loop, ",");                //выделяем из страки order первую команду rel 5 1,
    if (tmp != "no_command") sCmd.readStr(tmp);                                           //выполняем первую команду
    Serial.println("order_loop => " + order_loop);
    order_loop = deleteBeforeDelimiter(order_loop, ",");         //осекаем выполненную команду
  }
}

//=============================выполнение команд (через период) по очереди из строки order=======================================
/*void handleCMD_ticker() {

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
  }*/



//=======================================================================================================================================
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
