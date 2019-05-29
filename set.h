#include <ESP8266WiFi.h>
//---------------------------------------------------------
#include <ESP8266WebServer.h>
ESP8266WebServer HTTP(80);
//---------------------------------------------------------
#include <ESP8266SSDP.h>
//---------------------------------------------------------
#include <FS.h>
File fsUploadFile;
//---------------------------------------------------------
#include <ArduinoJson.h>
//---------------------------------------------------------
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;
//#include <ESP8266HTTPClient.h>
//---------------------------------------------------------
//#include <DNSServer.h>
//DNSServer dnsServer;
//---------------------------------------------------------
//#include <SoftwareSerial.h>
//---------------------------------------------------------
#include <TickerScheduler.h>
TickerScheduler ts(10);
//---------------------------------------------------------
//#include <WebSocketsServer.h>
//WebSocketsServer webSocket = WebSocketsServer(81);
//#define debug_mode_web_sokets
//---------------------------------------------------------
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
//---------------------------------------------------------
#include <WiFiClientSecure.h>
//---------------------------------------------------------
#include <StringCommand.h>
StringCommand sCmd;
//---------------------------------------------------------
//#include "InputDebounce.h"
//---------------------------------------------------------
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire *oneWire;
DallasTemperature sensors;
//---------------------------------------------------------
#include <Bounce2.h>                 
#define NUM_BUTTONS 6
boolean but[NUM_BUTTONS];
Bounce * buttons = new Bounce[NUM_BUTTONS];
//---------------------------------------------------------
#include "GyverFilters.h" //настраивается в GyverHacks.h - MEDIAN_FILTER_SIZE
GMedian testFilter;
//---------------------------------------------------------
#define reconnecting 60000
#define scenario_update_int 20000
#define push_update_int 10000
//---------------------------------------------------------
#define tank_level_shooting_interval 500 //интервал выстрела датчика
#define tank_level_times_to_send 20 //после скольки выстрелов делать отправку данных
//---------------------------------------------------------
#define analog_update_int 20000
//---------------------------------------------------------
#define temp_update_int 20000
//---------------------------------------------------------
enum { MQTT_WIFI, WIFI, LEVEL, ANALOG, DS18B20, SCENARIO, PUSH , ULTRASONIC, TEST};

String chipID = "";
String prefix   = "/IoTmanager";
String prex;
String conStatus = "";
String ids;
String configSetup = "{}";
String configJson = "{}";
String optionJson = "{}";

int port = 80;

String order_main;
String order_switch;
String order_push;

String all_vigets = "";
String scenario;

boolean busy;



