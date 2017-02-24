
#include <neopixel.h> //neopixels duh
#include "application.h"
#include "pixelRing.h"

#define RING_COUNT 24
#define RING_PIN D1
#define RING_TYPE WS2812

#define PIXEL_COUNT 15
#define PIXEL_PIN D0
#define PIXEL_TYPE WS2812

uint32_t beaconArray[RING_COUNT + PIXEL_COUNT] = {0};
uint32_t auroraArray[RING_COUNT + PIXEL_COUNT + 10] = {0}; //6 total colors, 

int lightCase = 0; //Case 0 is beacon, 1 is aurora, 2 is weather 
uint32_t colorReceived; //The color picked up


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

	String str = String(data);
    	char strBuffer[40] = "";
    	str.toCharArray(strBuffer, 40);
    	colorFromID = strtok(strBuffer, "~");
    	colorRecieved = atof(strtok(NULL, "~"));

    // DEBUG
    	String sColorRecieved = String(colorRecieved);
    	Particle.publish("Color_Recieved", System.deviceID() + "~" + sColorRecieved);
	colorChange();
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
void updateBeacon(){
	
	
