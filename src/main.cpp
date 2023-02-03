#include <WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

const char* ssid     = "POPLs";
const char* password = "12345678n";
char path[] = "/";
char host[] = "192.168.43.158:5000";
  
WebSocketClient webSocketClient;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);
  

  if (client.connect("192.168.43.158", 5000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }

  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }
}

void loop() {
  if (client.connected()) {
    String data;
    webSocketClient.getData(data);

    if (data.length() > 0) {
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, data);
      if (!error) {
        Serial.println("Received JSON: ");
        serializeJson(doc, Serial);
        Serial.println("");
      } else {
        Serial.println("Error parsing received JSON");
      }
    }

    StaticJsonDocument<200> doc;
    doc["message"] = "Hello from ESP32";
    String jsonString;
    serializeJson(doc, jsonString);
    webSocketClient.sendData(jsonString);
    
  } else {
    Serial.println("Client disconnected.");
    while (1) {
      // Hang on disconnect.
    }
  }

  delay(3000);
}
