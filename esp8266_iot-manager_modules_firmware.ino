#include "set.h"

void setup() {

  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  prex = prefix + "/" + chipID;

  Serial.begin(115200);
  delay(5);
  Serial.println("");
  FS_init();
  Serial.println("FS_init");
  configSetup = readFile("config.json", 4096);
  jsonWrite(configJson, "SSDP", jsonRead(configSetup, "SSDP"));
  jsonWrite(configJson, "lang", jsonRead(configSetup, "lang"));
  Serial.println(configSetup);
  Serial.println("FileConfig");
  WIFI_init();
  Serial.println("WIFI_init");
  Time_init();
  Serial.println("Time_init");
  SSDP_init();
  Serial.println("SSDP_init");
  HTTP_init();
  Serial.println("HTTP_init");
  CMD_init();
  Serial.println("CMD_init");
  //WebSoket_init ();
  //Serial.println("WebSoket_init");
  All_Modules_init();
  Serial.println("All_Modules_init");
  Modules_web_page_init();
  Serial.println("Modules_web_page_init");
  MQTT_init();
  Serial.println("MQTT_init");
  Sensors_init();
  Serial.println("Sensors_init");
  
  test();

  //ts.disable(TEST);
  //ts.enable(TEST);
}


void loop()
{
  HTTP.handleClient();
  //  webSocket.loop();
  handleMQTT();
  handleCMD();


  ts.update();
}
