#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined ESP32
#include <WiFi.h>
#endif
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
TickerScheduler ts(30);
enum { MQTT_WIFI, WIFI, LEVEL, ANALOG, PH, DALLAS, ANALOG_LOG, LEVEL_LOG, DALLAS_LOG, PH_LOG, CMD, TIMERS , TEST};
//---------------------------------------------------------
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
//---------------------------------------------------------
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
//---------------------------------------------------------
//#include <WiFiClientSecure.h>
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
GMedian medianFilter;
//---------------------------------------------------------
#define led_status
//---------------------------------------------------------
#define reconnecting_mqtt 60000
//---------------------------------------------------------
#define reconnecting_wifi 60000
//---------------------------------------------------------
#define scenario_update_int 5000
//---------------------------------------------------------
#define CMD_update_int 3000
//---------------------------------------------------------
#define analog_update_int 5000
//---------------------------------------------------------
#define ph_shooting_interval 500 //интервал выстрела датчика
#define ph_times_to_send 10      //после скольки выстрелов делать отправку данных
//---------------------------------------------------------
#define temp_update_int 5000
//---------------------------------------------------------
#define tank_level_shooting_interval 500 //интервал выстрела датчика
#define tank_level_times_to_send 20 //после скольки выстрелов делать отправку данных
//---------------------------------------------------------
//#define push_pushbullet
//#define push_onesignal
//---------------------------------------------------------
//#define date_logging



String chipID = "";
String prefix   = "/IoTmanager";
String prex;
String ids;

String configSetup = "{}";
String configJson = "{}";
String optionJson = "{}";
String valuesJson = "{}";

int port = 80;

String order_loop;
String order_timer = "";

String current_time;

String all_vigets = "";
String scenario;
String timers;

boolean busy;

boolean flagLoggingAnalog = false;
boolean flagLoggingLevel = false;
boolean flagLoggingDallas = false;
boolean flagLoggingPh = false;

boolean by_button;

int scenario_line_status [] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
