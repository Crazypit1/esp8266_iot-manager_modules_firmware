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
  buttons_init();
  Serial.println("buttons_init");
  MQTT_init();
  Serial.println("MQTT_init");
  Push_init();
  Serial.println("Push_init");
  handleCMD_ticker();

#ifdef date_logging      
      addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Device started");
#endif

}


void loop()
{
  HTTP.handleClient();
#ifdef debug_mode_web_sokets
  webSocket.loop();
#endif
  handleMQTT();
  handleCMD_loop();
  handleButton();

  //Serial.println(GetTime());

  ts.update();
}
