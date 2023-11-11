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
            if (x >= 0 && x < xViewportSize && y >= 0 && y < yViewportSize) {
                backgroundLayer->drawPixel(x, y, colors[on]);
            }
        });
        backgroundLayer->swapBuffers(true);
        delay(initialDelay);
        int lastcrc = 0;
        int looksDead = 0;
        int xMin = xViewportMin;
        int yMin = yViewportMin;
        for (int l = 1; l <= 8000; l++) {
            delay(speed);
            lifeImplementation.nextGeneration();
            int crc = 0;
            backgroundLayer->fillScreen(colors[0]);

            if (speedDivisor > 0 && l % speedDivisor == 0) {
                xMin += speedX;
                yMin += speedY;
            }

            lifeImplementation.iterateLive([this, &crc, &xMin, &yMin](int x, int y, int on) {
                if (x >= xMin && x-xMin < xViewportSize && y >= yMin && y-yMin < yViewportSize) {
                    backgroundLayer->drawPixel(x-xMin, y-yMin, colors[on]);
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

    virtual void setViewport(int x, int y, int width, int height) {
        xViewportMin = x;
        yViewportMin = y;
        xViewportSize = width;
        yViewportSize = height;
    }

    virtual void setViewportSpeed(int x, int y, int divisor) {
        speedX = x;
        speedY = y;
        speedDivisor = divisor;
    }

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
    int xViewportMin = 0;
    int yViewportMin = 0;
    int xViewportSize = 64;
    int yViewportSize = 64;
    int speedX = 0;
    int speedY = 0;
    int speedDivisor = 0;
    SMLayerBackground<rgb24, 0U> *backgroundLayer;
    int nColors;
    const rgb24 *colors;
};

#endif