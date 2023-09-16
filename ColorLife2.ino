#define USE_ADAFRUIT_GFX_LAYERS

#include <Entropy.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>  // SmartLED Shield for Teensy 4 (V5)
#include <SmartMatrix.h>

#include "Life.h"

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
const rgb24 lavendar = { 220, 290, 255 };
const rgb24 white = { 255, 255, 255 };

const rgb24 dead = black;

const int nColors = 9;
const rgb24 colors[nColors] = { dead, red, orange, yellow, green, cyan, blue, purple, magenta };

const int xSize = kMatrixWidth;
const int ySize = kMatrixHeight;
SimpleLife* life;
Quadlife* quadlife;
const int speed = 20;
int initialDelay = 0;
int xSpeed = 0;
int ySpeed = 0;

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

// the setup() method runs once, when the sketch starts
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  Entropy.Initialize();
  randomSeed(Entropy.random());

  matrix.addLayer(&backgroundLayer);
  matrix.begin();
  matrix.setBrightness(defaultBrightness);
  backgroundLayer.enableColorCorrection(true);

  life = new SimpleLife(xSize, ySize);
  quadlife = new Quadlife();
}

// the loop() method runs over and over again,
// as long as the board has power
void loop() {
  // clear screen
  backgroundLayer.fillScreen(dead);
  backgroundLayer.swapBuffers(true);

  initialDelay = 0;
  start();
  life->iterateLive([](int x, int y, int on) {
    backgroundLayer.drawPixel(x, y, colors[on]);
  });
  backgroundLayer.swapBuffers(true);

  int looksDead = 0;
  int lastcrc = 0;
  delay(initialDelay);

  for (int l = 1; l <= 8000; l++) {
    delay(speed);
    life->nextGeneration();
    int crc = 0;
    backgroundLayer.fillScreen(dead);
    //life->iterateLive(update);

    life->iterateLive([&crc](int x, int y, int on) {
      backgroundLayer.drawPixel(x, y, colors[on]);
      for (byte tempI = 8; tempI; tempI--) {
        byte sum = (crc ^ on) & 0x01;
        crc >>= 1;
        if (sum) {
          crc ^= 0x8C;
        }
        on >>= 1;
      }
    });
    backgroundLayer.swapBuffers(true);

    if (l % 12 == 0) {
      if (crc == lastcrc) {
        looksDead++;
      } else {
        looksDead = 0;
      }
      if (looksDead > 10) break;
      lastcrc = crc;
    }
  }
}

void start() {
  int r = random(100);
  if (r < 10) {
    ASJ2023();
  } else if (r == 10) {
    snarkcatalystvariants();
  } else if (r == 11) {
    tannersp46gun();
  } else {
    startRandom();
  }
}

void startRandom() {
  float prob = .25;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {
      int r = random(256);
      int on = r / 256. < prob ? 1 + (r % (nColors - 1)) : 0;
      life->set(x, y, on);
    }
  }
}

void ASJ2023() {
  initialDelay = 1000;
  int x = 30, y = 22;
  const char* rle = R"(
    6.2B5.4C5.5A$5.B2.B3.C4.C6.A$4.B4.B2.C11.A$4.B4.B2.C11.A$4.B4.B3.4C7.
    A$4.6B7.C6.A$4.B4.B7.C6.A$4.B4.B2.C4.C2.A3.A$4.B4.B3.4C4.3A5$.4D5.2E5.
    4F3.6G$D4.D3.E2.E3.F4.F7.G$D4.D2.E4.E2.F4.F6.G$5.D2.E4.E7.F5.G$4.D3.E
    4.E6.F5.3G$2.2D4.E4.E4.2F9.G$.D6.E4.E3.F11.G$D8.E2.E3.F7.G4.G$6D4.2E4.
    6F3.4G!
    )";
  loadrle((xSize - x) / 2, (ySize - y) / 2, rle);
}

void snarkcatalystvariants() {
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
  loadrle((xSize - x) / 2, (ySize - y) / 2, rle);
}

void tannersp46gun() {
  //#N tannersp46gun.rle
  //#C https://conwaylife.com/wiki/Tanner%27s_p46
  //#C https://www.conwaylife.com/patterns/tannersp46gun.rle
  int x = 31, y = 44;
  const char* rle = R"(
    17.2B5.2C$17.2B5.2C11$17.B7.C$15.B.2B5.2C.C$15.B3.2B.2C3.C$16.B3.B.C3.
    C$17.3B3.3C10$14.A14.A$13.3A14.A$12.A.A.A11.3A$12.A.A.A$10.2A.3A.2A$9.
    A.2A.A.2A.A$3.2D3.2A.A5.A.2A$3.2D4.2A.A3.A.2A2.A.2A$10.3A3.3A3.2A.A2$
    2.2D$3.D$3D$D13.D$13.D.D.D.2D$12.D.2D.2D.D$12.D$11.2D!
    )";
  loadrle(0, 0, rle);
}

void lobstr() {
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
  loadrle((xSize - x) / 2, (ySize - y) / 2, rle);
}

void loadrle(int xOff, int yOff, const char* rle) {
  life->clear();

  char* p = rle;
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
