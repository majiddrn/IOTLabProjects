#include <Arduino.h>

class RGBLED
{
private:
    int freq;
    int redChannel;
    int greenChannel;
    int blueChannel;
    int resolution;

    int redPin;     
    int greenPin;   
    int bluePin;

    uint32_t redVal;
    uint32_t greenVal;
    uint32_t blueVal;

    String color;

    uint32_t hexToDecimal(String);
public:
    RGBLED(int, int ,int ,int, int, int, int, int);
    ~RGBLED(){};

    void applyColor(String);
    String getColor();
};