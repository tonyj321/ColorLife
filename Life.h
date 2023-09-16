#ifndef Life_h
#define Life_h

#include <functional>

// https://conwaylife.com/wiki/Colourised_Life

class Colorizer {
public:
  Colorizer(int nColors)
    : nColors(nColors){};
  int getNColors() {
    return nColors;
  }
  virtual int colorIndexForNewLife(int* neighborIndexes) = 0;
private:
  int nColors;
};

class Quadlife : public Colorizer {
public:
  Quadlife()
    : Colorizer(4){};
private:
  int colorIndexForNewLife(int* neighborIndexes) {
    int count[4];
    memset(count, 0, sizeof(count));

    for (int i = 0; i < 3; i++) {
      int index = neighborIndexes[i];
      if (++count[index] > 1) return index;
    }
    for (int i = 0; i < 4; i++) {
      if (count[i] == 0) return i;
    }
    // Logically, we can't get here, but the compiler does not know that
    return 0;
  }
};

class Niemieclife : public Colorizer {
public:
  Niemieclife()
    : Colorizer(8){};
private:
  int colorIndexForNewLife(int* neighborIndexes) {
    int count[8];
    int set[2];
    memset(count, 0, sizeof(count));
    memset(set, 0, sizeof(set));
    //Three cells of the same colour produce a child of the same colour (x+x+x→x)
    //Two cells of any one colour plus one of any colour favour the dominant colour (x+x+y→x)
    for (int i = 0; i < 3; i++) {
      int index = neighborIndexes[i];
      if (++count[index] > 1) return index;
      set[index / 4]++;
    }
    //Three cells of different colours in the same set produce the complement of the fourth colour (x+y+z→w');
    for (int s = 0; s < 2; s++) {
      if (set[s] == 3) {
        for (int i = 0; i < 4; i++) {
          if (count[i + s * 4] == 0) return i + (1 - s) * 4;
        }
      }
    }
    //Two complementary cells cancel out (x+x'+y→y)
    for (int i = 0; i < 4; i++) {
      if (count[i] == 1 && count[i + 4] == 1) {
        for (int j = 0; j < 8; j++) {
          if (j % 4 != i && count[j] == 1) {
            return j;
          }
        }
      }
    }
    //Two cells of different colours from one set plus one from the other set favour the solitary one (x+y+z'→z').
    for (int s = 0; s < 2; s++) {
      if (set[s] == 2) {
        for (int i = 0; i < 4; i++) {
          if (count[i + (1 - s) * 4] == 1) return i + (1 - s) * 4;
        }
      }
    }
    // Logically, we can't get here, but the compiler does not know that
    return 0;
  }
};


class Life {
public:
  // constructors
  Life(int bitsPerPixel);
  // methods
  void setPixel(int x, int y, int value);
  void iterate(void (*functionPointer)(int x, int y, int value));
private:
};

class SimpleLife {
public:
  SimpleLife(int w, int h)
    : width(w), height(h), colorizer(new Niemieclife()) {
    data1 = (byte*)malloc(sizeof(byte) * w * h);
    data2 = (byte*)malloc(sizeof(byte) * w * h);
    data = data1;
    next = data2;
    clear();
  }
  // methods
  void clear() {
    memset(data, 0, sizeof(byte) * width * height);
  }
  byte get(int x, int y) {
    if (x < 0 || x >= width) return 0;
    if (y < 0 || y >= height) return 0;
    return data[x + y * width];
  }
  void set(int x, int y, byte value) {
    if (x < 0 || x >= width) return;
    if (y < 0 || y >= height) return;
    data[x + y * width] = value;
  }
  void nextGeneration() {
    iterateNeighborhood([this](int x, int y, int value, int* neighbors) {
      // First decide if the cell is alive
      int count = 0;
      for (int n = 0; n < 8; n++) {
        if (neighbors[n]) count++;
      }
      int nextValue;
      bool live = value ? (count == 2 || count == 3) : (count == 3);
      if (live) {
        if (!value) {
          int liveNeighbors[3];
          int l = 0;
          for (int n = 0; n < 8; n++) {
            if (neighbors[n]) liveNeighbors[l++] = neighbors[n] - 1;
          }
          nextValue = colorizer->colorIndexForNewLife(liveNeighbors) + 1;
        } else {
          nextValue = value;
        }
      } else {
        nextValue = 0;
      }
      next[x + y * width] = nextValue;
    });
    byte* temp = data;
    data = next;
    next = temp;
  }
  void iterateNeighborhood(std::function<void(int x, int y, int value, int* neighbors)> lambda) {
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        int neighbors[] = {
          get(x - 1, y - 1), get(x, y - 1), get(x + 1, y - 1),
          get(x - 1, y), get(x + 1, y),
          get(x - 1, y + 1), get(x, y + 1), get(x + 1, y + 1)
        };
        lambda(x, y, get(x, y), neighbors);
      }
    }
  }
  void iterateLive(std::function<void(int x, int y, int value)> functionPointer) {
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        byte v = get(x, y);
        if (v != 0) {
          functionPointer(x, y, get(x, y));
        }
      }
    }
  }

private:
  int width;
  int height;
  byte* data1;
  byte* data2;
  byte* data;
  byte* next;
  Colorizer* colorizer;
};
#endif