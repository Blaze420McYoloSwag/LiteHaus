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
#define STRIP_LEDS    16
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
String selfName = "Tim"; //The name of this device
String otherName = "Bonnie"; //The name of the device you're listening to
char publishString [64]; //the string being published
/*
__________________________Misc Variables__________________________
*/
const int button = D3; //The push button pin
const int potPin = A0; //The pin with the brightness potentiometer attached.
uint8_t brightness = 254; //Global brightness assignment
uint8_t baseLedsLit; //The current number of lit LEDS
uint8_t whileDelay = 8; //delay used in the while loop, in ms (Zero might mess with the wifi if held too long)
uint8_t extinguishValue = 20; //Cut off the light here before things get weird.

unsigned long decayRate = 7500; //decay rate in MS (11500). a fully lit beacon decays to zero in a bit more than 8 hours.
unsigned long decayUpdate = 0;

unsigned long spinRate = 250; //spin rate in MS. ideally a full rotation happens every 5 seconds or so
unsigned long spinUpdate = 0;

/*
__________________________Function Declarations__________________________
*/
void beaconFill(NSFastLED::CHSV color); //This function fills the beacon with various colors based on the input beacon color calculated
void beaconSpin(); //This function spins the beacon colors
void ColorUpdate(const char *name, const char *data);

/*
__________________________Timer Declarations__________________________
*/
Timer decayTimer(decayRate, colorDecay);
Timer spinTimer (spinRate, beaconSpin);
/*
__________________________The Big Show__________________________
*/
void setup() {
    pinMode(button, INPUT_PULLDOWN); //Push button (for now... muhaahahaha)
	delay(3000); // 3 second delay for recovery
    Particle.subscribe("Beacon_Update",ColorUpdate); //Subscribe to the updates

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
	    delay(50); //slow the spin a bit
	}
  	// set master brightness control
  	FastLED.setBrightness(brightness);
  	decayTimer.start();
  	spinTimer.start();
}

void loop()
{	
    if(digitalRead(button) == HIGH){ //If the button is pressed
        delay(50); //bullshit debounce delay
        whileTouching(); //this badboy runs while the 
    }
    if(beaconColor.value > extinguishValue){
        hsv2rgb_rainbow( HSVRing, ringLeds, RING_LEDS); // convert the HSV array to the RGB array (if the beacon is still lit)
    }
    else{
        fill_solid(ringLeds, RING_LEDS, CRGB::Black);
    }
    brightness = map(analogRead(potPin),0,4096,0,254); // set the brightness
    FastLED.setBrightness(brightness);
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
	
	
	sprintf(publishString, "%u~%u~%u",beaconColor.hue,beaconColor.saturation,beaconColor.value);
    Particle.publish("Beacon_Update", String(selfName) + "~" + publishString); //Publish the beacon color to the cloud
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
    
    if(beaconColor.value > extinguishValue){ //test if the beacon is still lit
        beaconColor.value--; //decrement the inital beacon color brightness
        
        //make the entire ring array as bright as the initial beacon color
        for(int i = 0; i <RING_LEDS;i++){
            HSVRing[i].value = beaconColor.value; 
        }
        
    }
    else{
        beaconColor.value = 0;
    }

    baseLedsLit = map(baseColor.value,0,254,2,STRIP_LEDS+1); // figure out how many base LEDs are lit
    fill_solid(stripLeds, STRIP_LEDS, CRGB::Black); //turn the beacon black
    fill_solid(stripLeds, baseLedsLit, baseColor); //write the relevant LEDs the color
    sprintf(publishString, "%u~%u~%u",beaconColor.hue,beaconColor.saturation,beaconColor.value);
    Particle.publish("Beacon_Update", String(selfName) + "~" + publishString); //Publish the beacon color to the cloud
}

void beaconFill(NSFastLED :: CHSV color){ //Fills the ring LED's with the HSV array made from the beacon color sent
    fill_solid(HSVRing, RING_LEDS, color);
    uint8_t hue = color.hue;
    uint8_t sat = color.saturation;
    for (int i = 0; i < RING_LEDS/2; i++){ //this tweaks the hues around the ring
        HSVRing[i].hue = hue - 3*i;
        HSVRing[RING_LEDS -1- i].hue = hue -  3*i;
        HSVRing[i].saturation= sat - 10*i;
        HSVRing[RING_LEDS -1- i].saturation = sat -  10*i;
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
    if(receivedID == otherName){
        // DEBUG
        Particle.publish("Color_Recieved", selfName + "~" + receivedHue + "~" + receivedSaturation + "~" + receivedValue);
        // END DEBUG
        baseColor = CHSV(receivedHue, receivedSaturation, receivedValue);
    }
}
