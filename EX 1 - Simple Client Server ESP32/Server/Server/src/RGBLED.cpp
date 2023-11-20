#include <Arduino.h>
#include "RGBLED.h"

RGBLED::RGBLED(int freq, int redChannel, int greenChannel, int blueChannel, int resolution, int redPin, int greenPin, int bluePin):freq(freq),
redChannel(redChannel),
greenChannel(greenChannel),
blueChannel(blueChannel),
resolution(resolution),
redPin(redPin),
greenPin(greenPin),
bluePin(bluePin)
{
    ledcSetup(redChannel, freq, resolution);
    ledcSetup(greenChannel, freq, resolution);
    ledcSetup(blueChannel, freq, resolution);

    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
}

void RGBLED::applyColor(String colorCode) {
    this->color = colorCode;

    this->redVal = hexToDecimal(colorCode.substring(0, 2));
    this->greenVal = hexToDecimal(colorCode.substring(2, 4));
    this->blueVal = hexToDecimal(colorCode.substring(4, 6));

    ledcWrite(redChannel, this->redVal);
    ledcWrite(greenChannel, this->greenVal);
    ledcWrite(blueChannel, this->blueVal);
}

uint32_t RGBLED::hexToDecimal(String hexString) {
    int decimalValue = 0;
    for (int i = 0; i < hexString.length(); i++) {
        char c = hexString.charAt(i);
        int digit;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            digit = 10 + (c - 'A');
        } else if (c >= 'a' && c <= 'f') {
            digit = 10 + (c - 'a');
        } else {
            return -1;
        }
        decimalValue = decimalValue * 16 + digit;
    }
  return decimalValue;
}

String RGBLED::getColor() {
    return this->color;
}