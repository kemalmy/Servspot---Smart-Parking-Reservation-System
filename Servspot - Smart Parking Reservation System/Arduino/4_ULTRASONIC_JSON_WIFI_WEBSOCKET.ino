#include <WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

const char* ssid = "MIFI1";
const char* password = "RIFQIAHMADF";

WebSocketsServer webSocket = WebSocketsServer(81);

const int trigPins[] = {15, 4, 13, 14};
const int echoPins[] = {2, 16, 12, 27};
const int buzzerPins[] = {17, 0, 0, 0};

unsigned long lastTime = 0;
unsigned long interval = 500;

StaticJsonDocument<200> jsonDocument;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    pinMode(buzzerPins[i], OUTPUT);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  unsigned long currentTime = millis();

  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;

    jsonDocument.clear();
    JsonArray sensorReadings = jsonDocument.createNestedArray("sensorReadings");

    for (int i = 0; i < 4; i++) {
      measureDistance(trigPins[i], echoPins[i], buzzerPins[i], i + 1, sensorReadings);
    }

    String jsonString;
    serializeJson(jsonDocument, jsonString);
    webSocket.broadcastTXT(jsonString);
  }
}

void measureDistance(int trigPin, int echoPin, int buzzerPin, int sensorNumber, JsonArray& sensorReadings) {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delay(5);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  int beepDuration = map(distance, 0, 200, 200, 20); // Adjust the range as needed

  digitalWrite(buzzerPin, HIGH);
  delay(beepDuration);
  digitalWrite(buzzerPin, LOW);
  
  JsonObject sensorReading = sensorReadings.createNestedObject();
  sensorReading["sensorNumber"] = sensorNumber;
  sensorReading["distance"] = distance;
}
