#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "RGBLED.h"

#define WIFI_SSID "ESPServer"
#define WIFI_PASSWORD "12345678"

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

const int redPin = 12;     
const int greenPin = 13;   
const int bluePin = 14;

RGBLED rgbLED(freq, redChannel, greenChannel, blueChannel, resolution, redPin, greenPin, bluePin);

AsyncWebServer server(80);

void onRequest(AsyncWebServerRequest *request) {
}

void onFileUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  

    
    // Serial.printf("body=%s\n", (char*) data);
    
    // request->send(200, "text/plain", "Ok");
}

void setup() {
  Serial.begin(9600);

  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  IPAddress ipAddress = WiFi.softAPIP();
  Serial.print("IP Address: ");
  Serial.println(ipAddress);

  server.on("/", HTTP_POST, onRequest, onFileUpload, onBody);

  server.begin();

}

void loop() {
  
}