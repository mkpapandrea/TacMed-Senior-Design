//Code to test pulse ox to oled
//some code taken from examples in the library
//max30102  using i2c
//spd1306 oled using i2c

//libraries for pulseox
#include <MAX30105.h>
#include <Wire.h>
#include <heartRate.h>

#include <splash.h>
#include <Adafruit_SSD1306.h>

#include <SD.h>

MAX30105 pulseox; //max30102  object

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float bpm;
int beatAvg;

void setup()
{
  Serial.begin(115200);

 

  // Initialize sensor
  if (!pulseox.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("put it on your arm");

  pulseox.setup(); 
  pulseox.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  pulseox.setPulseAmplitudeGreen(0); //Turn off Green LED
}




void loop()
{
  long irValue = pulseox.getIR();

  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    bpm = 60 / (delta / 1000.0);

    if (bpm < 255 && bpm > 20)
    {
      rates[rateSpot++] = (byte)bpm; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(bpm);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}
