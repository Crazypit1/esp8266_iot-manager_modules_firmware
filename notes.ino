/*

   список команд:

   rel 1 1
   pwm 1 100
   swi 1 1
   pus topic body



   а) события происходящии в любой момент времени (отрабатываются двумя командами)
   1. команда для инициализации она стандартна
   2. команда выполнения
   - по событию в строку order loop или order ticker добавляется команда, к примеру возьмем rel 1 1
   - фунция handleCMD_loop() или handleCMD_ticker() выполняет команды по очереди пока они есть
   - когда функция выполняет команды запускается void данной команды и внего подставляются данные команды (для rel 1 1 -> void relayControl()
   - так как некоторые данные необходимые для выполнения команды мы получаем во время инициализации то читаем их дополнительно из json строки
   и здесь данные соотносятся по номерам

   б) циклические события (отрабатываются одной командой, вызываемой через заданный промежуток времени)
   -здесь существует команда которая запускается через заданный промежуток времени scenario_update_int -> stringExecution(scenario) -> void handleScenario()
   -данные взятые из String scenario подставляются в void handleScenario() и сравниваются там с данными датчиков в итоге отправляются события

   в) события чтения сенсоров (отрабатываются одной командой только для инициализации)
   -команда для инициализации она стандартна и в ней запускается tascker чтения данных с данного датчика и данные тут же преобразовываются и отправляются

   г) события происходящие в любой момент времени, но имеющие прямую связь с отправкой (без order loop или order ticker)
   -произошло событие прихода в serial port тут же записывем что пришло и запускаем команду stringExecution(serial_text) -> void handleSerialEvent()
   -в ней сравнивается заданный текст и тот который пришел и если он совпадает то выполняем



*/

//!//->событие нажатия на кнопку в приложении: создается строка rel 1 1, выполняется функцией handleCMD(loop) (по номеру)
//!//->событие перемещения ползунка в приложении: создается строка pwm 1 50, выполняется функцией handleCMD(loop) (по номеру)
//!//->событие отправки пуш в любом месте кода: создается строка pus title body, выполняется функцией handleCMD(tiker) (не нужно соотносить)

//!//->событие нажатия на физическую кнопку: создается строка swi 1 1, выполняется функцией handleCMD(loop) (по номеру)
//!//->событие прихода чего то в сериал: записывет файл json, и вызывает handleSerialEvent() если == то... (нет идентификатора)


//{"id":"0","page":"Alerts","widget":"anydata","class1":"item rounded text-left no-padding","class2":"assertive-bg","style2":"font-size:15px;font-weight:bold","descr":"16.08.2019 20:56:20 Обнаружено движение","topic":"/IoTmanager/9139530-1458400/alert3","pageId":"3"}
//{"id":"1","page":"Alerts","widget":"anydata","class1":"item rounded text-left no-padding","class2":"assertive-bg","style2":"font-size:15px;font-weight:bold","descr":"16.08.2019 20:56:22 Обнаружено движение","topic":"/IoTmanager/9139530-1458400/alert3","pageId":"3"}
//{"id":"2","page":"Alerts","widget":"anydata","class1":"item rounded text-left no-padding","class2":"assertive-bg","style2":"font-size:15px;font-weight:bold","descr":"16.08.2019 20:56:23 Обнаружено движение","topic":"/IoTmanager/9139530-1458400/alert3","pageId":"3"}
//{"id":"3","page":"Alerts","widget":"anydata","class1":"item rounded text-left no-padding","class2":"assertive-bg","style2":"font-size:15px;font-weight:bold","descr":"16.08.2019 20:56:25 Обнаружено движение","topic":"/IoTmanager/9139530-1458400/alert3","pageId":"3"}
//{"id":"4","page":"Alerts","widget":"anydata","class1":"item rounded text-left no-padding","class2":"assertive-bg","style2":"font-size:15px;font-weight:bold","descr":"16.08.2019 20:56:26 Обнаружено движение","topic":"/IoTmanager/9139530-1458400/alert3","pageId":"3"}
