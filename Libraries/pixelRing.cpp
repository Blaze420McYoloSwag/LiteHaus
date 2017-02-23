#include "pixelRing.h"
#include "Adafruit_NeoPixel.h"

pixelRing::pixelRing(uint16_t numPixels, uint8_t pin, uint16_t type)
: Adafruit_NeoPixel(numPixels,pin,type){};

void pixelRing :: ringShuffle(){
    uint32_t zeroColor = getPixelColor(0);
    for (int i = 0; i < numPixels()-2; i++){
        setPixelColor(i,getPixelColor(i+1));
    }
    setPixelColor(numPixels()-1,zeroColor);
}
uint8_t pixelRing :: getRed(uint32_t c) { return (uint8_t)(c >> 16);}
uint8_t pixelRing :: getGreen(uint32_t c) { return (uint8_t)(c >>  8);}
uint8_t pixelRing :: getBlue(uint32_t c) { return (uint8_t)c;}

uint32_t pixelRing:: dimStep(uint32_t color, int stepNum, int steps ){ //returns a dimmed version
    uint8_t red = getRed(color);
    uint8_t green = getGreen(color);
    uint8_t blue = getBlue(color);
    uint8_t newRed = red - ((stepNum *red)/steps); 
    uint8_t newGreen = green - ((stepNum *green)/steps); 
    uint8_t newBlue = blue - ((stepNum *blue)/steps); 

    return Color(newRed,newGreen,newBlue);
}

void pixelRing :: dimRing (int stepNum, int steps){ //Dims the entire ring
    for (int i = 0; i < numPixels()-1;i++){
        setPixelColor(i,dimStep(getPixelColor(i),stepNum,steps));
    }
}
