
void HTTP_init(void) {

  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  
  // ------------------Выполнение команды из запроса
  HTTP.on("/cmd", HTTP_GET, []() {             //http://192.168.88.45/cmd?command=rel 1 1
    String com = HTTP.arg("command");
    //Serial.println(com);
    
    sCmd.readStr(com);
    HTTP.send(200, "text / plain", "OK");
    //Serial.println(statusS);
  });
  // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, []() {

    HTTP.send(200, "application/json", configJson);  //config.live.json - динамические данные
  });
  // --------------------Выдаем данные optionJson
  HTTP.on("/config.option.json", HTTP_GET, []() {

    HTTP.send(200, "application/json", optionJson);  //config.option.json - данные не являющиеся событиями
  });
  // -------------------Выдаем данные configSetup
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configSetup); //config.setup.json - для хранения постоянных данных
  });
  // --------------------Выдаем данные all_vigets
  HTTP.on("/all.vigets.json", HTTP_GET, []() {

    HTTP.send(200, "text / plain", all_vigets);
  });
  
  // -------------------Выдаем данные configSetup
  HTTP.on("/restart", HTTP_GET, []() {
    String restart = HTTP.arg("device");          // Получаем значение device из запроса
    if (restart == "ok") {                         // Если значение равно Ок
      HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
#ifdef date_logging
      addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Device restarted by user");
#endif 
      ESP.restart();                                // перезагружаем модуль
    } else {                                        // иначе
      HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
    }
  });
  // Добавляем функцию Update для перезаписи прошивки по WiFi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Запускаем HTTP сервер
  HTTP.begin();
}
