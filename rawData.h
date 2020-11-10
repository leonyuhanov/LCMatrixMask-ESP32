#ifndef rawData_h
#define rawData_h
#include "Arduino.h"

class rawData
{
  public:
    rawData(short int numLEDs, byte bytesPerLED);
    void setPixel(short int pixelIndex, byte *pixelColour);
    void getPixel(short int pixelIndex, byte *pixelColour);
    
    byte* LEDs;
    short int _frameLength;
    short int _numLEDs;
    byte _bytesPerLED;
    short int _counter;
    
  private:
};

#endif
