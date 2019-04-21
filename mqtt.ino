//===============================================ИНИЦИАЛИЗАЦИЯ================================================

void MQTT_init() {

  HTTP.on("/mqttSave", HTTP_GET, []() {

    String mqtt_server = HTTP.arg("mqttServer");
    int mqtt_port = HTTP.arg("mqttPort").toInt();
    String mqtt_user = HTTP.arg("mqttUser");
    String mqtt_pass = HTTP.arg("mqttPass");

    jsonWrite(configSetup, "mqttServer", mqtt_server);
    jsonWrite(configSetup, "mqttPort", mqtt_port);
    jsonWrite(configSetup, "mqttUser", mqtt_user);
    jsonWrite(configSetup, "mqttPass", mqtt_pass);

    saveConfig();

    client.disconnect();
    MQTT_Connecting(false);


    HTTP.send(200, "application/json", stateMQTT());

  });


  jsonWrite(configSetup, "chipID", chipID);

  //проверка подключения к серверу

#ifdef reconnecting
  ts.add(1, reconnecting, [&](void*) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("->WiFi-ok");
      if (client.connected()) {
        Serial.println("->MQTT-ok");
        led_blink(2, 20, "off");
      } else {
        Serial.println("->Lost MQTT connection, start reconnecting");
        led_blink(2, 20, "on");
        MQTT_Connecting(false);
      }
    } else {
      Serial.println("->Lost WiFi connection");
      ts.remove(1);
      StartAPMode();
    }
  }, nullptr, true);
#endif
}




//================================================ОБНОВЛЕНИЕ====================================================

void  handleMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connected()) {
      client.loop();
    }
    else {

    }
  }
}

//===============================================ПОДКЛЮЧЕНИЕ========================================================
void MQTT_Connecting(boolean send_date) {

  String mqtt_server = jsonRead(configSetup, "mqttServer");

  if ((mqtt_server != "")) {

    client.setServer(mqtt_server.c_str(), jsonReadtoInt(configSetup, "mqttPort"));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        Serial.println("->Connecting to MQTT server commenced");
        if (client.connect(chipID.c_str(), jsonRead(configSetup, "mqttUser").c_str(), jsonRead(configSetup, "mqttPass").c_str())) {
          led_blink(2, 20, "off");
          Serial.println("->Connecting to MQTT server completed");
          //Serial.println(stateMQTT());
          SoketData ("stateMQTT", stateMQTT(), "");

          client.setCallback(callback);

          client.subscribe(prefix.c_str());  // Для приема получения HELLOW и подтверждения связи
          client.subscribe((prefix + "/" + chipID + "/+/control").c_str()); // Подписываемся на топики control
          client.subscribe((prefix + "/ids").c_str()); // Подписываемся на топики ids

          //SCENARIO ANALOG > 5 800324-1458415 rel1 0
          if (jsonRead(configSetup, "scenario") == "1") {
            String all_text = readFile("scenario.all.txt", 1024) + "\r\n";
            all_text.replace("\r\n", "\n");
            all_text.replace("\r", "\n");
            while (all_text.length() != 0) {
              String line_ = selectToMarker (all_text, "\n");
              String id = selectFromMarkerToMarker(line_, " ", 4);
              if (id != "not found") {
                Serial.println(id);
                //long st_time = millis();
                int st = client.subscribe((prefix + "/" + id + "/+/status").c_str(), 0);
                //long end_time = millis();
                //Serial.println("status = " + String(st) + ", timeout = " + String(end_time - st_time));
              }
              all_text = deleteBeforeDelimiter(all_text, "\n");
            }
          }

          if (send_date) outcoming_date(); //отправляем данные в виджеты



        } else {
          //Serial.println(stateMQTT());
          SoketData ("stateMQTT", stateMQTT(), "");
          Serial.println("try again in " + String(reconnecting / 1000) +  " sec");
          led_blink(2, 20, "on");
        }
      }
    }
  }
}

//=====================================================ВХОДЯЩИЕ И ИСХОДЯЩИЕ ДАННЫЕ========================================================

void callback(char* topic, byte* payload, unsigned int length) {

  //Serial.print(topic);
  String topic_str = String(topic);

  String str;
  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
  }
  //Serial.println(" " + str);
  if (str == "HELLO") outcoming_date();

  if (topic_str.indexOf("control") > 0) {                      //IoTmanager/800324-1458415/rel0/control 1
    topic_str = deleteToMarkerLast(topic_str, "/control");     //IoTmanager/799371-1458415/rel0
    topic_str = selectToMarkerLast(topic_str, "/");                                      //rel0
    String t = topic_str.substring(3);                                                      //0
    topic_str.replace(t, " " + t);                                                      //rel 0
    topic_str += " " + str;
    order += topic_str + ",";
    //Serial.println(order);
  }
}


/*void callback(const MQTT::Publish& pub) { //если что то пришло

  //Serial.print(pub.topic());
  //Serial.print(" => ");
  //Serial.print(pub.payload_string());
  //Serial.println();
  String payload = pub.payload_string();

  //---------обработка входящих данных----------------------------------------------------------------------------------------
  if (pub.payload_string() == "HELLO" ) outcoming_date();                  //отправляем данные при обновлении страницы!!!
  if (String(pub.topic()) == prefix + "/ids") ids = pub.payload_string();  //получаем ids

  String topic_ = String(pub.topic());    //формируем строку из строки /pin5 ==>> pin 5 1,   (где pin - название группы(о чем идет речь), 5 - номер пина, 1 - значение )
  if (topic_.indexOf("control") > 0) {
    //IoTmanager/800324-1458415/rel0/control 1
    topic_ = deleteToMarkerLast(topic_, "/control");
    //Serial.println(topic_);                               //IoTmanager/799371-1458415/rel0
    topic_ = selectToMarkerLast(topic_, "/");
    //Serial.println(topic_);                               //rel0
    String t = topic_.substring(3);
    //Serial.println(t);                                    //0
    topic_.replace(t, " " + t);
    //Serial.println(topic_);                               //rel 0
    topic_ += " " + pub.payload_string();
    //Serial.println(topic_);
    order += topic_ + ",";
    Serial.println(order);
  }
  ///IoTmanager/800324-1458415/rel1/status {"status":"1"}

  /* if (topic_.indexOf(jsonRead(optionJson, "scenario_device")) > 0) {

    }*/
//}

//данные которые отправляем при подключении или отбновлении страницы
void outcoming_date() {

  sendMQTT("test", "work");
  sendAllWigets();
  sendAllData();
  Serial.println("->Sending all date to iot manager completed");

}
//== == == == == == == == == = CONFIG == == == == == == == == == == == == == == == == == == =
///IoTmanager/2058631-1589487/config {----viget----}
///sendMQTT("config", data);
void sendMQTT(String end_of_topik, String data) {
  String topik = prefix + "/" + chipID + "/" + end_of_topik;
  client.beginPublish(topik.c_str(), data.length(), false);
  client.print(data);
  client.endPublish();
}
//== == == == == == == == == = STATUS == == == == == == == == == == == == == == == == == == =
///IoTmanager/800324-1458415/rel1/status {"status":"1"}
///sendSTATUS(topic, state)
void sendSTATUS(String topik, String state) {
  topik = prefix + "/" + chipID + "/" + topik + "/" + "status";
  String json_ = "{}";
  jsonWrite(json_, "status", state);

  //long st_time = millis();
  int send_status =  client.publish (topik.c_str(), json_.c_str(), false);
  //long end_time = millis();
  //Serial.println("status = " + String(send_status) + ", timeout = " + String(end_time - st_time));
}
//== == == == == == == == == = CONTROL == == == == == == == == == == == == == == == == == == =
///IoTmanager/800324-1458415/rel1/control 1
void sendCONTROL(String id, String topik, String state) {
  String  all_line = prefix + "/" + id + "/" + topik + "/control";

  //long st_time = millis();
  int send_status = client.publish (all_line.c_str(), state.c_str(), false);
  //long end_time = millis();
  //Serial.println("status = " + String(send_status) + ", timeout = " + String(end_time - st_time));
}


//выполнение команд в лупе по очереди из строки order
void handleCMD() {

  if (order != "") {

    String tmp = selectToMarker(order, ",");      //выделяем из страки order первую команду rel 5 1
    sCmd.readStr(tmp);                            //выполняем первую команду
    Serial.println(order);
    order = deleteBeforeDelimiter(order, ",");    //осекаем выполненную команду


  }
}
//=====================================================ОТПРАВЛЯЕМ ВИДЖЕТЫ========================================================
void sendAllWigets() {

  String viget = all_vigets;
  int counter = 0;

  while (viget.length() != 0) {
    if (!client.connected()) return;
    String tmp = selectToMarker (viget, "\r\n");
    jsonWrite(tmp, "id", String(counter));
    sendMQTT("config", tmp);
    Serial.println(counter);
    //Serial.println(tmp);
    counter++;
    viget = deleteBeforeDelimiter(viget, "\r\n");
  }
}
//=====================================================ОТПРАВЛЯЕМ ДАННЫЕ В ВИДЖЕТЫ ПРИ ОБНОВЛЕНИИ СТРАНИЦЫ========================================================
void sendAllData() {

  String current_config = configJson;
  current_config.replace("{", "");
  current_config.replace("}", "");
  current_config += ",";

  while (current_config.length() != 0) {

    String tmp = selectToMarker (current_config, ","); //"SSDP":"OVEN"
    String topic =  selectToMarker (tmp, ":");        //"SSDP"
    topic.replace("\"", "");                          //SSDP

    String state =  selectToMarkerLast (tmp, ":");
    state.replace("\"", "");
    if (topic != "SSDP" && topic != "lang" && topic != "ip") sendSTATUS(topic, state);
    current_config = deleteBeforeDelimiter(current_config, ",");
  }
}
//=====================================================ОТПРАВЛЯЕМ ДАННЫЕ В ВИДЖЕТЫ ПРИ ОБНОВЛЕНИИ ДАННЫХ========================================================
void Sensors_init() {

  ts.add(6, 10000, [&](void*) {

    long st_time = millis();

    if (client.connected()) {

      //========================================МОДУЛЬ TANK LEVEL==============================================
      if (jsonRead(configSetup, "module_tank_level") == "1")
      {
        long duration_;
        int distance_cm;
        int level_persent;
        static int distance_cm_old; //переменная static сохраняет свое значение между вызовами функции
        digitalWrite(14, LOW);
        yield();
        delayMicroseconds(2);
        yield();
        digitalWrite(14, HIGH);
        yield();
        delayMicroseconds(10);
        yield();
        digitalWrite(14, LOW);
        duration_ = pulseIn(12, HIGH, 30000); // 3000 µs = 50cm // 30000 µs = 5 m
        distance_cm = duration_ / 29 / 2;

        jsonWrite(optionJson, "distance_cm", distance_cm);

        if (distance_cm_old != distance_cm) {

          level_persent = map(distance_cm, jsonReadtoInt(optionJson, "empty_level"), jsonReadtoInt(optionJson, "full_level"), 0, 100);
          sendSTATUS("lev", String(level_persent));
          jsonWrite(configJson, "lev", level_persent);
          Serial.println("1");
        }
        distance_cm_old = distance_cm;
      }
      //===========================================МОДУЛЬ АНАЛОГОВЫЙ СЕНСОР========================================
      if (jsonRead(configSetup, "module_analog") == "1") {

        int analog = analogRead(A0);
        jsonWrite(optionJson, "analog", analog);
        static int analog_old;

        if (analog_old != analog) {

          int analog_out = map(analog, jsonReadtoInt(optionJson, "start_value"), jsonReadtoInt(optionJson, "end_value"), jsonReadtoInt(optionJson, "start_value_out"), jsonReadtoInt(optionJson, "end_value_out"));
          sendSTATUS("ana", String(analog_out));
          jsonWrite(configJson, "ana", analog_out);
          Serial.println("2");
        }
        analog_old = analog;
      }
      //===============================================================================================================

      if (jsonRead(configSetup, "scenario") == "1") stringExecution(scenario);

      //===============================================================================================================

    }

    long end_time = millis();
    Serial.println("sensor total timeout = " + String(end_time - st_time));

  }, nullptr, true);
}

String stateMQTT() {

  int state = client.state();

  switch (state) {
    case -4: return "the server didn't respond within the keepalive time";
      break;
    case -3: return "the network connection was broken";
      break;
    case -2: return "the network connection failed";
      break;
    case -1: return "the client is disconnected cleanly";
      break;
    case 0: return "the client is connected";
      break;
    case 1: return "the server doesn't support the requested version of MQTT";
      break;
    case 2: return "the server rejected the client identifier";
      break;
    case 3: return "the server was unable to accept the connection";
      break;
    case 4: return "the username/password were rejected";
      break;
    case 5: return "the client was not authorized to connect";
      break;
  }
}
