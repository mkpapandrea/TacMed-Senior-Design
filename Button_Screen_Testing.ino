#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Bounce2.h>

// OLED display TWI address
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(-1);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int mode = 0;
int modeCount = 3;

int v = 0;
int vMax = 200;
int vMin = 50;

const int BtnU = 13;
Bounce Up = Bounce ();
const int BtnL = 12;
Bounce Left = Bounce ();
const int BtnC = 11;
Bounce Mid = Bounce ();
const int BtnR = 10;
Bounce Right = Bounce ();
const int BtnD = 9;
Bounce Down = Bounce ();

int Capacity[] = {6, 8, 10, 12, 18, 22, 24, 25, 35};
int Cap = 0;
int CapMax = 8;

void setup() {
  Serial.begin(9600);
  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  {
    pinMode(BtnU, INPUT);                       //Debounced Up Button
    Up.attach(BtnU);
    Up.interval(5);

    pinMode(BtnD, INPUT);                       //Debounced Down Button
    Down.attach(BtnD);
    Down.interval(5);

    pinMode(BtnL, INPUT);                       //Debounced Left Button
    Left.attach(BtnL);
    Left.interval(5);

    pinMode(BtnR, INPUT);                       //Debounced Right Button
    Right.attach(BtnR);
    Right.interval(5);

    pinMode(BtnC, INPUT);                       //Debounced Center Button
    Mid.attach(BtnC);
    Mid.interval(5);
  }
}

void loop() {
  Up.update();                              //update bounce vlaues
  Down.update();
  Left.update();
  Right.update();
  Mid.update();

  display.clearDisplay();

  if (Left.read()) {
    if (mode >= 1) {
      mode -= 1;
    } else {
      mode = modeCount;
    }
  }

  if (Right.read()) {
    if (mode <= (modeCount - 1)) {
      mode += 1;
    } else {
      mode = 0;
    }
  }

  switch (mode) {
    case 0:
      display.setCursor(0, 0);
      display.print("Mode: Speed 1");

      if (Down.read()) {
        if (v > vMin) {
          v -= 1;
        }
      }
      if (Up.read()) {
        if (v < vMax) {
          v += 1;
        }
      }

      break;
    case 1:
      display.setCursor(0, 0);
      display.print("Mode: Speed 10");

      if (Down.read()) {
        if (v > vMin) {
          v -= 10;
        }
      }
      if (Up.read()) {
        if (v < vMax) {
          v += 10;
        }
      }

      break;
    case 2:
      display.setCursor(0, 0);
      display.print("Mode: Speed 25");

      if (Down.read()) {
        if (v > vMin) {
          v -= 25;
        }
      }
      if (Up.read()) {
        if (v < vMax) {
          v += 25;
        }
      }

      break;
    case 3:
      display.setCursor(0, 0);
      display.print("Mode: Capacity");
      {
        if (Down.read()) {
          if (Cap > 0) {
            Cap -= 1;
          }
        }
        if (Up.read()) {
          if (Cap < CapMax) {
            Cap += 1;
          }
        }
      }
      break;
    case 4:

      break;
  }

  String v1 = String("Speed: N");
  String v2 = String("/" + v);
  String velocity = String(v1 + v2);
  display.setCursor(0, 10);
  display.print(velocity);
  
  String c1 = String("Capacity: N");
  String c2 = String("/" + Capacity[Cap]);
  String CapVal = String(c1 + c2);
  display.setCursor(0, 20);
  display.print(CapVal);

  display.display();

  Serial.print(mode);
  Serial.print(velocity);
  Serial.println(CapVal);
  delay(10);
}

