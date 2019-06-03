# esp8266 + Iot manager modules firmware
Это модульная прошивка для ESP8266 и приложения IoT Manager. Она позволяет быстро настроить умный дом на базе ESP8266 без программирования.

## Ссылка для скачивания прошивки: [esp8266_iot-manager_modules_firmware](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/releases/download/v1.0.7%2B/esp8266_iot-manager_modules_firmware.zip)

## Ссылка для скачивания IOT manager: [IOT manager](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/raw/master/iot_manager/IoT%20Manager%201.5.5.apk)


1. Модуль управления реле (можно управлять всеми пинами модуля)

2. Модуль физической кнопки (можно по нажатию отправлять команды включения/выключения реле на другие модули или на этот, а так же отправлять push уведомления)

3. Модуль измерения уровня в баке (поддержка дальномера HC-SR04 или водонепроницаемого JSN-SR04T v2.0) 

4. Модуль аналогового сенсора (можно задать диапазон входных значений и диапазон выходных значений, диапазон входных значений будет масштабироваться в диапазон выходных значений)

5. Модуль температурного сенсора ds18b20. Появляется красивый термометр в iot manager. 

6. Модуль управления ШИМ. В iot manager появятся ползунки которыми можно будет устанавливать значения.

Сценарии работают так что на любое пороговое значение любого сенсора можно назначить либо команду включения реле на другом устройстве либо на этом, либо можно назначить отправку пуш либо все сразу. И пуш и команду. 


Видео подробных инструкций:


[1.Как прошить модуль:](https://www.youtube.com/watch?v=octnqt9XBAs)

[2.Настройка подключения к серверу mqtt:](https://www.youtube.com/watch?v=SXgtQ0zh1RQ&t)

[3.Настройка модуля управления реле:](https://www.youtube.com/watch?v=1aIoAOH7Hms&t)

[4.Настройка Push Notification:](https://www.youtube.com/watch?v=8vzU5YEo9LE&t)

[5.Система измерения уровня воды в баке:](https://www.youtube.com/watch?v=RXWekYtgQ6Y&t)

[6.Как настроить сценарии:](https://www.youtube.com/watch?v=2v368psGFLc&t)