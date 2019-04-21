//=====================================================ОТПРАВЛЯЕМ ДАННЫЕ СЕНСОРОВ В ВИДЖЕТЫ ПРИ ОБНОВЛЕНИИ ДАННЫХ========================================================
void Sensors_init() {

  ts.add(6, 10000, [&](void*) {

    long st_time = millis();

    if (client.connected()) {

      //========================================Модуль измерения уровня в баке==============================================
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
          Serial.print("sensor 1 send date");
        }
        distance_cm_old = distance_cm;
      }
      //===========================================Модуль аналогового сенсора========================================
      if (jsonRead(configSetup, "module_analog") == "1") {

        int analog = analogRead(A0);
        jsonWrite(optionJson, "analog", analog);
        static int analog_old;

        if (analog_old != analog) {

          int analog_out = map(analog, jsonReadtoInt(optionJson, "start_value"), jsonReadtoInt(optionJson, "end_value"), jsonReadtoInt(optionJson, "start_value_out"), jsonReadtoInt(optionJson, "end_value_out"));
          sendSTATUS("ana", String(analog_out));
          jsonWrite(configJson, "ana", analog_out);
          Serial.print("sensor 2 send date");
        }
        analog_old = analog;
      }
      //============================================Сценарии для всех модулей==============================================
      
      if (jsonRead(configSetup, "scenario") == "1") stringExecution(scenario);

    }

    long end_time = millis();
    Serial.println(" sensors total timeout = " + String(end_time - st_time));

  }, nullptr, true);
}
