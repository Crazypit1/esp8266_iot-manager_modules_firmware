#ifdef push_onesignal

void push(String push_msg)
{
  WiFiClientSecure push_client;
  if (!push_client.connect("onesignal.com", 443)) return;
 
    String push_data = "{\"app_id\": \"8871958c-5f52-11e5-8f7a-c36f5770ade9\",\"include_player_ids\":[\"вот-тут-код-из-установленного-на-устройстве-ИОТманагера\"],\"android_group\":\"IoT Manager\",\"contents\": {\"en\": \"" + push_msg + "\"}}";
 
  push_client.println("POST /api/v1/notifications HTTP/1.1");
  push_client.print("Host:");
  push_client.println("onesignal.com");
  push_client.println("User-Agent: esp8266.Arduino.IoTmanager");
  push_client.print("Content-Length: ");
  push_client.println(push_data.length());
  push_client.println("Content-Type: application/json");
  push_client.println("Connection: close");
  push_client.println();
  push_client.println(push_data);
}

#endif
