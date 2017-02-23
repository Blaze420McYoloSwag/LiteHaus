#ifndef PIXELRING_H
#define PIXELRING_H

#include "Adafruit_Neopixel.h"

class pixelRing: public Adafruit_NeoPixel {
    public:
        pixelRing(uint16_t numPixels, uint8_t pin, uint16_t type)
        : Adafruit_NeoPixel(numPixels,pin,type){};
        void ringShuffle();
        uint32_t dimStep(uint32_t color, int stepNum, int steps); //This reduces the inputted color
        void dimRing(int stepNum, int steps);
        uint8_t getRed(uint32_t color);
        uint8_t getBlue(uint32_t color);
        uint8_t getGreen(uint32_t color);
};

#endif
