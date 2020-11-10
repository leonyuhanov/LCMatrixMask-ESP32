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
  //SPI.setFrequency(500000);
  SPI.setFrequency(5000000);
  
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

//  NODEmcu SPI APA102 LED Strip Driver code
void renderLEDs()
{
  byte chanCnt=0;
  //for(chanCnt=0; chanCnt<5; chanCnt++)
  for(chanCnt=1; chanCnt<17; chanCnt++)
  {
    //setChanel( chans[chanCnt] );
    setChanel( chanCnt );
    digitalWrite(outputEnable, LOW);
    SPI.writeBytes(leds.LEDs, leds._frameLength);
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    digitalWrite(outputEnable, HIGH);
  }
}



void loop()
{   
  startTimer(1000);
  while(!hasTimedOut())
  {
    dynamicMultiColourBlock.getColour(cIndex%dynamicMultiColourBlock._bandWidth, tempColour);
    testData(tempColour);
    cIndex++;
  }

  startTimer(1000);
  tempColour[0] = maxValue;
  tempColour[1] = 0;
  tempColour[2] = 0;
  while(!hasTimedOut())
  {
    testData(tempColour);
  }

  startTimer(1000);
  tempColour[0] = 0;
  tempColour[1] = maxValue;
  tempColour[2] = 0;
  while(!hasTimedOut())
  {
    testData(tempColour);
  }
  startTimer(1000);
  tempColour[0] = 0;
  tempColour[1] = 0;
  tempColour[2] = maxValue;
  while(!hasTimedOut())
  {
    testData(tempColour);
  }
  
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

void testData(byte *colourVal)
{
  for(tickCnt=0; tickCnt<numLeds; tickCnt++)
  {
    leds.setPixel(tickCnt, colourVal);
  }
  renderLEDs();
  yield();
  
}

void setChanel(byte chanID)
{
	/*
	A = 1
	B = 2
	C = 4
	D = 8
	E = 16
	*/
	byte indexCount=5, acum=0, teller=1, id=chanID, mIndex=0;
	
	for(indexCount; indexCount>0; indexCount--)
	{
		acum = id & teller;
		multiplexPins[mIndex][1] = acum;
		id = id >> 1;
		digitalWrite(multiplexPins[mIndex][0], multiplexPins[mIndex][1]);
    mIndex++;
	}
}
