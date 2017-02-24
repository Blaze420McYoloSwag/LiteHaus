#include "captouch.h"
#include "FastLED.h"

#define RING_PIN    3
#define RING_LEDS    64
CRGB ringLeds[RING_LEDS];

#define STRIP_PIN    3
#define STRIP_LEDS    64
CRGB stripLeds[STRIP_LEDS];

CRGB red = CHSV( 0, 0, 0);
CRGB green  = CHSV( 85, 0, 0);
CRGB blue  = CHSV( 150, 0, 0);
CRGB beaconColor = CHSV( 0, 0, 0);
CRGB baseColor = CHSV( 0, 0, 0);

uint8_t  colors = 3;
uint32_t currentPalette =  [red,  green,  blue];


CapTouch::Event touchEvent;


uint8_t brightness = 254; //Global brightness assignment


void setup() {
	delay(3000); // 3 second delay for recovery
	Particle.subscribe("LampUpdate", gotColorUpdate, MY_DEVICES);
	Touch.setup();
  	FastLED.addLeds<NEOPIXEL, RING_PIN>(ringLeds, RING_LEDS);
	FastLED.addLeds<NEOPIXEL, STRIP_PIN>(stripLeds, STRIP_LEDS);

  	// set master brightness control
  	FastLED.setBrightness(brightness);
}

  
void loop()
{	
	touchEvent = Touch.getEvent();
  	if (touchEvent == CapTouch::TouchEvent) {
		whileTouching();
	}
	update();
	FastLED.show();
}

void gotColorUpdate(const char *name, const char *data) {

	String str = String(data);
    char strBuffer[40] = "";
    str.toCharArray(strBuffer, 40);
    colorFromID = strtok(strBuffer, "~");
    baseColor = atof(strtok(NULL, "~"));

    // DEBUG
    String sColorRecieved = String(baseColor);
    Particle.publish("Color_Recieved", System.deviceID() + "~" + sColorRecieved);
}

void whileTouching() {
	uint8_t colorIndex = 0;
	uint8_t satBright = 20;
	CRGB color = red;

    while (touchEvent != CapTouch::ReleaseEvent) {
		satBright += 5;
		color.saturation = satBright;
		color.brightness = satBright;
		if(satBright >= 255){
			satBright = 20;
			colorIndex +=1;
			color = currentPalette[colorIndex % colors];		
		}        
	}
	uint8_t pixelsLit = map(satBright,0,255,0,STRIP_LEDS+1);
	for int(i = 0; i <= pixelsLit && i <= STRIP_LEDS;i++){
		stripLeds[i] = color;
	}
	if(pixelsLit > STRIP_LEDS){
		fill_solid(&ringLeds, RING_LEDS, color); //at max capacity, light up the whole beacon
	}
	fastLED.show();
	beaconColor = color;
	String sColor = String(beaconColor);
	Particle.publish("colorUpdate", System.deviceID() + "~" + sColor, 60, PRIVATE);
}
