#include <WiFi.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>

WebSocketsServer webSocket = WebSocketsServer(80);

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
  WifiInfo();

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
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    webSocket.loop();
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      // send message to client
      // webSocket.sendTXT(num, "message here");
      break;
  }

}

String JsonInitSend()
{
  StaticJsonDocument<200> Init;

  Init["microcontroller"] = "feeder";
  Init["doublePortion"] = 1;
  Init["feedingInterval"] = 10;
  Init["feedingCount"] = "gps";
  Init["maxFeedingCount"] = 10;
  Init["lastFeedingTime"] = 14;
  Init["type"] = "pf";
  serializeJsonPretty(Init, Serial);
}
