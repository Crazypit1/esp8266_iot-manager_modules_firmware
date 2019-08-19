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
#include <ESP8266HTTPClient.h>
//---------------------------------------------------------
//#include <DNSServer.h>
//DNSServer dnsServer;
//---------------------------------------------------------
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(4, 5); //  TX, RX
//---------------------------------------------------------
#include <TickerScheduler.h>
TickerScheduler ts(10);
enum { MQTT_WIFI, WIFI, LEVEL, ANALOG, DS18B20, SCENARIO, CMD, TIMERS };
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
#define led_status
//---------------------------------------------------------
#define reconnecting 60000
//---------------------------------------------------------
#define scenario_update_int 5000
//---------------------------------------------------------
#define CMD_update_int 3000   //for push
//---------------------------------------------------------
#define analog_update_int 5000
//---------------------------------------------------------
#define temp_update_int 5000
//---------------------------------------------------------
#define tank_level_shooting_interval 500 //интервал выстрела датчика
#define tank_level_times_to_send 20 //после скольки выстрелов делать отправку данных
//---------------------------------------------------------



String chipID = "";
String prefix   = "/IoTmanager";
String prex;
String conStatus = "";
String ids;

String configSetup = "{}";
String configJson = "{}";
String optionJson = "{}";

int port = 80;

String order_loop;
String order_ticker;

String current_time;

String all_vigets = "";
String scenario;
String timers;

boolean busy;
