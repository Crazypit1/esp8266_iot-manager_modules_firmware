void WIFI_init() {

  // --------------------Получаем SSDP со страницы
  HTTP.on("/ssid", HTTP_GET, []() {
    jsonWrite(configSetup, "ssid", HTTP.arg("ssid"));
    jsonWrite(configSetup, "password", HTTP.arg("password"));
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });
  // --------------------Получаем SSDP со страницы
  HTTP.on("/ssidap", HTTP_GET, []() {
    jsonWrite(configSetup, "ssidAP", HTTP.arg("ssidAP"));
    jsonWrite(configSetup, "passwordAP", HTTP.arg("passwordAP"));
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });


  // Попытка подключения к точке доступа
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  led_blink(2, 1, "on");
  byte tries = 15;
  String _ssid = jsonRead(configSetup, "ssid");
  String _password = jsonRead(configSetup, "password");
  WiFi.persistent(false);
  if (_ssid == "" && _password == "") {
    WiFi.begin();
  }
  else {
    WiFi.begin(_ssid.c_str(), _password.c_str());
  }
  // Делаем проверку подключения до тех пор пока счетчик tries
  // не станет равен нулю или не получим подключение
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    // Если не удалось подключиться запускаем в режиме AP
    Serial.println("");
    StartAPMode();

  }
  else {
    // Иначе удалось подключиться отправляем сообщение
    // о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
    led_blink(2, 1, "off");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    jsonWrite(configJson, "ip", WiFi.localIP().toString());
  }
}

bool StartAPMode() {
  Serial.println("WiFi up AP");
  IPAddress apIP(192, 168, 4, 1);
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  // Отключаем WIFI
  WiFi.disconnect();
  // Меняем режим на режим точки доступа
  WiFi.mode(WIFI_AP);
  // Задаем настройки сети
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  //Включаем DNS
  //dnsServer.start(53, "*", apIP);
  // Включаем WIFI в режиме точки доступа с именем и паролем
  // хронящихся в переменных _ssidAP _passwordAP
  String _ssidAP = jsonRead(configSetup, "ssidAP");
  String _passwordAP = jsonRead(configSetup, "passwordAP");
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  jsonWrite(configJson, "ip", apIP.toString());
  led_blink(2, 200, "on");


  ts.add(WIFI, reconnecting, [&](void*) {

    Serial.println("try find router");
    if (RouterFind(jsonRead(configSetup, "ssid"))) { 
      ts.remove(WIFI);
      WIFI_init();                                      //ESP.restart();
      MQTT_init();
    }
  }, nullptr, true);

  return true;
}



boolean RouterFind(String ssid) {

  int n = WiFi.scanComplete ();
  if (n == -2) {                       //Сканирование не было запущено, запускаем
    WiFi.scanNetworks (true, false);   //async, show_hidden
    return false;
  }
  if (n == -1) {                       //Сканирование все еще выполняется
    return false;
  }
  if (n > 0) {
    for (int i = 0; i <= n; i++) {
      if (WiFi.SSID (i) == ssid) {
        WiFi.scanDelete();
        return true;
      }
    }
    WiFi.scanDelete();
    return false;
  }
}

