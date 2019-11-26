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
#include <Adafruit_GFX.h>

//#include <SD.h>

#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(-1);

MAX30105 pulseox; //max30102  object

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float bpm;
int beatAvg;
int lastBeatAvg;
String bpmstr;
long delta;

const int bigmotor = 10;
const int smallmotor = 5;
const int Led = LED_BUILTIN;

void setup()
{
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setTextSize(1);                                   //Near it display the average BPM you can display the BPM if you want
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.println("Begin Initialization");
  Serial.println("Begin Initialization");
  display.display();

  delay(500);

  for (int I = 0 ; I <= 1024 ; I += 1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Initializing");
    display.print(I);
    display.display();
  }


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


  pinMode(bigmotor, OUTPUT);
  pinMode(smallmotor, OUTPUT);
  pinMode(Led, OUTPUT);

  digitalWrite(bigmotor, HIGH);
  digitalWrite(smallmotor, HIGH);
  digitalWrite(Led, HIGH);
  delay(500);
  digitalWrite(bigmotor, LOW);
  digitalWrite(smallmotor, LOW);
  digitalWrite(Led, LOW);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Done Initialization");
  display.println("Waiting For Data");
  display.display();
  display.setTextSize(1.5);
}




void loop()
{
  long irValue = pulseox.getIR();

  if (beatAvg != lastBeatAvg) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("BPM Average: ");
    display.println(beatAvg);
    display.print("BPM  Actual: ");
    display.println(bpm);
    display.print("Beat  Delay: ");
    display.println(delta);
    display.display();
    lastBeatAvg = beatAvg;
  }

  if (checkForBeat(irValue) == true) {
    delta = millis() - lastBeat;
    lastBeat = millis();

    bpm = 60 / (delta / 1000.0);
    bpmstr = String(bpm);

    if (bpm < 255 && bpm > 20) {
      rates[rateSpot++] = (byte)bpm; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  /*
    display.setCursor(0, 0);
    display.print(bpmstr);
    display.display();*/

  if (irValue > 50000) {
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");


    Serial.print(bpm);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
    Serial.print(", Delay=");
    Serial.print(delta);
  } else {
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print("   Put it on your arm");
  }
  Serial.println();

  if (millis() - lastBeat <= 2500) {
    //digitalWrite(bigmotor, HIGH);
    digitalWrite(smallmotor, HIGH);
    digitalWrite(Led, HIGH);
  } else {
    //digitalWrite(bigmotor, LOW);
    digitalWrite(smallmotor, LOW);
    digitalWrite(Led, LOW);
  }

}
