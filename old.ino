/*        if (client.connect(chipID.c_str()), jsonRead(configSetup, "mqttUser"), jsonRead(configSetup, "mqttPass")) {
          Serial.println("2->Connecting to MQTT server completed");

          //client.set_callback(callback);//запускаем ожидание входящих данных
          Serial.println("3->callback set");

          client.subscribe(prefix.c_str());  // Для приема получения HELLOW и подтверждения связи
          client.subscribe((prefix + "/" + chipID + "/+/control").c_str()); // Подписываемся на топики control
          client.subscribe((prefix + "/ids").c_str()); // Подписываемся на топики ids
          Serial.println("4->subscribe done");

          outcoming_date(); //отправляем данные в виджеты

          Serial.println("5->Sending all date to iot manager completed");

          led_blink(2, 20, "off");

        } else {
          Serial.println("->Could not connect to MQTT server");
        }*/







