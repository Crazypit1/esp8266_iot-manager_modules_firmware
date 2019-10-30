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
    MQTT_Connecting();

    String tmp = "{}";
    jsonWrite(tmp, "title", "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>" + stateMQTT());
    jsonWrite(tmp, "class", "pop-up");

    HTTP.send(200, "application/json", tmp);
  });

  jsonWrite(configSetup, "chipID", chipID);

  //проверка подключения к серверу
  ts.add(MQTT_WIFI, reconnecting_mqtt, [&](void*) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("->WiFi-ok");
      if (client.connected()) {
        Serial.println("->MQTT-ok");
#ifdef led_status
        led_blink(2, 20, "off");
#endif
      } else {
        MQTT_Connecting();
      }
    } else {
      Serial.println("->Lost WiFi connection");
#ifdef date_logging
      addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Lost WiFi connection");
#endif
      ts.remove(MQTT_WIFI);
      StartAPMode();
    }
  }, nullptr, true);
}

//================================================ОБНОВЛЕНИЕ====================================================

void  handleMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connected()) {
      client.loop();
    }
  }
}
//===============================================ПОДКЛЮЧЕНИЕ========================================================
void MQTT_Connecting() {

  String mqtt_server = jsonRead(configSetup, "mqttServer");

  if ((mqtt_server != "")) {
#ifdef led_status
    led_blink(2, 20, "on");
#endif
    Serial.println("->Lost MQTT connection, start reconnecting");
#ifdef date_logging
    addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Lost MQTT connection, start reconnecting");
#endif
    client.setServer(mqtt_server.c_str(), jsonReadtoInt(configSetup, "mqttPort"));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        Serial.println("->Connecting to MQTT server commenced");

        busy = true;

        if (client.connect(chipID.c_str(), jsonRead(configSetup, "mqttUser").c_str(), jsonRead(configSetup, "mqttPass").c_str())) {
#ifdef led_status
          led_blink(2, 20, "off");
#endif
          Serial.println("->MQTT connected");
#ifdef date_logging
          addFile("log.txt", GetDataDigital() + " " + GetTime() + "->MQTT connected");
#endif

          client.setCallback(callback);

          client.subscribe(prefix.c_str());  // Для приема получения HELLOW и подтверждения связи
          client.subscribe((prefix + "/" + chipID + "/+/control").c_str()); // Подписываемся на топики control
          //client.subscribe((prefix + "/" + chipID + "/test").c_str());  //Для приема получения work и подтверждения связи (для приложения mqtt IOT MQTT Panel)
          /* String tmp_line = id_of_other_device;

            while (tmp_line.length() != 0) {

             String id = selectToMarker(tmp_line, ",");  //2058631-1589487 1
             id = selectFromMarkerToMarker(id, " ", 0);
             client.subscribe((prefix + "/" + id + "/+/status").c_str(), 0);
             Serial.println("->subscribed to device, id: " + id);

             tmp_line = deleteBeforeDelimiter(tmp_line, ",");
            }*/

          client.subscribe((prefix + "/ids").c_str()); // Подписываемся на топики ids
          sendMQTT("test", "work");
          Serial.println("->Callback set, subscribe done");

          busy = false;

          outcoming_date(); //отправляем данные в виджеты

        } else {
          //Serial.println(stateMQTT());
          Serial.println("try again in " + String(reconnecting_mqtt / 1000) +  " sec");
#ifdef led_status
          led_blink(2, 20, "on");
#endif
        }
      }
    }
  } else {
    Serial.println("->No date for MQTT connection");
  }
}

//=====================================================ВХОДЯЩИЕ ДАННЫЕ========================================================

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print(topic);
  String topic_str = String(topic);

  String str;
  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
  }
  Serial.println(" -> " + str);
  if (str == "HELLO") outcoming_date();
  //if (str == "work") outcoming_date();     //Для приема получения work и подтверждения связи (для приложения mqtt IOT MQTT Panel)
  //превращает название топика в команду, а значение в параметр команды
  if (topic_str.indexOf("control") > 0) {                        //IoTmanager/800324-1458415/RelaySet1/control 1   /IoTmanager/9139530-1458400/RelaySet1/control -> 1
    //Serial.println(topic_str);
    String topic = selectFromMarkerToMarker(topic_str, "/", 3);  //RelaySet1
    String number = selectToMarkerLast(topic, "Set");            //1
    topic.replace(number, "");                                   //RelaySet
    String final_line = topic + " " + number + " " + str;        //RelaySet 1 1
    Serial.println(final_line);
    order_loop += final_line + ",";

  }
}

//данные которые отправляем при подключении или отбновлении страницы
void outcoming_date() {

  busy = true;

  sendAllWigets();
  sendAllData();

  if (flagLoggingAnalog) sendLogData("log.analog.txt", "loganalog");
  if (flagLoggingPh) sendLogData("log.ph.txt", "logph");
  if (flagLoggingDallas) sendLogData("log.dallas.txt", "logdallas");
  if (flagLoggingLevel) sendLogData("log.level.txt", "loglevel");

  Serial.println("->Sending all date to iot manager completed");

  busy = false;

}
//======================================CONFIG==================================================
///IoTmanager/2058631-1589487/config {----viget----}
///sendMQTT("config", data);
boolean sendMQTT(String end_of_topik, String data) {
  String topik = prefix + "/" + chipID + "/" + end_of_topik;
  boolean send_status = client.beginPublish(topik.c_str(), data.length(), false);
  client.print(data);
  client.endPublish();
  return send_status;
}
//======================================STATUS==================================================
///IoTmanager/2058631-1589487/rel1/status {"status":"1"}
///sendSTATUS(topic, state)
void sendSTATUS(String topik, String state) {
  topik = prefix + "/" + chipID + "/" + topik + "/" + "status";
  String json_ = "{}";
  jsonWrite(json_, "status", state);

  //long st_time = millis();
  int send_status =  client.publish (topik.c_str(), json_.c_str(), false);
  //long end_time = millis();
  //Serial.println("send status = " + String(send_status) + ", timeout = " + String(end_time - st_time));
}
/*void sendSTATUS(String topik, String state, String type, String param) {
  topik = prefix + "/" + chipID + "/" + topik + "/" + "status";
  String json_ = "{}";
  jsonWrite(json_, "status", state);
  jsonWrite(json_, type, param);

  //long st_time = millis();
  int send_status =  client.publish (topik.c_str(), json_.c_str(), false);
  //long end_time = millis();
  //Serial.println("send status = " + String(send_status) + ", timeout = " + String(end_time - st_time));
}*/
//======================================CONTROL==================================================
///IoTmanager/2058631-1589487/rel1/control 1
void sendCONTROL(String id, String topik, String state) {
  String  all_line = prefix + "/" + id + "/" + topik + "/control";

  //long st_time = millis();
  int send_status = client.publish (all_line.c_str(), state.c_str(), false);
  //long end_time = millis();
  //Serial.println("send control = " + String(send_status) + ", timeout = " + String(end_time - st_time));
}

//=====================================================ОТПРАВЛЯЕМ ВИДЖЕТЫ========================================================
void sendAllWigets() {

  int counter = 0;
  String line;
  int psn_1 = 0;
  int psn_2;

  do  {

    psn_2 = all_vigets.indexOf("\r\n", psn_1);
    line = all_vigets.substring(psn_1, psn_2);
    jsonWrite(line, "id", String(counter));
    counter++;
    sendMQTT("config", line);
    Serial.println(line);
    psn_1 = psn_2 + 1;

  } while (psn_2 + 2 < all_vigets.length());

  getMemoryLoad("->after send all vigets");
}

//=====================================================ОТПРАВЛЯЕМ ДАННЫЕ В ВИДЖЕТЫ ПРИ ОБНОВЛЕНИИ СТРАНИЦЫ========================================================
void sendAllDataTest() {

  String line;
  int psn_1 = 0;
  int psn_2;

  do  {
    psn_2 = configJson.indexOf(",", psn_1);
    line = configJson.substring(psn_1, psn_2);

    String topic =  selectToMarker (line, ":");
    topic.replace("{", "");
    topic.replace("}", "");
    topic.replace("\"", "");
    String state =  selectToMarkerLast (line, ":");
    state.replace("{", "");
    state.replace("}", "");
    state.replace("\"", "");
    if (topic != "SSDP" && topic != "lang" && topic != "ip" && topic.indexOf("_in") < 0) {
      sendSTATUS(topic, state);
      //Serial.println("-->" + topic);
    }

    psn_1 = psn_2 + 1;
  } while (psn_2 + 25 < configJson.length());

  getMemoryLoad("->after send all date");
}

//57
void sendAllData() {   //берет строку json и ключи превращает в топики а значения колючей в них посылает

  String current_config = configJson;                      //{"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1"}
  getMemoryLoad("->after send all date");
  current_config.replace("{", "");
  current_config.replace("}", "");                         //"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1"
  current_config += ",";                                   //"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1",

  while (current_config.length() != 0) {

    String tmp = selectToMarker (current_config, ",");
    String topic =  selectToMarker (tmp, ":");
    topic.replace("\"", "");
    String state =  selectToMarkerLast (tmp, ":");
    state.replace("\"", "");
    if (topic != "SSDP" && topic != "lang" && topic != "ip" && topic.indexOf("_in") < 0) {
      sendSTATUS(topic, state);
      Serial.println("-->" + topic + " " + state);
    }
    current_config = deleteBeforeDelimiter(current_config, ",");
  }
}

void sendLogData(String file, String topic) {

  String log_date = readFile(file, 5000) + "\r\n";
  getMemoryLoad("->after send log date");

  log_date.replace("\r\n", "\n");
  log_date.replace("\r", "\n");

  while (log_date.length() != 0) {
    String tmp = selectToMarker (log_date, "\n");

    //sendSTATUS(topic, selectFromMarkerToMarker(tmp, " ", 2));
    if (tmp != "") sendSTATUS(topic, tmp);

    log_date = deleteBeforeDelimiter(log_date, "\n");
  }
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
/*void scenario_devices_topiks_subscribe() {

  //SCENARIO ANALOG > 5 800324-1458415 rel1 0
  if (jsonRead(configSetup, "scenario") == "1") {
    //String all_text = readFile("scenario.all.txt", 1024) + "\r\n";
    String all_text = scenario + "\r\n";
    all_text.replace("\r\n", "\n");
    all_text.replace("\r", "\n");
    while (all_text.length() != 0) {
      String line_ = selectToMarker (all_text, "\n");
      String id = selectFromMarkerToMarker(line_, " ", 4);
      if (id != "not found") {
        client.subscribe((prefix + "/" + id + "/+/status").c_str(), 0);
        Serial.println("subscribed to device, id: " + id);
      }
      all_text = deleteBeforeDelimiter(all_text, "\n");
    }
  }
  }
*/
/*void scenario_devices_test_msg_send() {

  if (jsonRead(configSetup, "scenario") == "1") {

    String all_text = scenario + "\r\n";
    all_text.replace("\r\n", "\n");
    all_text.replace("\r", "\n");
    while (all_text.length() != 0) {
      String line_ = selectToMarker (all_text, "\n");
      String id = selectFromMarkerToMarker(line_, " ", 4);
      if (id != "not found") {
        //Serial.println();
        Serial.println(client.publish ((prefix + "/" + id).c_str(), "CHECK", true));

      }
      all_text = deleteBeforeDelimiter(all_text, "\n");
    }
  }
  }*/

/*
      //-----------------------------------------------------------------------------------------------------------------------------------------------
      //jsonWrite(tmp, "status", "1");

      String current_config = configJson;                  //{"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1"}
      current_config.replace("{", "");
      current_config.replace("}", "");                      //"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1"
      current_config += ",";                                //"SSDP":"MODULES","lang":"","ip":"192.168.43.60","DS":"34.00","rel1":"1","rel2":"1",

      while (current_config.length() != 0) {

        String tmp = selectToMarker (current_config, ",");  //"rel1":"1"
        String topic =  selectToMarker (tmp, ":");          //"rel1"
        topic.replace("\"", "");                            //rel1
        Serial.println(topic);
        String state =  selectToMarkerLast (tmp, ":");      //"1"
        state.replace("\"", "");                            //1

        //if (viget.lastIndexOf(topic) > 0) {
          jsonWrite(tmp, "status", state);
        //}
        current_config = deleteBeforeDelimiter(current_config, ",");
      }
     //-------------------------------------------------------------------------------------------------------------------------------------------------
*/
