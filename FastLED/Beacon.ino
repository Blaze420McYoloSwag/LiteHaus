// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

//The FastLED variables
#define RING_PIN    D1
#define RING_LEDS    24
CRGB ringLeds[RING_LEDS];
#define STRIP_PIN    D0
#define STRIP_LEDS    15
CRGB stripLeds[STRIP_LEDS];

//The fastLED color declarations
const int  colors = 3;
CHSV red = CHSV(0, 0 , 0);
CHSV green  = CHSV(85, 0 , 0);
CHSV blue  = CHSV(171, 0 , 0);
CHSV currentPalette[colors] =  {red, green, blue};

CHSV beaconColor; //The color beacon sent
CHSV baseColor;  //The beacon color received



char beaconString[40]; //the string being published

uint8_t brightness = 254; //Global brightness assignment
int button = D3; //The push button pin


void setup() {
    pinMode(button, INPUT_PULLDOWN);
	delay(3000); // 3 second delay for recovery


  	FastLED.addLeds<NEOPIXEL, RING_PIN>(ringLeds, RING_LEDS);
	FastLED.addLeds<NEOPIXEL, STRIP_PIN>(stripLeds, STRIP_LEDS);

  	// set master brightness control
  	FastLED.setBrightness(brightness);
}

  
void loop()
{	if(digitalRead(button) == HIGH){
        whileTouching();
    }
}


void whileTouching() {
	
	unsigned int colorIndex = 0; //Integer used to select applied color
	unsigned int satBright = 0; //The initial brightness and saturation of the lights
	CHSV color = green; //The first color
	fill_solid(stripLeds, STRIP_LEDS, CRGB::Black);
	fill_solid(ringLeds, RING_LEDS, CRGB::Black);

    while (digitalRead(button) == HIGH) {

		if(satBright > 250){
		    ringFill(color.hue);
		    fill_solid(stripLeds, STRIP_LEDS, CRGB::Black);
		    fill_solid(ringLeds, RING_LEDS, CRGB::Black);
			satBright = 0;
			colorIndex ++;
			color = currentPalette[colorIndex % colors];
			
			}        
	
		uint8_t pixelsLit = map(satBright,0,250,0,STRIP_LEDS);
		
		for (int i = 0; i <= pixelsLit && i <= STRIP_LEDS;i++){
			stripLeds[i] = color;
			}


		color.saturation = satBright;
		color.value = satBright;
		satBright += 15;
		FastLED.show();
		delay(60);
		now = millis();
	}
	FastLED.show();
	beaconColor = color;
	fill_solid(stripLeds, STRIP_LEDS, CRGB::Black);
	fill_solid(ringLeds, RING_LEDS, CRGB::Black);
	sprintf(publishString, "%u:%u:%u",beaconColor.hue,beaconColor.saturation,beaconColor.value);
    Spark.publish("Color: ",publishString);
    lastTime = now;

}


void ringFill(int colorHue){ //This function lights up the beacon when the colors are maxed

    for(int i = 0; i <= 255; i++){
        fill_solid(ringLeds, RING_LEDS, CHSV(colorHue,255,i));
        FastLED.show();
        delay(5);
    }
}
