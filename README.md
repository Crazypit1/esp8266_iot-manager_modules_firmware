# esp8266 + Iot manager modules firmware
Это модульная прошивка для ESP8266 и приложения IOT Manager. Она позволяет быстро настроить умный дом на базе ESP8266 без программирования.



## Ссылка для скачивания прошивки: [esp8266_iot-manager_modules_firmware](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/releases/download/v1.0.3/esp8266_iot-manager_modules_firmware.zip)

![download](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/blob/master/data/tutorial/settings1.png)
![download](https://github.com/DmitryBorisenko33/esp8266_iot-manager_modules_firmware/blob/master/data/tutorial/settings2.png)

После того как скачаете архив, нужно распаковать его в любое место. И запустить программу ESP8266Flasher.exe. Затем проверить что бы настройки соответствовали скриншоту. После прошивки модуль создаст сеть с названием "WiFi". Нужно подключиться к ней и перейти в браузере (google chrome) по адресу 192.168.4.1. Нажать на кнопку "КОНФИГУРАЦИЯ WIFI" и заполнить поля подключения к WIFI роутеру и поле имени устройства. После этого нужно нажать кнопку сохранить и перезагрузить модуль. Если модуль wemos то лучше перезагружать кнопкой на плате. После перезагрузки устройство подключится к wifi. Далее надо будет перейти в сетевое окружение компьютера и там появится иконка устройства. Нужно нажать на нее два раза и web интерфейс откроется снова. Следующим шагом нужно создать аккаунт на сайте https://cloudmqtt.com и ввести учетные данные mqtt в устройство и в приложение IoT Manager.
