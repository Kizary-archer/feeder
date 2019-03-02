#include <WiFi.h>

void setup() {

  Serial.begin(115200);
  WiFi.begin("dlink9321", "19311933af");

  Serial.print("Connecting");  //  "Подключение"
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
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
    
}
}
