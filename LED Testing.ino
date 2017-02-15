
#include <neopixel.h> //neopixels duh
#include "application.h"

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

#define RING_COUNT 24
#define RING_PIN D1
#define RING_TYPE WS2812

#define PIXEL_COUNT 15
#define PIXEL_PIN D0
#define PIXEL_TYPE WS2812

uint32_t beaconArray[RING_COUNT + PIXEL_COUNT] = {0};
int lightCase = 0; //Case 0 is beacon, 1 is aurora, 2 is weather 


Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
pixelRing ring = pixelRing(RING_COUNT, RING_PIN, RING_TYPE);


void setup() {
	strip.begin();
	ring.begin();
	strip.show();
	ring.show();
	Particle.subscribe("LampUpdate", gotColorUpdate, MY_DEVICES);
}

void loop() {
	
	update();
	strip.show();
	ring.show();
}

void gotColorUpdate(const char *name, const char *data) {

	if(lightCase == 2){
		lightCase = 0;
	}
	else{
		lightCase++;
}

void update(){
		if(Time.now() - beaconUpdateDelay > beaconLastUpdate){
			updateBeacon();
			beaconLastUpdate = Time.now();
			}
		if(Time.now() - auroraUpdateDelay > auroraLastUpdate){
			updateAurora();
			auroraLastUpdate =Time.now();
			}
		if(Time.now() - weatherUpdateDelay > weatherLastUpdate){
			updateWeather();
			weatherLastUpdate =Time.now();
			}
			
}
