#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "ESPServer"
#define WIFI_PASSWORD "12345678"

#define SERVER "http://192.168.4.1/"

#define TOUCH_SENSOR 17

unsigned long lastPressed = 0;

void setup() {
  Serial.begin(9600);

  pinMode(TOUCH_SENSOR, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  while (Serial.available()) {
    String code = Serial.readString();
    code.trim();
    if (code.length() == 6) {
      String json = "{\"RGBLED\":\"" + code + "\"}";

      HTTPClient http;

      http.begin(SERVER);
      http.addHeader("Content-Type", "application/json");

      int httpResponse = http.POST(json);

      Serial.println("Code: " + (String)httpResponse);

      http.end();
    } else {
      Serial.println("Not valid");
    }
  }
  

  if (digitalRead(TOUCH_SENSOR) == HIGH && millis() - lastPressed >= 300) {
    Serial.println("HIGH");

    HTTPClient http;

    http.begin(SERVER);
    http.addHeader("Content-Type", "application/json");
    String json = "{\"ColorLED\":\"ON\"}";

    int httpResponse = http.POST(json);

    Serial.println("Code: " + (String)httpResponse);

    http.end();

    lastPressed = millis();
  }
}