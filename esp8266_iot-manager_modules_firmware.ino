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
#ifdef debug_mode_web_sokets
  WebSoket_init ();
  Serial.println("WebSoket_init");
#endif
  All_Modules_init();
  Serial.println("All_Modules_init");
  Modules_web_page_init();
  Serial.println("Modules_web_page_init");
  MQTT_init();
  Serial.println("MQTT_init");
  Sensors_init();
  Serial.println("Sensors_init");
  Push_init();
  Serial.println("Push_init");

  //ts.disable(TEST);
  //ts.enable(TEST);  

}


void loop()
{
  HTTP.handleClient();
#ifdef debug_mode_web_sokets
  webSocket.loop();
#endif
  handleMQTT();
  handleCMD();
  handleButton();
  handleSwitch();


  ts.update();
}
