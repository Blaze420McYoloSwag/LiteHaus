// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

int receivedHue = 0;
int receivedSaturation = 0;
int receivedValue = 0;
int baseLightsLit = 0; //This value is the number of light lit in the base

CHSV baseColor;


String selfName = "Bonnie"; //The name of this device
String otherName = "Tim"; //The name of the device you're listening to

void setup() {
    Particle.subscribe("Beacon Update", ColorUpdate, MY_DEVICES);
}

void loop() {

}




void ColorUpdate(const char *name, const char *data) {
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

