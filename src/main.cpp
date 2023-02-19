#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "POPLs";
const char* password = "infopops";
const char* host = "192.168.43.158"; // IP address of the host to connect to
int port = 5000; // port to use for the websocket connection

int ThermistorPin = 34, SwPin = 33, FlamePin = 32, FanPin = 23, LightPin = 22;
float Vo;
float R1 = 10000.0;
float temprature, Tc, in_put_vol;

WiFiClient client;
WebSocketsClient webSocket;

float adc(float bins){
	return (3.3/4095.0) * bins;
}

float temp_fun(){
  Vo = analogRead(ThermistorPin);
  // Measured
  //Calculating Temprature
  in_put_vol = (float) adc(Vo);
  temprature =  (4050.0 / (12.42 - log(in_put_vol)));
  Tc = temprature - 273.0;
  Serial.print("Temprature in celisus: ");
  Serial.println(Tc);
  return Tc;
}

void fan(){
  if (temp_fun() > 30){
    digitalWrite( FanPin, HIGH);
  }
  else{
    digitalWrite( FanPin, LOW);
  }
}

void switcher(){
  if (digitalRead(SwPin)){
    digitalWrite(LightPin, HIGH);
  }
  else{
    digitalWrite(LightPin, LOW);
  }
}

void flame_fun(){
  if (digitalRead(FlamePin)){
    Serial.println("there is flame");
  }
  else{
    Serial.println("there is flame");
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected");
      break;
    case WStype_TEXT:
      StaticJsonDocument<200> doc;                    // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        // JSON string was received correctly, so information can be retrieved:
        const char* message = doc["message"];
        Serial.println("Received user: ");
        Serial.println("message: " + String(message));

      }
      Serial.println("");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  pinMode(LightPin, OUTPUT);
  pinMode(FanPin, OUTPUT);
  pinMode(FlamePin, INPUT);
  pinMode(SwPin, INPUT);
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  webSocket.begin(host, port, "/");
  webSocket.onEvent(webSocketEvent);

  while (webSocket.isConnected()) {
    Serial.println("Connecting to websocket server...");
    delay(1000);
    webSocket.begin(host, port, "/");
    webSocket.onEvent(webSocketEvent);
  }

  Serial.println("Websocket connected");
}

void loop() {
  webSocket.loop();

  fan();
  switcher();
  flame_fun();

  if (webSocket.isConnected()) {
    Serial.println("Sending data...");
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["message"] = random(100);                    // write data into the JSON object -> I used "rand1" and "rand2" here, but you can use anything else
    serializeJson(doc, jsonString);                   // convert JSON object to string
    // Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
    webSocket.sendTXT(jsonString);               // send JSON string to clients
    delay(1000);
  }
}
