# esp8266 + Iot manager modules firmware
Это модульная прошивка для ESP8266 и приложения IoT Manager. Она позволяет быстро настроить умный дом на базе ESP8266 без программирования.



## Ссылка для скачивания прошивки: [esp8266_iot-manager_modules_firmware](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/releases/download/v1.0.7%2B/esp8266_iot-manager_modules_firmware.zip)

## Ссылка для скачивания IOT manager:[IOT manager](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/raw/master/iot_manager/IoT%20Manager%201.5.5.apk)



После того как скачаете архив, нужно распаковать его в любое место. И запустить программу ESP8266Flasher.exe. Затем проверить что бы настройки соответствовали скриншоту. После прошивки модуль создаст сеть с названием "WiFi". Нужно подключиться к ней и перейти в браузере (google chrome) по адресу 192.168.4.1. Нажать на кнопку "КОНФИГУРАЦИЯ WIFI" и заполнить поля подключения к WIFI роутеру и поле имени устройства. После этого нужно нажать кнопку сохранить и перезагрузить модуль. Если модуль wemos то лучше перезагружать кнопкой на плате. После перезагрузки устройство подключится к wifi. Далее нужно будет перейти в сетевое окружение компьютера и там появится иконка устройства с данным ему именем. Нужно нажать на нее два раза и web интерфейс откроется снова. Следующим шагом нужно создать аккаунт на сайте https://cloudmqtt.com и ввести учетные данные mqtt в устройство и в приложение IoT Manager.


Здесь приведины видео инструкций:

[Часть 1 - Как прошить модуль esp8266 ](https://www.youtube.com/watch?v=octnqt9XBAs)

[Часть 2 - Настройка подключения к серверу MQTT (cloudmqtt) ](https://www.youtube.com/watch?v=SXgtQ0zh1RQ)

[Часть 3 - Настройка модуля управления реле ](https://www.youtube.com/watch?v=1aIoAOH7Hms&t)

[Часть 4 - Настройка push уведомлений ](https://www.youtube.com/watch?v=8vzU5YEo9LE)
