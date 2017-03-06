// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

int receivedHue = 0;
int receivedSaturation = 0;
int receivedValue = 0;

CHSV baseColor;

void setup() {
    Particle.subscribe("Beacon Update", ColorUpdate, MY_DEVICES);
}

void loop() {

}




void ColorUpdate(const char *name, const char *data) {
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
    String sColorRecieved = String(colorRecieved);
    Particle.publish("Color_Recieved", System.deviceID() + "~" + receivedHue + "~" + receivedSaturation + "~" receivedValue);
    // END DEBUG
    baseColor = CHSV(receivedHue, receivedSaturation, receivedValue)
}
