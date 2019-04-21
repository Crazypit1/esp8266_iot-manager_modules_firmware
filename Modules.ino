void Modules_web_page_init() {
  if (jsonRead(configSetup, "scenario") == "1") scenario = readFile("scenario.all.txt", 1024);
  if (jsonRead(configSetup, "scenario") == "1") stringExecution(scenario);
  //==================================================================================================
  HTTP.on("/scenario", HTTP_GET, []() {
    jsonWrite(configSetup, "scenario", HTTP.arg("status"));
    saveConfig();
    if (jsonRead(configSetup, "scenario") == "1") scenario = readFile("scenario.all.txt", 1024);
    if (jsonRead(configSetup, "scenario") == "1") stringExecution(scenario);
    HTTP.send(200, "text/plain", "OK");
  });
  HTTP.on("/module_relay", HTTP_GET, []() {
    jsonWrite(configSetup, "module_relay", HTTP.arg("status"));
    saveConfig();
    All_Modules_init();
    //outcoming_date();
    HTTP.send(200, "text/plain", "OK");
  });
  HTTP.on("/module_tank_level", HTTP_GET, []() {
    jsonWrite(configSetup, "module_tank_level", HTTP.arg("status"));
    saveConfig();
    All_Modules_init();
    //outcoming_date();
    HTTP.send(200, "text/plain", "OK");
  });
  HTTP.on("/module_analog", HTTP_GET, []() {
    jsonWrite(configSetup, "module_analog", HTTP.arg("status"));
    saveConfig();
    All_Modules_init();
    //outcoming_date();
    HTTP.send(200, "text/plain", "OK");
  });
  //==================================================================================================
  HTTP.on("/all_modules_init", HTTP_GET, []() {
    All_Modules_init();
    outcoming_date();
    HTTP.send(200, "text/plain", "OK");
  });
}

void All_Modules_init() {
   
  all_vigets = "";
 
  if (jsonRead(configSetup, "module_relay") == "1") txtExecution("blok.relay.txt");
  if (jsonRead(configSetup, "module_tank_level") == "1") txtExecution("blok.tank.level.txt");
  if (jsonRead(configSetup, "module_analog") == "1") txtExecution("blok.analog.txt");
  
}
//====================================================================================================
void txtExecution(String file) {

  String command_all = readFile(file, 1024) + "\r\n";  //"\r\n"

  command_all.replace("\r\n", "\n");
  command_all.replace("\r", "\n");

  while (command_all.length() != 0) {

    String tmp = selectToMarker (command_all, "\n");
    sCmd.readStr(tmp);
    command_all = deleteBeforeDelimiter(command_all, "\n");
  }
}
void stringExecution(String str) {

  String command_all = str + "\r\n";  //"\r\n"

  command_all.replace("\r\n", "\n");
  command_all.replace("\r", "\n");

  while (command_all.length() != 0) {

    String tmp = selectToMarker (command_all, "\n");
    sCmd.readStr(tmp);
    command_all = deleteBeforeDelimiter(command_all, "\n");
  }
}


