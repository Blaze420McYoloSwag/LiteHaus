// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
FASTLED_USING_NAMESPACE; //Gotta include the namespace because reasons

/*
__________________________FastLED variables__________________________
*/
#define RING_PIN    D1
#define RING_LEDS    24
CRGB ringLeds[RING_LEDS]; //Gotta use CRGB because FastLED won't auto convert from HSV to RGB
CHSV HSVRing[RING_LEDS]; //All the real magic happens to this array

#define STRIP_PIN    D0
#define STRIP_LEDS    15
CRGB stripLeds[STRIP_LEDS]; //Same situation as the ring stuff above
//CHSV HSVStrip[STRIP_LEDS];

/*
__________________________FastLED color declarations__________________________
*/
const int  colors = 5;
CHSV red = CHSV(0, 0 , 0);
CHSV yellow  = CHSV(64, 0 , 0);
CHSV green  = CHSV(85, 0 , 0);
CHSV blue  = CHSV(171, 0 , 0);
CHSV purple  = CHSV(192, 0 , 0);
CHSV currentPalette[colors] =  { blue, green, yellow, purple, red};

CHSV beaconColor; //The color beacon sent
CHSV baseColor = CHSV(0,0,0);  //The beacon color received

/*
__________________________Communication Variables__________________________
*/
char beaconString[40]; //the string being published
String selfName = "Tim"; //The name of this device
String otherName = "Bonnie"; //The name of the device you're listening to

/*
__________________________Misc Variables__________________________
*/
const int button = D3; //The push button pin
uint8_t brightness = 254; //Global brightness assignment
uint8_t baseLedsLit; //The current number of lit LEDS
uint8_t whileDelay = 4; //delay used in the while loop, in ms (Zero might mess with the wifi if held too long)

unsigned long decayRate = 11500; //decay rate in MS. a fully lit beacon decays to zero in a bit more than 8 hours.
unsigned long decayUpdate = 0;

unsigned long spinRate = 210; //spin rate in MS. ideally a full rotation happens every 5 seconds or so
unsigned long spinUpdate = 0;

/*
__________________________Function Declarations__________________________
*/
void beaconFill(NSFastLED::CHSV color); //This function fills the beacon with various colors based on the input beacon color calculated
void beaconSpin(); //This function spins the beacon colors

/*
__________________________The Big Show__________________________
*/
void setup() {
    pinMode(button, INPUT_PULLDOWN); //Push button (for now... muhaahahaha)
	delay(3000); // 3 second delay for recovery
    Particle.subscribe("Beacon Update", ColorUpdate, MY_DEVICES); //Subscribe to the updates

  	FastLED.addLeds<NEOPIXEL, RING_PIN>(ringLeds, RING_LEDS); //initalize the ring
	FastLED.addLeds<NEOPIXEL, STRIP_PIN>(stripLeds, STRIP_LEDS); //initialize the strip
	fill_solid(stripLeds, STRIP_LEDS, CRGB::Black); //black out the strip
	
	fill_rainbow(ringLeds, RING_LEDS, 0, 10); //add rainbow to ring
	for(int i = 0; i < 3 * RING_LEDS; i++){ //this will spin the ring around 3 times
	    CRGB tempColor = ringLeds[0];
	    for(int j = 0; j <RING_LEDS-1; j++){
	        ringLeds[j] = ringLeds[j+1];
	    }
	    ringLeds[RING_LEDS - 1] = tempColor;
	    FastLED.show();
	}
  	// set master brightness control
  	FastLED.setBrightness(brightness);
}

void loop()
{	
    if(digitalRead(button) == HIGH){ //If the button is pressed
        delay(50); //bullshit debounce delay
        whileTouching(); //this badboy runs while the 
    }
    if((decayUpdate + decayRate) < millis()){  //Check if enough time has passed to decay the lights
        colorDecay(); //the function to decay the colors (duh)
        decayUpdate = millis();
    }
    if((spinUpdate + spinRate) < millis()){  //Check if enough time has passed to decay the lights
        beaconSpin(); //the function to spin the beacon
        spinUpdate = millis();
    }
    if(HSVRing[0].value){
        hsv2rgb_rainbow( HSVRing, ringLeds, RING_LEDS); // convert the HSV array to the RGB array (if the beacon is still lit)
    }
    
    FastLED.show(); //Light the beacon
}


/*
__________________________Touch Functions__________________________
*/

void whileTouching() {
	uint8_t colorIndex = 0; //Integer used to select applied color
	unsigned int satBright = 0; //The initial brightness and saturation of the lights
    CHSV color; //The resulting color
    
    //Set the colors black
	fill_solid(stripLeds, STRIP_LEDS, CRGB::Black);
	fill_solid(ringLeds, RING_LEDS, CRGB::Black);

    while (digitalRead(button) == HIGH) { //Loop that runs while the button is pressed
        color = currentPalette[colorIndex % colors]; //The first color
        //The saturation and value are equal and stepped
        color.saturation = satBright;
		color.value = satBright;

		if(satBright > 250){ //If light hits the maximum, trip the ring animation and reset
		    ringFill(color.hue);
		    fill_solid(stripLeds, STRIP_LEDS, CRGB::Black);
		    fill_solid(ringLeds, RING_LEDS, CRGB::Black);
			satBright = 0;
			colorIndex ++;
			}        
	
		uint8_t pixelsLit = map(satBright,0,245,0,STRIP_LEDS); //map the number of lit pixels in the base
		
		for (int i = 0; i <= pixelsLit && i <= STRIP_LEDS;i++){ //assign all the strip LED's
			stripLeds[i] = color; 
			}

		satBright += 5; //increment the saturation and brightness
		FastLED.show(); //Light em up
		delay(whileDelay); //wait
	}
	FastLED.show();
	fill_solid(stripLeds, STRIP_LEDS, CRGB::Black); //black the strip again
	fill_solid(stripLeds, baseLedsLit, baseColor); //write the relevant LEDs the base color
	beaconColor = color; //assign the final color to beacon
	beaconFill(color); //fill the beacon accordingly
	
	char publishString [64];
	sprintf(publishString, "%u~%u~%u",beaconColor.hue,beaconColor.saturation,beaconColor.value);
    Spark.publish("Beacon Update", String(selfName) + "~" + publishString); //Publish the beacon color to the cloud
}


/*
__________________________Color Functions__________________________
*/

void ringFill(int colorHue){ //This function lights up the beacon when the colors are maxed

    for(int i = 0; i <= 255; i++){
        fill_solid(ringLeds, RING_LEDS, CHSV(colorHue,255,i));
        FastLED.show();
        //delay(3);
    }
}

void colorDecay(){
    if(baseColor.value){ //test if the base is still lit
        baseColor.value--; //cut the brightness
    }
    if(beaconColor.value){ //test if the beacon is still lit
        beaconColor.value--; //decrement the inital beacon color brightness
        
        //make the entire ring array as bright as the initial beacon color
        for(int i = 0; i <RING_LEDS;i++){
            HSVRing[i].value = beaconColor.value; 
        }
        
    }
    baseLedsLit = map(baseColor.value,0,255,2,STRIP_LEDS); // figure out how many base LEDs are lit
    fill_solid(stripLeds, STRIP_LEDS, CRGB::Black); //turn the beacon black
    fill_solid(stripLeds, baseLedsLit, baseColor); //write the relevant LEDs the color
}

void beaconFill(NSFastLED :: CHSV color){ //Fills the ring LED's with the HSV array made from the beacon color sent
    fill_solid(HSVRing, RING_LEDS, color);
    uint8_t hue = color.hue;
    for (int i = 1; i < RING_LEDS/2; i++){ //this tweaks the hues around the ring
        HSVRing[i].hue = hue+i;
        HSVRing[RING_LEDS-i].hue = hue + i;
    }
}
void beaconSpin(){
    CHSV tempColor = HSVRing[0];
    for(int i = 0; i < RING_LEDS-1; i++){
        HSVRing[i] = HSVRing[i+1];
    }
    HSVRing[RING_LEDS-1] = tempColor;
}

/*
__________________________Communication Functions__________________________
*/

void ColorUpdate(const char *name, const char *data) { //The function to received the base color from the cloud
    uint8_t receivedHue;
    uint8_t receivedSaturation;
    uint8_t receivedValue;
    String receivedID;
    if (data) {
        char input[64];
        strcpy(input,data);
        char *p;
        receivedID = strtok(input,"~");
        p = strtok(NULL,"~");
        receivedHue = atoi(p);
        p = strtok(NULL,"~");
        receivedSaturation = atoi(p);
        p = strtok(NULL,"~");
        receivedValue = atoi(p);
    }
    // DEBUG
    Particle.publish("Color_Recieved", selfName + "~" + receivedHue + "~" + receivedSaturation + "~" + receivedValue);
    // END DEBUG
    if(receivedID ==otherName ){
        baseColor = CHSV(receivedHue, receivedSaturation, receivedValue);
    }
}
