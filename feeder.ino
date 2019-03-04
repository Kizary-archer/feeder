#include <WiFi.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

//EEPROM
#define ServoAngle 0  //угол поворота сервопривода
#define doublePortion 1 //двойная порция
#define feedingInterval 2 //интервал кормления
#define feedingCount 3 //используется для сброса значений при обновлении корма
#define maxFeedingCount 4 //колл.порций
#define lastFeedingTime 5 //последнее время кормления
//Settings
#define servoPin 18
Servo servo;

WebSocketsServer webSocket = WebSocketsServer(8080);

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);      // Standard 50hz servo
  servo.attach(servoPin, 500, 2400);
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
    case WStype_ERROR:
      Serial.printf("[%u] ERROR!\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
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
      DynamicJsonDocument JsonEvent(20);
      deserializeJson(JsonEvent, payload);
      Serial.print(JsonEvent["event"].as<char*>());
      String event = JsonEvent["event"];
      bool dat =  JsonEvent["data"];
      jsonEvent(event, dat);

      // send message to client
      // webSocket.sendTXT(num, "message here");
      break;
  }

}

void jsonEvent(String event, bool dat) {

  if (event == "startFeeding") {byte pos = 90+(EEPROM.read(doublePortion)*90);ServoMotor(pos);}
  else if (event == "feedUpdated") EEPROM.write(feedingCount, 0);
  else if (event == "doublePortion") EEPROM.write(doublePortion, dat);
  else if (event == "feedingInterval") Serial.println("установка интервала");
  EEPROM.commit();
  // EEPROM.write(ServoAngle,90 + (EEPROM.read(doublePortion)*90))

}

String JsonInitSend()
{
  DynamicJsonDocument JsonInit(200);

  JsonInit["event"] = "init";

  JsonObject data = JsonInit.createNestedObject("data");
  data["microcontroller"] = "feeder";
  data["doublePortion"] = EEPROM.read(doublePortion);
  data["feedingInterval"] = 2;
  data["feedingCount"] = EEPROM.read(feedingCount);
  data["maxFeedingCount"] = 20;
  data["lastFeedingTime"] = 1551710602278;
  data["type"] = "pf";

  serializeJsonPretty(JsonInit, Serial);
  String output;
  serializeJson(JsonInit, output);
  return (output);
}
void ServoMotor(byte pos) {
  Serial.println(pos);
  servo.write(pos);
}
