#define USE_ADAFRUIT_GFX_LAYERS

#include <Entropy.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>        // SmartLED Shield for Teensy 4 (V5)
#include <SmartMatrix.h>

#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 64;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 64;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SMARTMATRIX_HUB75_64ROW_MOD32SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

const int defaultBrightness = 100*(255/100);    // full brightness
const rgb24 dead = {0, 0, 0};
const rgb24 green  = {0,255,0};
const rgb24 red    = {255,0,0};
const rgb24 purple = {178,102,255};
const rgb24 yellow   = {255,255,0};
const int nColors = 5;
const rgb24 colors[nColors] = {dead, green, red, purple, yellow };

const int xSize = kMatrixWidth;  
const int ySize = kMatrixHeight;  
byte** board;
const int speed = 20;

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

    board = (byte**) malloc(sizeof(byte*) * xSize);
    for (int x=0; x<xSize; x++) {
      board[x] = (byte*) malloc(sizeof(byte) * ySize);
    }
}

// the loop() method runs over and over again,
// as long as the board has power
void loop() {    
    // clear screen
    backgroundLayer.fillScreen(dead);
    backgroundLayer.swapBuffers(true);
    
    float prob = .25;
    for (int x=0; x<xSize; x++) {
      for (int y=0; y<ySize; y++) {
        int r = random(256);
        int on = r/256.<prob ? 1+(r%(nColors-1)) : 0;
        board[x][y] = on;
        backgroundLayer.drawPixel(x,y,colors[on]);
      }
    }
    int looksDead = 0;
    int lastcrc = 0;
    backgroundLayer.swapBuffers(true);
    for (int l=0;l<8000;l++) { 
      delay(speed);
      byte board2[xSize][ySize];
      byte crc = 0;
      for (int x=0; x<xSize; x++) {
        for (int y=0; y<ySize; y++) {
          int colorCount[nColors];
          for (int c=0; c<nColors; c++) colorCount[c] = 0;
          colorCount[get(x-1, y-1)]++;
          colorCount[get(x  , y-1)]++;
          colorCount[get(x+1, y-1)]++;
          colorCount[get(x-1, y)]++;
          colorCount[get(x+1, y)]++;
          colorCount[get(x-1, y+1)]++;
          colorCount[get(x  , y+1)]++;
          colorCount[get(x+1, y+1)]++;    
          int count = colorCount[1] + colorCount[2] + colorCount[3] + colorCount[4];
          int majority = 0;
          if (colorCount[1]>1)  majority = 1;
          else if (colorCount[2]>1) majority = 2;
          else if (colorCount[3]>1) majority = 3;
          else if (colorCount[4]>1) majority = 4;
          else {
            if (colorCount[1]==0)  majority = 1;
            else if (colorCount[2]==0) majority = 2;
            else if (colorCount[3]==0) majority = 3;
            else if (colorCount[4]==0) majority = 4;            
          }      
          byte wasLive = board[x][y];
          int on = wasLive ? ((count==2 || count==3) ? wasLive : 0) : ((count==3) ? majority : 0); 
          board2[x][y] = on;
          backgroundLayer.drawPixel(x,y,colors[on]);
          for (byte tempI = 8; tempI; tempI--) {
              byte sum = (crc ^ wasLive) & 0x01;
              crc >>= 1;
              if (sum) {
                 crc ^= 0x8C;
              }
              wasLive >>= 1;
            }          
         }
      }
      if (l%12 == 0) {
         if (crc == lastcrc) { looksDead++; }
         else { looksDead = 0; }
         if (looksDead > 10) break;
         lastcrc = crc;
      } 
      for (int x=0; x<xSize; x++) {
        for (int y=0; y<ySize; y++) {
          board[x][y] = board2[x][y];
        }
      }
      backgroundLayer.swapBuffers(true);
    }
}

int get(int x, int y) {
  if (x<0 || x>=xSize) return 0;
  if (y<0 || y>=ySize) return 0;
  return board[x][y];
}
