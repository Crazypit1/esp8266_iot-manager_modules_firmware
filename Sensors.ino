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
    ts.add(LEVEL, tank_level_shooting_interval, [&](void*) {

      long duration_;
      int distance_cm;
      int level_persent;
      static int distance_cm_old; //переменная static сохраняет свое значение между вызовами функции
      static int counter;
      //-------------------------------------------------------------------------------------------
      digitalWrite(14, LOW);
      delayMicroseconds(2);
      digitalWrite(14, HIGH);
      delayMicroseconds(10);
      digitalWrite(14, LOW);
      duration_ = pulseIn(12, HIGH, 30000); // 3000 µs = 50cm // 30000 µs = 5 m
      distance_cm = duration_ / 29 / 2;
      //Serial.print(counter);
      //Serial.print("-");
      //Serial.print(distance_cm);
      //Serial.print("=>");
      distance_cm = testFilter.filtered(distance_cm);//отсечение промахов медианным фильтром
      //Serial.println(distance_cm);
#ifdef debug_mode_web_sokets
      SoketData("module_tank_level_s", distance_cm, 1);
#endif
      //--------------------------------------------------------------------------------------------
      counter++;

      if (counter > tank_level_times_to_send) {
        counter = 0;
        if (distance_cm_old != distance_cm) {

          int empty_level = jsonReadtoInt(optionJson, "empty_level");
          int full_level = jsonReadtoInt(optionJson, "full_level");

          jsonWrite(optionJson, "distance_cm", distance_cm);
          
          /*static boolean flag1 = true;
          static boolean flag2 = true;          
          if (distance_cm > empty_level || distance_cm < full_level) {

            if (flag1) add_viget("alerts", "TANK LEVEL SENSOR OUT OF RANGE!");
            outcoming_date();
            flag1 = false;
            flag2 = true;
          } else {
            if (flag2) del_viget("TANK LEVEL SENSOR OUT OF RANGE!");
            outcoming_date();
            flag1 = true;
            flag2 = false;
          }*/


          level_persent = map(distance_cm, empty_level, full_level, 0, 100);
          sendSTATUS("lev", String(level_persent));
          jsonWrite(configJson, "lev", level_persent);
          Serial.println("sensor tank level send date " + String(level_persent));

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
    ts.add(ANALOG, analog_update_int, [&](void*) {

      int analog = analogRead(A0);
      jsonWrite(optionJson, "analog", analog);
      static int analog_old;

#ifdef debug_mode_web_sokets
      SoketData("module_analog_s", analog, 1);
#endif

      if (analog_old != analog) {

        int analog_out = map(analog, jsonReadtoInt(optionJson, "start_value"), jsonReadtoInt(optionJson, "end_value"), jsonReadtoInt(optionJson, "start_value_out"), jsonReadtoInt(optionJson, "end_value_out"));
        sendSTATUS("ana", String(analog_out));
        jsonWrite(configJson, "ana", analog_out);
        Serial.println("sensor analog send date " + String(analog_out));

      }

      analog_old = analog;

    }, nullptr, true);
  } else {
    ts.disable(ANALOG);
  }
}
//=========================================Модуль температурного сенсора ds18b20============================================================
void ds18b20_() {
  if (jsonRead(configSetup, "module_ds18b20") == "1") {
    ts.add(DS18B20, temp_update_int, [&](void*) {

      float temp = 0;
      sensors.requestTemperatures();
      temp = sensors.getTempCByIndex(0);
      jsonWrite(optionJson, "ds18b20", String(temp));
      static float temp_old;

#ifdef debug_mode_web_sokets
      SoketData("module_ds18b20_s", String(temp), "1");
#endif

      if (temp_old != temp) {

        sendSTATUS("DS", String(temp));
        jsonWrite(configJson, "DS", String(temp));
        Serial.println("sensor ds18b20 send date " + String(temp));

      }

      temp_old = temp;

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

        Serial.println("scenario send date ");
      }
    }, nullptr, true);
  } else {
    ts.disable(SCENARIO);
  }
}






