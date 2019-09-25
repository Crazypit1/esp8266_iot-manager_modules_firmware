#include "set.h"

void setup() {

  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  prex = prefix + "/" + chipID;

  Serial.begin(115200);
  delay(5);
  Serial.println("");
  //--------------------------------------------------------------
  FS_init();
  Serial.println("FS_init");
  configSetup = readFile("config.json", 4096);
  jsonWrite(configJson, "SSDP", jsonRead(configSetup, "SSDP"));
  jsonWrite(configJson, "lang", jsonRead(configSetup, "lang"));
  //--------------------------------------------------------------
  CMD_init();
  Serial.println("CMD_init");
  //--------------------------------------------------------------
  HTTP_init();
  Serial.println("HTTP_init");
  //--------------------------------------------------------------
  buttons_init();
  Serial.println("buttons_init");
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
  // WebSoket_init ();
  // Serial.println("WebSoket_init");
  //--------------------------------------------------------------
  MQTT_init();
  Serial.println("MQTT_init");
  //--------------------------------------------------------------
  Push_init();
  Serial.println("Push_init");
  //--------------------------------------------------------------

#ifdef date_logging
  addFile("log.txt", GetDataDigital() + " " + GetTime() + "->Device started");
#endif

  order_timer = "1:60,2:120,3:200,";

  ts.add(TEST, 1000, [&](void*) {

    while (order_timer.length() != 0) {

      String tmp = selectToMarker (order_timer, ",");

      int num = selectFromMarkerToMarker(tmp, ":" , 0).toInt();
      int sec = selectFromMarkerToMarker(tmp, ":" , 1).toInt();
      sec--;
      
      order_timer_new += num;
      order_timer_new += ":";
      order_timer_new += sec;
      order_timer_new += ",";
      if (sec <= 0) order_timer_new = "";

      order_timer = deleteBeforeDelimiter(order_timer, ",");
      //Serial.println(order_timer);
    }

    order_timer = order_timer_new;
    order_timer_new = "";
    Serial.println(order_timer);

  }, nullptr, true);

}


void loop()
{
  HTTP.handleClient();

  //webSocket.loop();

  handleMQTT();
  handleCMD_loop();
  handleButton();

  handleScenario();

  //Serial.println(GetTime());

  ts.update();
}
