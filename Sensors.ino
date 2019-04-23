//=====================================================ОТПРАВЛЯЕМ ДАННЫЕ СЕНСОРОВ В ВИДЖЕТЫ ПРИ ОБНОВЛЕНИИ ДАННЫХ========================================================
void Sensors_init() {
  tank_level_();
  analog_();
  ds18b20_();

  scenario_();
}
//========================================Модуль измерения уровня в баке==============================================
void  tank_level_() {
  if (jsonRead(configSetup, "module_tank_level") == "1") {
    ts.add(LEVEL, sensors_update_int, [&](void*) {
      if (client.connected()) {
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
          Serial.println("sensor tank level send date");
        }
        distance_cm_old = distance_cm;
      }
    }, nullptr, true);
  } else {
    ts.disable(LEVEL);
  }
}
//===========================================Модуль аналогового сенсора========================================
void analog_() {
  if (jsonRead(configSetup, "module_analog") == "1") {
    ts.add(ANALOG, sensors_update_int, [&](void*) {
      if (client.connected()) {

        int analog = analogRead(A0);
        jsonWrite(optionJson, "analog", analog);
        static int analog_old;

        if (analog_old != analog) {

          int analog_out = map(analog, jsonReadtoInt(optionJson, "start_value"), jsonReadtoInt(optionJson, "end_value"), jsonReadtoInt(optionJson, "start_value_out"), jsonReadtoInt(optionJson, "end_value_out"));
          sendSTATUS("ana", String(analog_out));
          jsonWrite(configJson, "ana", analog_out);
          Serial.println("sensor analog send date");
        }
        analog_old = analog;
      }
    }, nullptr, true);
  } else {
    ts.disable(ANALOG);
  }
}
//=========================================Модуль температурного сенсора ds18b20============================================================
void ds18b20_() {
  if (jsonRead(configSetup, "module_ds18b20") == "1") {
    ts.add(DS18B20, sensors_update_int, [&](void*) {
      if (client.connected()) {

        float temp = 0;
        sensors.requestTemperatures();
        temp = sensors.getTempCByIndex(0);
        jsonWrite(optionJson, "ds18b20", String(temp));
        static float temp_old;

        if (temp_old != temp) {
   
          sendSTATUS("DS", String(temp));
          jsonWrite(configJson, "DS", String(temp));
          Serial.println("sensor ds18b20 send date");
        }
        temp_old = temp;
      }
    }, nullptr, true);
  } else {
    ts.disable(DS18B20);
  }
}
//============================================Сценарии для всех модулей==============================================
void scenario_() {
  if (jsonRead(configSetup, "scenario") == "1") {
    ts.add(SCENARIO, scenario_update_int, [&](void*) {
      if (client.connected()) {
        static boolean flag = false;
        if (flag) stringExecution(scenario);
        flag = true;
        Serial.println("scenario send date");
      }
    }, nullptr, true);
  } else {
    ts.disable(SCENARIO);
  }
}






