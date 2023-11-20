 #include <Arduino.h>
 #include <ESPAsyncWebServer.h>
#include <JSON.h>
#include "RGBLED.h"

#define WIFI_SSID "ESPServer"
#define WIFI_PASSWORD "12345678"

#define SEVEN_COLOR_LED 17

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

const int redPin = 12;     
const int greenPin = 13;   
const int bluePin = 14;

bool colorLEDOn = false;

RGBLED rgbLED(freq, redChannel, greenChannel, blueChannel, resolution, redPin, greenPin, bluePin);

AsyncWebServer server(80);

bool isJSONValid(const char* jsonString) {
  int curlyBraceCount = 0;
  int squareBracketCount = 0;

  for (int i = 0; jsonString[i] != '\0'; i++) {
    char c = jsonString[i];

    if (c == '{') {
      curlyBraceCount++;
    } else if (c == '}') {
      curlyBraceCount--;
    } else if (c == '[') {
      squareBracketCount++;
    } else if (c == ']') {
      squareBracketCount--;
    }
  }

  return curlyBraceCount == 0 && squareBracketCount == 0;
}

void onRequest(AsyncWebServerRequest *request) {
}

void onFileUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  
    Serial.println("New input:");
    Serial.println((char*)data);

    if (!isJSONValid((char*)data)) {
      Serial.println("Parsing input failed!");
      request->send(400, "text/plain", "JSON Problem");
      return;
    }

    JSONVar json = JSON.parse((char*)data);

    String colorCodeRGB = json["RGBLED"];
    String sevenColorLED = json["ColorLED"];

    sevenColorLED.trim();
    colorCodeRGB.trim();

    if (sevenColorLED.indexOf("ON") >= 0) {
      Serial.println("Output LED: ON");
      digitalWrite(SEVEN_COLOR_LED, HIGH);
    } else {
      Serial.println("Output LED: OFF");
      digitalWrite(SEVEN_COLOR_LED, LOW);
    }
    rgbLED.applyColor(colorCodeRGB);

    request->send(200, "text/plain", "Ok");
}

void setup() {
 Serial.begin(9600);

  pinMode(SEVEN_COLOR_LED, OUTPUT);
  digitalWrite(SEVEN_COLOR_LED, 0);

  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  IPAddress ipAddress = WiFi.softAPIP();
  Serial.print("IP Address: ");
  Serial.println(ipAddress);

  server.on("/", HTTP_POST, onRequest, onFileUpload, onBody);

  server.begin();

}

void loop() {
  
}