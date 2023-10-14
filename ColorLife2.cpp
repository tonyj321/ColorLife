#include <Arduino.h>
//#define USE_ADAFRUIT_GFX_LAYERS

#include <Entropy.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>  // SmartLED Shield for Teensy 4 (V5)
#include <SmartMatrix.h>

#include "LEDMatrixLife.h"

#define COLOR_DEPTH 24                                         // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 64;                              // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 64;                             // Set to the height of your display
const uint8_t kRefreshDepth = 36;                              // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;                              // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SMARTMATRIX_HUB75_64ROW_MOD32SCAN;  // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);       // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

#include <Adafruit_GFX.h>  // Core graphics library
#include <Fonts/FreeSerif9pt7b.h>
#include <TimeLib.h>

const int defaultBrightness = 100 * (255 / 100);  // full brightness
// Red and green; yellow and purple; orange and blue; green and magenta
//const rgb24 red = { 255, 0, 0 };
//const rgb24 yellow = { 255, 255, 0 };
//const rgb24 orange = { 255, 165, 0 };
//const rgb24 green = { 0, 255, 0 };
//const rgb24 purple = { 255, 255, 255 };
//const rgb24 blue = { 0, 0, 255 };
//const rgb24 cyan = { 0, 255, 255 };
//const rgb24 magenta = { 255, 0, 255 };

// Taken from https://sashamaps.net/docs/resources/20-colors/

const rgb24 maroon = { 128, 0, 0 };
const rgb24 brown = { 170, 110, 40 };
const rgb24 olive = { 128, 128, 0 };
const rgb24 teal = { 0, 128, 128 };
const rgb24 navy = { 0, 0, 128 };
const rgb24 black = { 0, 0, 0 };
//
const rgb24 red = { 230, 25, 75 };
const rgb24 orange = { 245, 130, 48 };
const rgb24 yellow = { 255, 255, 25 };
const rgb24 lime = { 210, 245, 60 };
const rgb24 green = { 80, 180, 75 };
const rgb24 cyan = { 70, 240, 240 };
const rgb24 blue = { 0, 130, 200 };
const rgb24 purple = { 145, 30, 180 };
const rgb24 magenta = { 240, 50, 230 };
const rgb24 grey = { 128, 128, 128 };
//
const rgb24 pink = { 250, 190, 212 };
const rgb24 apricot = { 255, 215, 180 };
const rgb24 beige = { 255, 250, 200 };
const rgb24 mint = { 170, 255, 195 };
const rgb24 lavendar = { 220, 190, 255 };
const rgb24 white = { 255, 255, 255 };

const rgb24 dead = black;

const int nDefaultColors = 9;
const rgb24 defaultColors[nDefaultColors] = { dead, red, orange, yellow, green, cyan, blue, purple, magenta };
// Generations colors
//const int nColors = 8;
//const rgb24 colors[nColors] = { dead, rgb24(255, 0, 0), rgb24(255, 42, 0), rgb24(255, 84, 0), rgb24(255, 126, 0), rgb24(255, 168, 0), rgb24(255, 210, 0), rgb24(255, 254, 0) };
// SteepleChas colors
// const int nColors = 4;
// const rgb24 colors[nColors] = { dead, rgb24(255, 0, 0), rgb24(255, 128, 0),  rgb24(255, 255, 0) };

const int xSize = kMatrixWidth;
const int ySize = kMatrixHeight;
LEDMatrixLife* life;
NiemiecTreeRule defaultRule;

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER "T"  // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600;  // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    return pctime;
    if (pctime < DEFAULT_TIME) {  // check the value is a valid time (greater than Jan 1 2013)
      pctime = 0L;                // return 0 to indicate that the time is not valid
    }
  }
  return pctime;
}

// the setup() method runs once, when the sketch starts
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);

  Serial.begin(115200);
  while (!Serial)
    ;  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

  Entropy.Initialize();
  randomSeed(Entropy.random());

  matrix.addLayer(&backgroundLayer);
  matrix.begin();
  matrix.setBrightness(defaultBrightness);
  backgroundLayer.enableColorCorrection(true);

  //life = new SimpleLife(xSize, ySize, new NiemiecTreeRule());
  Life* lifeImplementation = new InfiniteLife(4, &defaultRule);
  //Life* lifeImplementation = new SimpleLife(xSize, ySize, defaultRule);
  life = new LEDMatrixLife(*lifeImplementation, &backgroundLayer);
}

void start(LEDMatrixLife* life);
void SteepleChase(LEDMatrixLife* life);
void Lava(LEDMatrixLife* life);
void tannersp46gun(LEDMatrixLife* life);
void snarkcatalystvariants(LEDMatrixLife* life);
void ASJ2023(LEDMatrixLife* life);
void startText(LEDMatrixLife* life, const char* text);
void startRandom(LEDMatrixLife* life);
void loadrle(LEDMatrixLife* life, int x, int y, const char *rle);

// the loop() method runs over and over again,
// as long as the board has power
void loop() {
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t);  // set the RTC
      setTime(t);
    }
  }

  // clear screen
  backgroundLayer.fillScreen(dead);
  backgroundLayer.swapBuffers(true);

  start(life);
}

void start(LEDMatrixLife* life) {
  life->clear();
  life->setRule(9, &defaultRule);
  life->setColorMap(nDefaultColors, defaultColors);
  life->setInitialDelay(0);
  int r = random(100);
  if (r < 10) {
    //ASJ2023(life);
    startText(life, "ASJ\n2023");
  } else if (r == 10) {
    snarkcatalystvariants(life);
  } else if (r == 11) {
    tannersp46gun(life);
  } else if (r == 12) {
    SteepleChase(life);
  } else if (r == 13) {
    Lava(life);
  } else if (r < 20) {
    char buffer[40];
    const char* months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    sprintf(buffer, "%s %d\n%d\n%02d:%02d:%02d", months[month() - 1], day(), year(), hour(), minute(), second());
    startText(life, buffer);
  } else {
    startRandom(life);
  }
  //lobstr(life);

}

void startText(LEDMatrixLife* life, const char* text) {
  life->setInitialDelay(1000);
  GFXcanvas8 canvas(xSize, ySize);
  canvas.setFont(&FreeSerif9pt7b);
  // Count lines
  char buffer[64];
  strcpy(buffer, text);
  int nLines = 1;
  for (unsigned int i = 0; i < strlen(text); i++) {
    if (text[i] == '\n') {
      nLines++;
      buffer[i] = '\0';
    }
  }
  Serial.printf("nLines = %d\n", nLines);
  uint8_t color = 1;
  int16_t x1, y1;
  uint16_t w, h;
  canvas.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  Serial.printf("0 %s %d %d %d %d %d\n", buffer, x1, y1, w, h, (xSize - w) / 2);
  canvas.setCursor((xSize - w) / 2, 18);
  for (unsigned int i = 0; i < strlen(text); i++) {
    if (text[i] == '\n') {
      canvas.println();
      canvas.getTextBounds(buffer + i + 1, 0, 0, &x1, &y1, &w, &h);
      Serial.printf("1 %s %d %d %d %d %d\n", buffer + i + 1, x1, y1, w, h, (xSize - w) / 2);
      canvas.setCursor((xSize - w) / 2, canvas.getCursorY() - 4);
    } else {
      canvas.setTextColor(color++);
      if (color >= nDefaultColors) color = 1;
      canvas.print(text[i]);
    }
  }
  for (int y = 0; y < ySize; y++) {
    for (int x = 0; x < xSize; x++) {
      life->set(x, y, canvas.getPixel(x, y));
    }
  }
  life->setInitialDelay(2000);
  life->run();
}

void startRandom(LEDMatrixLife* life) {
  float prob = .25;
  for (int y = 0; y < ySize; y++) {
    for (int x = 0; x < xSize; x++) {
      int r = random(256);
      int on = r / 256. < prob ? 1 + (r % (nDefaultColors - 1)) : 0;
      life->set(x, y, on);
    }
  }
  life->run();
}

// Lava rule by Mirek Wojtowicz
void Lava(LEDMatrixLife* life) {
  int x = 63, y = 63;  // rule = 12345/45678/8:T300,300
  GenerationsTreeRule rule;
  life->setRule(8, &rule);
  const int nColors = 8;
  const rgb24 colors[nColors] = { dead, rgb24(255, 0, 0), rgb24(255, 42, 0), rgb24(255, 84, 0), rgb24(255, 126, 0), rgb24(255, 168, 0), rgb24(255, 210, 0), rgb24(255, 254, 0) };
  life->setColorMap(nColors, colors);
  const char* rle = R"(
    63A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$
    A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A
    $A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.
    A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A
    61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$
    A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A$A61.A
    $A61.A$A61.A$A61.A$63A!
  )";
  loadrle(life, (xSize - x) / 2, (ySize - y) / 2, rle);
  life->run();
}

// Star Wars Fun collection
// 
// Steeplechase, p20
// 
// Mirek Wojtowicz, May 1999
void SteepleChase(LEDMatrixLife* life) {
  int x = 63, y = 59;  // rule = 345/2/4:P500,500
  Generations1TreeRule rule;
  life->setRule(4, &rule);
  const rgb24 colors[4] = { dead, rgb24(255, 0, 0), rgb24(255, 128, 0),  rgb24(255, 255, 0) };
  life->setColorMap(4, colors);

  const char* rle = R"(
    2$31.A$30.3A8.A2.A$31.A8.6A$31.A9.A2.A$30.3A8.A2.A$31.A8.6A$31.A9.A2.
    A$30.3A8.A2.A$31.A8.6A$5.CB24.A9.A2.A$7.C22.3A8.A2.A$4.A.A.B22.A8.6A
    10.CB$.A.6A22.A9.A2.A11.A.A$.B2A2.A.A11.A9.3A8.A2.A10.3A$.C.BA.CB11.
    3A9.A8.6A10.A$20.A10.A9.A2.A$20.A9.3A$19.3A9.A$20.A10.A$20.A9.3A$19.
    3A9.A$20.A10.A$20.A9.3A$19.3A9.A$20.A4$61.A$60.3A$48.A12.A$16.A4.A15.
    A9.3A11.A$2.A12.3A2.3A13.3A7.2A.2A10.2A$.3A12.A.2A.A13.2A.2A5.2A.A.2A
    9.A$2.A13.A.2A.A12.2A.A.2A5.2A.A.2A8.A$.ABC11.3A2.3A10.2A.A.2A7.2A.2A
    9.2A$16.A4.A12.2A.2A9.3A10.A$35.3A11.A11.A$36.A23.3A$61.A10$14.A4.A4.
    A8.A6.A9.A$13.3A2.3A2.3A6.3A4.3A7.3A$14.A.2A.A.2A.A8.A6.A9.A6.A.C$3.A
    10.A.2A.A.2A.A8.A6.A9.A5.3AB$2.3A8.3A2.3A2.3A6.3A4.3A7.3A5.A.A$3.A10.
    A4.A4.A8.A6.A9.A$2.ABC!
  )";
  loadrle(life, (xSize - x) / 2, (ySize - y) / 2, rle);
  life->run();
}

void ASJ2023(LEDMatrixLife* life) {
  life->setInitialDelay(1000);
  int x = 30, y = 22;
  const char* rle = R"(
    6.2B5.4C5.5A$5.B2.B3.C4.C6.A$4.B4.B2.C11.A$4.B4.B2.C11.A$4.B4.B3.4C7.
    A$4.6B7.C6.A$4.B4.B7.C6.A$4.B4.B2.C4.C2.A3.A$4.B4.B3.4C4.3A5$.4D5.2E5.
    4F3.6G$D4.D3.E2.E3.F4.F7.G$D4.D2.E4.E2.F4.F6.G$5.D2.E4.E7.F5.G$4.D3.E
    4.E6.F5.3G$2.2D4.E4.E4.2F9.G$.D6.E4.E3.F11.G$D8.E2.E3.F7.G4.G$6D4.2E4.
    6F3.4G!
    )";
  loadrle(life, (xSize - x) / 2, (ySize - y) / 2, rle);
  life->setInitialDelay(1000);
  life->run();
}

void snarkcatalystvariants(LEDMatrixLife* life) {
  //#N snarkcatalystvariants.rle
  //#C four Snark catalyst variants
  //#C    Top:  original variant by Mike Playle
  //#C   Left:  Shannon Omick (better clearance on a diagonal)
  //#C  Right:  Heinrich Koenig (better clearance on a different diagonal)
  //#C Bottom:  Simon Ekstrom (better clearance on two diagonals)
  //#C https://conwaylife.com/wiki/Snark
  //#C https://www.conwaylife.com/patterns/snarkcatalystvariants.rle
  int x = 51, y = 52;
  const char* rle = R"(
    20.2A$20.A.A$22.A4.2A$18.4A.2A2.A2.A$18.A2.A.A.A.A.2A$21.A.A.A.A$22.2A
    .A.A$26.A2$12.2A$13.A7.2A$13.A.A5.2A$14.2A25.B$39.3B$38.B$38.2B3$46.2B
    $24.2A21.B$24.A22.B.2B$14.3E8.3A11.2B4.3B2.B$4.D11.E10.A11.2B3.B3.2B$
    2.5D8.E5.2D21.4B$.D5.D13.D8.2B15.B$.D2.3D12.D.D7.B.B12.3B$2D.D15.2D8.
    B13.B$D2.4D21.2B14.5B$.2D3.D3.2D11.C22.B2.B$3.3D4.2D11.3C22.2B$3.D22.
    C$2D.D21.2C$2D.2D3$11.2D$12.D$9.3D$9.D25.2C$28.2C5.C.C$28.2C7.C$37.2C
    2$24.C$23.C.C.2C4.2C$23.C.C.C.C2.C2.C$22.2C.C.C.C3.2C$23.C2.2C.4C$23.
    C4.C3.C$24.3C.C2.C$26.C.C.C$29.C!
    )";
  loadrle(life, (xSize - x) / 2, (ySize - y) / 2, rle);
  life->run();
}

void tannersp46gun(LEDMatrixLife* life) {
  //#N tannersp46gun.rle
  //#C https://conwaylife.com/wiki/Tanner%27s_p46
  //#C https://www.conwaylife.com/patterns/tannersp46gun.rle
  //int x = 31, y = 44;
  const char* rle = R"(
    17.2B5.2C$17.2B5.2C11$17.B7.C$15.B.2B5.2C.C$15.B3.2B.2C3.C$16.B3.B.C3.
    C$17.3B3.3C10$14.A14.A$13.3A14.A$12.A.A.A11.3A$12.A.A.A$10.2A.3A.2A$9.
    A.2A.A.2A.A$3.2D3.2A.A5.A.2A$3.2D4.2A.A3.A.2A2.A.2A$10.3A3.3A3.2A.A2$
    2.2D$3.D$3D$D13.D$13.D.D.D.2D$12.D.2D.2D.D$12.D$11.2D!
    )";
  loadrle(life, 0, 0, rle);
  life->run();
}

void lobstr(LEDMatrixLife* life) {
  //#N lobster.rle
  //#O Matthias Merzenich, 2011
  //#C https://conwaylife.com/wiki/Lobster_(spaceship)
  //#C https://www.conwaylife.com/patterns/lobster.rle
  int x = 26, y = 26;
  const char* rle = R"(
    12b3o$12bo$13bo2b2o$16b2o$12b2o$13b2o$12bo2bo2$14bo2bo$14bo3bo$15b3obo
    $20bo$2o2bobo13bo$obob2o13bo$o4bo2b2o13b2o$6bo3bo6b2o2b2o2bo$2b2o6bo6b
    o2bo$2b2o4bobo4b2o$9bo5bo3bo3bo$10bo2bo4b2o$11b2o3bo5bobo$15bo8b2o$15b
    o4bo$14bo3bo$14bo5b2o$15bo5bo!
    )";
  loadrle(life, (xSize - x) / 2, (ySize - y) / 2, rle);
  life->run();
}

void loadrle(LEDMatrixLife* life, int xOff, int yOff, const char* rle) {
  life->clear();

  const char* p = rle;
  int count = 0;
  int x = xOff;
  int y = yOff;
  for (;;) {
    char c = *(p++);
    if (c == '!' || c == 0) {
      return;
    } else if (c == ' ' || c == '\n' || c == '\t') {
      continue;
    } else if (c >= '0' && c <= '9') {
      count = count * 10 + c - '0';
    } else {
      if (count == 0) count = 1;
      if (c == 'b' || c == '.') {
        x += count;
      } else if (c == 'o') {
        byte on = 1;
        for (int i = 0; i < count; i++) {
          life->set(x, y, on);
          x++;
        }
      } else if (c >= 'A' && c <= 'Z') {
        byte on = 1 + c - 'A';
        for (int i = 0; i < count; i++) {
          life->set(x, y, on);
          x++;
        }
      } else if (c == '$') {
        y += count;
        x = xOff;
      }
      count = 0;
    }
  }
}
