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
int initialDelay = 0;

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
    
    initialDelay = 0;
    start();

    int looksDead = 0;
    int lastcrc = 0;
    backgroundLayer.swapBuffers(true);
    delay(initialDelay);
    
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

void start() {
  int r = random(100);
  if (r < 10) {
    ASJ2023();
  } else if (r == 10) {
    snarkcatalystvariants();
  } else if ( r == 11) {
    tannersp46gun();
  } else {
    startRandom();
  }
}

void startRandom() {
    float prob = .25;
    for (int x=0; x<xSize; x++) {
      for (int y=0; y<ySize; y++) {
        int r = random(256);
        int on = r/256.<prob ? 1+(r%(nColors-1)) : 0;
        board[x][y] = on;
        backgroundLayer.drawPixel(x,y,colors[on]);
      }
    }
}

void ASJ2023() {
  initialDelay = 1000;
  int x = 30, y = 22;
  const char* rle = 
    "6.2B5.4C5.5A$5.B2.B3.C4.C6.A$4.B4.B2.C11.A$4.B4.B2.C11.A$4.B4.B3.4C7."
    "A$4.6B7.C6.A$4.B4.B7.C6.A$4.B4.B2.C4.C2.A3.A$4.B4.B3.4C4.3A5$.4D5.2B5."
    "4C3.6A$D4.D3.B2.B3.C4.C7.A$D4.D2.B4.B2.C4.C6.A$5.D2.B4.B7.C5.A$4.D3.B"
    "4.B6.C5.3A$2.2D4.B4.B4.2C9.A$.D6.B4.B3.C11.A$D8.B2.B3.C7.A4.A$6D4.2B4."
    "6C3.4A!";
  loadrle((xSize-x)/2, (ySize-y)/2, rle);
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
  const char* rle = 
    "20.2A$20.A.A$22.A4.2A$18.4A.2A2.A2.A$18.A2.A.A.A.A.2A$21.A.A.A.A$22.2A"
    ".A.A$26.A2$12.2A$13.A7.2A$13.A.A5.2A$14.2A25.B$39.3B$38.B$38.2B3$46.2B"
    "$24.2A21.B$24.A22.B.2B$14.3A8.3A11.2B4.3B2.B$4.D11.A10.A11.2B3.B3.2B$"
    "2.5D8.A5.2D21.4B$.D5.D13.D8.2B15.B$.D2.3D12.D.D7.B.B12.3B$2D.D15.2D8."
    "B13.B$D2.4D21.2B14.5B$.2D3.D3.2D11.C22.B2.B$3.3D4.2D11.3C22.2B$3.D22."
    "C$2D.D21.2C$2D.2D3$11.2D$12.D$9.3D$9.D25.2C$28.2C5.C.C$28.2C7.C$37.2C"
    "2$24.C$23.C.C.2C4.2C$23.C.C.C.C2.C2.C$22.2C.C.C.C3.2C$23.C2.2C.4C$23."
    "C4.C3.C$24.3C.C2.C$26.C.C.C$29.C!";
  loadrle((xSize-x)/2, (ySize-y)/2, rle);
}

void tannersp46gun() {
  //#N tannersp46gun.rle
  //#C https://conwaylife.com/wiki/Tanner%27s_p46
  //#C https://www.conwaylife.com/patterns/tannersp46gun.rle
  int x = 31, y = 44;
  const char* rle =
    "17.2B5.2C$17.2B5.2C11$17.B7.C$15.B.2B5.2C.C$15.B3.2B.2C3.C$16.B3.B.C3."
    "C$17.3B3.3C10$14.A14.A$13.3A14.A$12.A.A.A11.3A$12.A.A.A$10.2A.3A.2A$9."
    "A.2A.A.2A.A$3.2D3.2A.A5.A.2A$3.2D4.2A.A3.A.2A2.A.2A$10.3A3.3A3.2A.A2$"
    "2.2D$3.D$3D$D13.D$13.D.D.D.2D$12.D.2D.2D.D$12.D$11.2D!";
  loadrle(0, 0, rle);
}

void loadrle(int xOff, int yOff, const char* rle) {
  for (int x=0; x<xSize; x++) {
    for (int y=0; y<ySize; y++) {
      board[x][y] = 0;
    }
  }

  char* p = rle;
  int count = 0;
  int x = xOff;
  int y = yOff;
  for (;;) {
    char c = *(p++);
    if (c == '!' || c == 0) {
      return;
    } else if (c >= '0' && c <= '9') {
      count = count*10 + c - '0';
    } else {
      if (count == 0) count = 1;
      if (c == 'b' || c == '.') {
        x += count;
      } else if (c == 'o') {
        byte on = 1;
        for (int i=0; i<count; i++) {
            board[x][y] = on;
            backgroundLayer.drawPixel(x,y,colors[on]);
            x++;
        }
      } else if (c >= 'A' && c <= 'D') {
        byte on = 1 + c - 'A';
        for (int i=0; i<count; i++) {
            board[x][y] = on;
            backgroundLayer.drawPixel(x,y,colors[on]);
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
