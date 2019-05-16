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
//---------------------------------------------------------
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
//---------------------------------------------------------
//#include <WiFiClientSecure.h>
//WiFiClientSecure client_push;
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
#define reconnecting 60000
#define sensors_update_int 20000
#define scenario_update_int 20000
#define push_update_int 10000
//---------------------------------------------------------
enum { MQTT_WIFI, WIFI, LEVEL, ANALOG, DS18B20, SCENARIO, PUSH , TEST};

String chipID = "";
String prefix   = "/IoTmanager";
String prex;
String conStatus = "";
String ids;
String configSetup = "{}";
String configJson = "{}";
String optionJson = "{}";

int port = 80;
String order;
String order_switch;
String order_push;

String all_vigets = "";
String scenario;

boolean busy;

//------------------------------------------------------------------
const char* host = "api.github.com";
const int httpsPort = 443;
const char* accessToken = "o.qjOvmrc1wjGOGs20jir3G5kpFVL52hRa";
const char* fingerprint = "TODO";
//------------------------------------------------------------------
