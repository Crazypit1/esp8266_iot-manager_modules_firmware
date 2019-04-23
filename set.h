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
//WiFiClientSecure push_client;
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
#define BUFFER_SIZE 100
#define reconnecting 60000
#define sensors_update_int 5000
#define scenario_update_int 5000

//---------------------------------------------------------
enum { MQTT_WIFI, WIFI, LEVEL, ANALOG, DS18B20, SCENARIO, TEST };

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
String all_vigets = "";
String scenario;

