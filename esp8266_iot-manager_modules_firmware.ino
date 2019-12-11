#include "set.h"

void setup() {

  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId());
  prex = prefix + "/" + chipID;

  Serial.begin(115200);
  delay(5);
  Serial.println("");
  //--------------------------------------------------------------
  FS_init();
  Serial.println("FS_init");
  configSetup = readFile("config.json", 4096);
  jsonWrite(configJson, ssdpS, jsonRead(configSetup, ssdpS));
  jsonWrite(configJson, "lang", jsonRead(configSetup, "lang"));
  //--------------------------------------------------------------
  CMD_init();
  Serial.println("CMD_init");
  //--------------------------------------------------------------
  HTTP_init();
  Serial.println("HTTP_init");
  //--------------------------------------------------------------
  All_init();
  Serial.println("All_init");
  //--------------------------------------------------------------
  WIFI_init();
  Serial.println("WIFI_init");
  //--------------------------------------------------------------
  Time_init();
  Serial.println("Time_init");
  //--------------------------------------------------------------
  SSDP_init();
  Serial.println("SSDP_init");
  //--------------------------------------------------------------
  //WebSoket_init ();
  //Serial.println("WebSoket_init");
  //--------------------------------------------------------------
  MQTT_init();
  Serial.println("MQTT_init");
  //--------------------------------------------------------------
  Push_init();
  Serial.println("Push_init");
  //--------------------------------------------------------------
  initUpgrade();
  Serial.println("initUpgrade");
  //--------------------------------------------------------------

  /*
    ts.add(29, 1000, [&](void*) {
    getMemoryLoad();
    }, nullptr, true);
  */

#ifdef date_logging
  addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Device started");
#endif

}


void loop()
{
  HTTP.handleClient();

  //webSocket.loop();

  handleMQTT();
  handleCMD_loop();
  handleButton();

  handleScenario();

  ts.update();
}
