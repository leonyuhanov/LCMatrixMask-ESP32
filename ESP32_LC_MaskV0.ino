/*
      
*/      

#include "SPI.h"
#include "rawData.h"
#include "colourObject.h"
#include <WiFi.h>

const short int numLeds = 1;
const byte bytesPerLed = 3;
const byte maxValue = 128;
byte tempColour[3] = {0,0,0};

char colourList[9*3]={maxValue,0,0, maxValue,maxValue,0, 0,maxValue,0, 0,maxValue,maxValue, 0,0,maxValue, maxValue,0,maxValue, maxValue,maxValue,maxValue, maxValue,0,0,0,0,0};
unsigned short int tickCnt=0, cIndex=0;

rawData leds = rawData(numLeds, bytesPerLed);
colourObject dynamicMultiColourBlock(maxValue, 9, colourList);

//multiplexer Pins
byte multiplexPins[5][2] = { {32,0},{33,0},{25,0},{26,0},{27,0} };
byte chans[5] = {1, 2, 4, 8, 16}; 
byte outputEnable = 14, latchPin = 12;
//timers
unsigned long timeData[3];

void setup()
{
  //Disable WIFI
  WiFi.mode(WIFI_OFF);  
  
  Serial.begin(115200);
  
  //set up SPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(20000000);
  
  //set up GPIO
  //Output Enable
  pinMode(outputEnable, OUTPUT);
  digitalWrite(outputEnable, LOW);
  //Latch
  pinMode(latchPin, OUTPUT);
  digitalWrite(latchPin, LOW);
  //ABCDE pins
  for(tickCnt=0; tickCnt<5; tickCnt++)
  {
    pinMode(multiplexPins[tickCnt][0], OUTPUT);
    digitalWrite(multiplexPins[tickCnt][0], LOW);
  } 

  
}

byte hasTimedOut()
{
  timeData[1] = millis();
  if(timeData[2] < timeData[1]-timeData[0])
  {
    return 1;
  }
  return 0;
}
void startTimer(unsigned long durationInMillis)
{
  timeData[0] = millis(); 
  timeData[2] = durationInMillis;
}

void renderLEDs()
{
  byte chanCnt=0;
  for(chanCnt=0; chanCnt<32; chanCnt++)
  {
    SPI.writeBytes(leds.LEDs, leds._frameLength);
    digitalWrite(latchPin, HIGH);
    digitalWrite(outputEnable, HIGH);
    setChanel( chanCnt, 5 );   
    digitalWrite(outputEnable, LOW);
    digitalWrite(latchPin, LOW);
  }
}


/
void loop()
{   
  testLoop();

  /*
  startTimer(100);  
  while(!hasTimedOut())
  {
    fillPixels(tempColour);
  }
  tempColour[0] = (tempColour[0]-1)%maxValue;
  */
  
  /*
  dynamicMultiColourBlock.getColour(cIndex%dynamicMultiColourBlock._bandWidth, tempColour);
  leds.setPixel(0, tempColour);
  for(tickCnt=numLeds-1; tickCnt>0; tickCnt--)
  {
    leds.getPixel(tickCnt-1, tempColour);
    leds.setPixel(tickCnt, tempColour);
  }
  renderLEDs();
  delay(10);
  cIndex+=10;
  */
}

void testLoop()
{
  unsigned short int trackCnt=0;

  leds.LEDs[trackCnt] = 1;
  while(true)
  {
    //startTimer(10);
    //while(!hasTimedOut())
    //{
      renderLEDs();
      yield();
    //}
    //leds.LEDs[trackCnt] = leds.LEDs[trackCnt] << 1;
    leds.LEDs[trackCnt]++;
    if(leds.LEDs[trackCnt]==0)
    {
      trackCnt = (trackCnt+1)%leds._frameLength;
      leds.LEDs[trackCnt] = 1;
    }
  }

  
}

void fillPixels(byte *colourVal)
{
  for(tickCnt=0; tickCnt<numLeds; tickCnt++)
  {
    leds.setPixel(tickCnt, colourVal);
  }  
}

void setChanel(byte chanID, byte chanCnt)
{
	/*
	A = 1
	B = 2
	C = 4
	D = 8
	E = 16
	*/
	byte indexCount=chanCnt, acum=0, teller=1, id=chanID, mIndex=0;
	
	for(indexCount; indexCount>0; indexCount--)
	{
		acum = id & teller;
		multiplexPins[mIndex][1] = acum;
		id = id >> 1;
		digitalWrite(multiplexPins[mIndex][0], multiplexPins[mIndex][1]);
    mIndex++;
	}
}
