#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
//#include <DNSServer.h>
//#include <SoftwareSerial.h>
#include <TickerScheduler.h>
#include <WebSocketsServer.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
//#include <WiFiClientSecure.h>
#include <StringCommand.h>
//#include "InputDebounce.h"

#define BUFFER_SIZE 100
#define reconnecting 60000

WebSocketsServer webSocket = WebSocketsServer(81);
TickerScheduler ts(10);
ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer HTTP(80);
File fsUploadFile;
WiFiClientSecure push_client;
StringCommand sCmd;
//DNSServer dnsServer;
WiFiClient espClient;
PubSubClient client(espClient);

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

