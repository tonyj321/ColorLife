#ifndef LEDMatrixLife_h
#define LEDMatrixLife_h

#include <Arduino.h>
#include <SmartMatrix.h>

#include "Life.h"

class LEDMatrixLife {
   public:
    LEDMatrixLife(Life &implementation,
                  SMLayerBackground<rgb24, 0U> *backgroundLayer)
        : lifeImplementation(implementation),
          backgroundLayer(backgroundLayer) {}

    // methods
    virtual void clear() { lifeImplementation.clear(); }

    virtual void setRule(int nStates, TreeRule* treeRule) {
        lifeImplementation.setRule(nStates, treeRule);
    }

    virtual void set(int x, int y, byte value) {
        lifeImplementation.set(x, y, value);
    }

    virtual void run() {
        backgroundLayer->fillScreen(colors[0]);
        lifeImplementation.iterateLive([this](int x, int y, int on) {
            if (x >= 0 && x < xSize && y >= 0 && y < ySize) {
                backgroundLayer->drawPixel(x, y, colors[on]);
            }
        });
        backgroundLayer->swapBuffers(true);
        delay(initialDelay);
        int lastcrc = 0;
        int looksDead = 0;
        for (int l = 1; l <= 8000; l++) {
            delay(speed);
            lifeImplementation.nextGeneration();
            int crc = 0;
            backgroundLayer->fillScreen(colors[0]);

            lifeImplementation.iterateLive([this, &crc](int x, int y, int on) {
                if (x >= 0 && x < xSize && y >= 0 && y < ySize) {
                    backgroundLayer->drawPixel(x, y, colors[on]);
                    for (byte tempI = 8; tempI; tempI--) {
                        byte sum = (crc ^ on) & 0x01;
                        crc >>= 1;
                        if (sum) {
                            crc ^= 0x8C;
                        }
                        on >>= 1;
                    }
                }
            });
            backgroundLayer->swapBuffers(true);

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

    virtual void setViewport(int x, int y, int width, int height) {}

    virtual void setColorMap(int nColors, const rgb24 *colors) {
        this->nColors = nColors;
        this->colors = colors;
    }

    virtual void setInitialDelay(int initialDelay) {
        this->initialDelay = initialDelay;
    }

   private:
    Life &lifeImplementation;
    int initialDelay = 0;
    int speed = 20;
    int xSize = 64;
    int ySize = 64;
    SMLayerBackground<rgb24, 0U> *backgroundLayer;
    int nColors;
    const rgb24 *colors;
};

#endif