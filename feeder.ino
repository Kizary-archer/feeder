#include <WiFi.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define feedUpdated 0
#define doublePortion 1
#define feedingInterval 2
#define feedingCount 3
#define maxFeedingCount 4
#define lastFeedingTime 5
#define ServoAngle 6

WebSocketsServer webSocket = WebSocketsServer(8080);

void setup() {
  Serial.begin(115200);

  EEPROM.begin(20);
  WiFi.begin("dlink9321", "19311933af");
  Serial.print("Connecting");  //  "Подключение"
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  WifiInfo(); //IP,MASK,MAC

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void WifiInfo() {
  Serial.println();
  //ESP WiFi info
  uint8_t macAddr [ 6 ];
  WiFi.macAddress (macAddr);
  Serial.printf("Connected\nmac address: %02x:%02x:%02x:%02x:%02x:%02x \n" , macAddr [ 0 ], macAddr [ 1 ], macAddr [ 2 ], macAddr [ 3 ], macAddr [ 4 ], macAddr [ 5 ]);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet mask: ");
  Serial.println ( WiFi.subnetMask ());
}

void loop() {
  webSocket.loop();
  vTaskDelay (10);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        String InitSend = JsonInitSend();
        webSocket.sendTXT(num, InitSend);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      
      // send message to client
      // webSocket.sendTXT(num, "message here");
      break;
  }

}

void JsonEvent(String event,byte dat){
  
    if (event == "startFeeding") 
    else if (event == "feedUpdated") {ESPSerial.println(modeUpdate(oper_mode));reStart();}
    else if (event == "doublePortion") ESPSerial.println(modeUpdate(analize_mode));
    else if (event == "feedingInterval") resetFunc();

}

String JsonInitSend()
{
  DynamicJsonDocument JsonInit(200);

  JsonInit["event"] = "init";

  JsonObject data = JsonInit.createNestedObject("data");
  data["microcontroller"] = "feeder";
  data["doublePortion"] = 1;
  data["feedingInterval"] = 10;
  data["feedingCount"] = 4;
  data["maxFeedingCount"] = 10;
  data["lastFeedingTime"] = 14;
  data["type"] = "pf";

  serializeJsonPretty(JsonInit, Serial);
  String output;
  serializeJson(JsonInit, output);
  return (output);
}