#include "rawData.h"

rawData::rawData(short int numLEDs, byte bytesPerLED)
{  
  _numLEDs = numLEDs;
  _bytesPerLED = bytesPerLED;
  _frameLength = numLEDs * bytesPerLED;
  LEDs = new byte[_frameLength];

  //Init Array
  clearData();
}

void rawData::setPixel(short int pixelIndex, byte *pixelColour)
{
  _counter = _bytesPerLED * pixelIndex;
  LEDs[ _counter ] = pixelColour[0];
  LEDs[ _counter + 1 ] = pixelColour[1];
  LEDs[ _counter + 2 ] = pixelColour[2];
}
void rawData::getPixel(short int pixelIndex, byte *pixelColour)
{
  _counter = _bytesPerLED * pixelIndex;
  pixelColour[0] = LEDs[ _counter ];
  pixelColour[1] = LEDs[ _counter + 1 ];
  pixelColour[2] = LEDs[ _counter + 2 ];
}

void rawData::clearData()
{
  for(_counter=0; _counter<_frameLength; _counter++)
  {
    LEDs[_counter] = 0;
  }
}
