#ifndef Life_h
#define Life_h

#include <assert.h>
#include <climits>
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

class TreeRule {
public:
  virtual int transition(int* neighbors) = 0;
};

class GenerationsTreeRule : public TreeRule {
  public:
    int transition(int* neighbors) {
      int node = 37;
      int* n = neighbors;
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      node = lookup[node][*(n++)];
      return node;

    }
  private:
    const byte lookup[38][8] = {
      {0,2,3,4,5,6,7,0},
      {0,1,3,4,5,6,7,0},
      {0,1,0,0,0,0,0,0},
      {1,1,1,1,1,1,1,1},
      {2,3,2,2,2,2,2,2},
      {3,3,3,3,3,3,3,3},
      {4,5,4,4,4,4,4,4},
      {1,1,3,4,5,6,7,0},
      {1,7,1,1,1,1,1,1},
      {3,8,3,3,3,3,3,3},
      {5,9,5,5,5,5,5,5},
      {6,10,6,6,6,6,6,6},
      {7,7,7,7,7,7,7,7},
      {8,12,8,8,8,8,8,8},
      {9,13,9,9,9,9,9,9},
      {10,14,10,10,10,10,10,10},
      {11,15,11,11,11,11,11,11},
      {1,2,3,4,5,6,7,0},
      {7,17,7,7,7,7,7,7},
      {12,18,12,12,12,12,12,12},
      {13,19,13,13,13,13,13,13},
      {14,20,14,14,14,14,14,14},
      {15,21,15,15,15,15,15,15},
      {16,22,16,16,16,16,16,16},
      {17,17,17,17,17,17,17,17},
      {18,24,18,18,18,18,18,18},
      {19,25,19,19,19,19,19,19},
      {20,26,20,20,20,20,20,20},
      {21,27,21,21,21,21,21,21},
      {22,28,22,22,22,22,22,22},
      {23,29,23,23,23,23,23,23},
      {24,24,24,24,24,24,24,24},
      {25,31,25,25,25,25,25,25},
      {26,32,26,26,26,26,26,26},
      {27,33,27,27,27,27,27,27},
      {28,34,28,28,28,28,28,28},
      {29,35,29,29,29,29,29,29},
      {30,36,30,30,30,30,30,30}
    };
};

class NiemiecTreeRule : public TreeRule {
public:
  int transition(int* neighbors) {
    int node = 271;
    int* n = neighbors;
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    node = lookup[node][*(n++)];
    return node;
  }
private:
  const short lookup[272][9] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 0, 2, 2, 2, 2, 2, 2, 2, 2 },
    { 1, 3, 3, 3, 3, 3, 3, 3, 3 },
    { 1, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 2, 5, 5, 5, 5, 5, 5, 5, 5 },
    { 2, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 8, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 7, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 2, 5, 7, 8, 9, 7, 5, 9, 8 },
    { 3, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 6, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 2, 5, 8, 11, 12, 11, 12, 5, 8 },
    { 4, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 2, 5, 9, 12, 14, 14, 12, 9, 5 },
    { 5, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 2, 5, 7, 11, 14, 16, 12, 9, 8 },
    { 2, 5, 5, 12, 12, 12, 12, 5, 5 },
    { 2, 5, 9, 5, 9, 9, 5, 9, 5 },
    { 2, 5, 8, 8, 5, 8, 5, 5, 8 },
    { 3, 6, 10, 13, 15, 17, 18, 19, 20 },
    { 2, 7, 7, 7, 7, 7, 7, 7, 7 },
    { 2, 8, 7, 11, 16, 16, 11, 7, 8 },
    { 2, 9, 7, 16, 14, 16, 14, 9, 7 },
    { 2, 7, 7, 16, 16, 16, 16, 7, 7 },
    { 2, 9, 7, 7, 9, 7, 9, 9, 7 },
    { 2, 8, 7, 8, 7, 7, 8, 7, 8 },
    { 3, 10, 22, 23, 24, 25, 17, 26, 27 },
    { 2, 11, 11, 11, 11, 11, 11, 11, 11 },
    { 2, 12, 16, 11, 14, 16, 12, 14, 11 },
    { 2, 11, 16, 11, 16, 16, 11, 16, 11 },
    { 2, 12, 11, 11, 12, 11, 12, 12, 11 },
    { 2, 8, 8, 11, 11, 11, 11, 8, 8 },
    { 3, 13, 23, 29, 30, 31, 32, 17, 33 },
    { 2, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 2, 14, 16, 16, 14, 16, 14, 14, 16 },
    { 2, 12, 14, 12, 14, 14, 12, 14, 12 },
    { 2, 9, 9, 14, 14, 14, 14, 9, 9 },
    { 3, 15, 24, 30, 35, 36, 37, 38, 17 },
    { 2, 16, 16, 16, 16, 16, 16, 16, 16 },
    { 3, 17, 25, 31, 36, 40, 30, 24, 23 },
    { 2, 12, 12, 12, 12, 12, 12, 12, 12 },
    { 3, 18, 17, 32, 37, 30, 42, 15, 13 },
    { 2, 9, 9, 9, 9, 9, 9, 9, 9 },
    { 3, 19, 26, 17, 38, 24, 15, 44, 10 },
    { 2, 8, 8, 8, 8, 8, 8, 8, 8 },
    { 3, 20, 27, 33, 17, 23, 13, 10, 46 },
    { 4, 21, 28, 34, 39, 41, 43, 45, 47 },
    { 5, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 6, 49, 49, 49, 49, 49, 49, 49, 49 },
    { 7, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 8, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 9, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 10, 49, 51, 52, 53, 51, 49, 53, 52 },
    { 11, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 12, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 13, 49, 52, 55, 56, 55, 56, 49, 52 },
    { 14, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 49, 53, 56, 58, 58, 56, 53, 49 },
    { 16, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 17, 49, 51, 55, 58, 60, 56, 53, 52 },
    { 18, 49, 49, 56, 56, 56, 56, 49, 49 },
    { 19, 49, 53, 49, 53, 53, 49, 53, 49 },
    { 20, 49, 52, 52, 49, 52, 49, 49, 52 },
    { 21, 50, 54, 57, 59, 61, 62, 63, 64 },
    { 22, 51, 51, 51, 51, 51, 51, 51, 51 },
    { 23, 52, 51, 55, 60, 60, 55, 51, 52 },
    { 24, 53, 51, 60, 58, 60, 58, 53, 51 },
    { 25, 51, 51, 60, 60, 60, 60, 51, 51 },
    { 26, 53, 51, 51, 53, 51, 53, 53, 51 },
    { 27, 52, 51, 52, 51, 51, 52, 51, 52 },
    { 28, 54, 66, 67, 68, 69, 61, 70, 71 },
    { 29, 55, 55, 55, 55, 55, 55, 55, 55 },
    { 30, 56, 60, 55, 58, 60, 56, 58, 55 },
    { 31, 55, 60, 55, 60, 60, 55, 60, 55 },
    { 32, 56, 55, 55, 56, 55, 56, 56, 55 },
    { 33, 52, 52, 55, 55, 55, 55, 52, 52 },
    { 34, 57, 67, 73, 74, 75, 76, 61, 77 },
    { 35, 58, 58, 58, 58, 58, 58, 58, 58 },
    { 36, 58, 60, 60, 58, 60, 58, 58, 60 },
    { 37, 56, 58, 56, 58, 58, 56, 58, 56 },
    { 38, 53, 53, 58, 58, 58, 58, 53, 53 },
    { 39, 59, 68, 74, 79, 80, 81, 82, 61 },
    { 40, 60, 60, 60, 60, 60, 60, 60, 60 },
    { 41, 61, 69, 75, 80, 84, 74, 68, 67 },
    { 42, 56, 56, 56, 56, 56, 56, 56, 56 },
    { 43, 62, 61, 76, 81, 74, 86, 59, 57 },
    { 44, 53, 53, 53, 53, 53, 53, 53, 53 },
    { 45, 63, 70, 61, 82, 68, 59, 88, 54 },
    { 46, 52, 52, 52, 52, 52, 52, 52, 52 },
    { 47, 64, 71, 77, 61, 67, 57, 54, 90 },
    { 48, 65, 72, 78, 83, 85, 87, 89, 91 },
    { 49, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 50, 93, 93, 93, 93, 93, 93, 93, 93 },
    { 51, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 52, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 53, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 54, 93, 95, 96, 97, 95, 93, 97, 96 },
    { 55, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 56, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 57, 93, 96, 99, 100, 99, 100, 93, 96 },
    { 58, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 59, 93, 97, 100, 102, 102, 100, 97, 93 },
    { 60, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 61, 93, 95, 99, 102, 104, 100, 97, 96 },
    { 62, 93, 93, 100, 100, 100, 100, 93, 93 },
    { 63, 93, 97, 93, 97, 97, 93, 97, 93 },
    { 64, 93, 96, 96, 93, 96, 93, 93, 96 },
    { 65, 94, 98, 101, 103, 105, 106, 107, 108 },
    { 66, 95, 95, 95, 95, 95, 95, 95, 95 },
    { 67, 96, 95, 99, 104, 104, 99, 95, 96 },
    { 68, 97, 95, 104, 102, 104, 102, 97, 95 },
    { 69, 95, 95, 104, 104, 104, 104, 95, 95 },
    { 70, 97, 95, 95, 97, 95, 97, 97, 95 },
    { 71, 96, 95, 96, 95, 95, 96, 95, 96 },
    { 72, 98, 110, 111, 112, 113, 105, 114, 115 },
    { 73, 99, 99, 99, 99, 99, 99, 99, 99 },
    { 74, 100, 104, 99, 102, 104, 100, 102, 99 },
    { 75, 99, 104, 99, 104, 104, 99, 104, 99 },
    { 76, 100, 99, 99, 100, 99, 100, 100, 99 },
    { 77, 96, 96, 99, 99, 99, 99, 96, 96 },
    { 78, 101, 111, 117, 118, 119, 120, 105, 121 },
    { 79, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 80, 102, 104, 104, 102, 104, 102, 102, 104 },
    { 81, 100, 102, 100, 102, 102, 100, 102, 100 },
    { 82, 97, 97, 102, 102, 102, 102, 97, 97 },
    { 83, 103, 112, 118, 123, 124, 125, 126, 105 },
    { 84, 104, 104, 104, 104, 104, 104, 104, 104 },
    { 85, 105, 113, 119, 124, 128, 118, 112, 111 },
    { 86, 100, 100, 100, 100, 100, 100, 100, 100 },
    { 87, 106, 105, 120, 125, 118, 130, 103, 101 },
    { 88, 97, 97, 97, 97, 97, 97, 97, 97 },
    { 89, 107, 114, 105, 126, 112, 103, 132, 98 },
    { 90, 96, 96, 96, 96, 96, 96, 96, 96 },
    { 91, 108, 115, 121, 105, 111, 101, 98, 134 },
    { 92, 109, 116, 122, 127, 129, 131, 133, 135 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 93, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 94, 138, 138, 138, 138, 138, 138, 138, 138 },
    { 95, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 96, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 97, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 98, 138, 140, 141, 142, 140, 138, 142, 141 },
    { 99, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 100, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 101, 138, 141, 144, 145, 144, 145, 138, 141 },
    { 102, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 103, 138, 142, 145, 147, 147, 145, 142, 138 },
    { 104, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 105, 138, 140, 144, 147, 149, 145, 142, 141 },
    { 106, 138, 138, 145, 145, 145, 145, 138, 138 },
    { 107, 138, 142, 138, 142, 142, 138, 142, 138 },
    { 108, 138, 141, 141, 138, 141, 138, 138, 141 },
    { 109, 139, 143, 146, 148, 150, 151, 152, 153 },
    { 110, 140, 140, 140, 140, 140, 140, 140, 140 },
    { 111, 141, 140, 144, 149, 149, 144, 140, 141 },
    { 112, 142, 140, 149, 147, 149, 147, 142, 140 },
    { 113, 140, 140, 149, 149, 149, 149, 140, 140 },
    { 114, 142, 140, 140, 142, 140, 142, 142, 140 },
    { 115, 141, 140, 141, 140, 140, 141, 140, 141 },
    { 116, 143, 155, 156, 157, 158, 150, 159, 160 },
    { 117, 144, 144, 144, 144, 144, 144, 144, 144 },
    { 118, 145, 149, 144, 147, 149, 145, 147, 144 },
    { 119, 144, 149, 144, 149, 149, 144, 149, 144 },
    { 120, 145, 144, 144, 145, 144, 145, 145, 144 },
    { 121, 141, 141, 144, 144, 144, 144, 141, 141 },
    { 122, 146, 156, 162, 163, 164, 165, 150, 166 },
    { 123, 147, 147, 147, 147, 147, 147, 147, 147 },
    { 124, 147, 149, 149, 147, 149, 147, 147, 149 },
    { 125, 145, 147, 145, 147, 147, 145, 147, 145 },
    { 126, 142, 142, 147, 147, 147, 147, 142, 142 },
    { 127, 148, 157, 163, 168, 169, 170, 171, 150 },
    { 128, 149, 149, 149, 149, 149, 149, 149, 149 },
    { 129, 150, 158, 164, 169, 173, 163, 157, 156 },
    { 130, 145, 145, 145, 145, 145, 145, 145, 145 },
    { 131, 151, 150, 165, 170, 163, 175, 148, 146 },
    { 132, 142, 142, 142, 142, 142, 142, 142, 142 },
    { 133, 152, 159, 150, 171, 157, 148, 177, 143 },
    { 134, 141, 141, 141, 141, 141, 141, 141, 141 },
    { 135, 153, 160, 166, 150, 156, 146, 143, 179 },
    { 136, 154, 161, 167, 172, 174, 176, 178, 180 },
    { 137, 137, 137, 137, 137, 137, 137, 137, 137 },
    { 138, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 139, 183, 183, 183, 183, 183, 183, 183, 183 },
    { 140, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 141, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 142, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 143, 183, 185, 186, 187, 185, 183, 187, 186 },
    { 144, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 145, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 146, 183, 186, 189, 190, 189, 190, 183, 186 },
    { 147, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 148, 183, 187, 190, 192, 192, 190, 187, 183 },
    { 149, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 150, 183, 185, 189, 192, 194, 190, 187, 186 },
    { 151, 183, 183, 190, 190, 190, 190, 183, 183 },
    { 152, 183, 187, 183, 187, 187, 183, 187, 183 },
    { 153, 183, 186, 186, 183, 186, 183, 183, 186 },
    { 154, 184, 188, 191, 193, 195, 196, 197, 198 },
    { 155, 185, 185, 185, 185, 185, 185, 185, 185 },
    { 156, 186, 185, 189, 194, 194, 189, 185, 186 },
    { 157, 187, 185, 194, 192, 194, 192, 187, 185 },
    { 158, 185, 185, 194, 194, 194, 194, 185, 185 },
    { 159, 187, 185, 185, 187, 185, 187, 187, 185 },
    { 160, 186, 185, 186, 185, 185, 186, 185, 186 },
    { 161, 188, 200, 201, 202, 203, 195, 204, 205 },
    { 162, 189, 189, 189, 189, 189, 189, 189, 189 },
    { 163, 190, 194, 189, 192, 194, 190, 192, 189 },
    { 164, 189, 194, 189, 194, 194, 189, 194, 189 },
    { 165, 190, 189, 189, 190, 189, 190, 190, 189 },
    { 166, 186, 186, 189, 189, 189, 189, 186, 186 },
    { 167, 191, 201, 207, 208, 209, 210, 195, 211 },
    { 168, 192, 192, 192, 192, 192, 192, 192, 192 },
    { 169, 192, 194, 194, 192, 194, 192, 192, 194 },
    { 170, 190, 192, 190, 192, 192, 190, 192, 190 },
    { 171, 187, 187, 192, 192, 192, 192, 187, 187 },
    { 172, 193, 202, 208, 213, 214, 215, 216, 195 },
    { 173, 194, 194, 194, 194, 194, 194, 194, 194 },
    { 174, 195, 203, 209, 214, 218, 208, 202, 201 },
    { 175, 190, 190, 190, 190, 190, 190, 190, 190 },
    { 176, 196, 195, 210, 215, 208, 220, 193, 191 },
    { 177, 187, 187, 187, 187, 187, 187, 187, 187 },
    { 178, 197, 204, 195, 216, 202, 193, 222, 188 },
    { 179, 186, 186, 186, 186, 186, 186, 186, 186 },
    { 180, 198, 205, 211, 195, 201, 191, 188, 224 },
    { 181, 199, 206, 212, 217, 219, 221, 223, 225 },
    { 182, 182, 182, 182, 182, 182, 182, 182, 182 },
    { 183, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 184, 228, 228, 228, 228, 228, 228, 228, 228 },
    { 185, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 186, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 187, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 188, 228, 230, 231, 232, 230, 228, 232, 231 },
    { 189, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 190, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 191, 228, 231, 234, 235, 234, 235, 228, 231 },
    { 192, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 193, 228, 232, 235, 237, 237, 235, 232, 228 },
    { 194, 227, 227, 227, 227, 227, 227, 227, 227 },
    { 195, 228, 230, 234, 237, 239, 235, 232, 231 },
    { 196, 228, 228, 235, 235, 235, 235, 228, 228 },
    { 197, 228, 232, 228, 232, 232, 228, 232, 228 },
    { 198, 228, 231, 231, 228, 231, 228, 228, 231 },
    { 199, 229, 233, 236, 238, 240, 241, 242, 243 },
    { 200, 230, 230, 230, 230, 230, 230, 230, 230 },
    { 201, 231, 230, 234, 239, 239, 234, 230, 231 },
    { 202, 232, 230, 239, 237, 239, 237, 232, 230 },
    { 203, 230, 230, 239, 239, 239, 239, 230, 230 },
    { 204, 232, 230, 230, 232, 230, 232, 232, 230 },
    { 205, 231, 230, 231, 230, 230, 231, 230, 231 },
    { 206, 233, 245, 246, 247, 248, 240, 249, 250 },
    { 207, 234, 234, 234, 234, 234, 234, 234, 234 },
    { 208, 235, 239, 234, 237, 239, 235, 237, 234 },
    { 209, 234, 239, 234, 239, 239, 234, 239, 234 },
    { 210, 235, 234, 234, 235, 234, 235, 235, 234 },
    { 211, 231, 231, 234, 234, 234, 234, 231, 231 },
    { 212, 236, 246, 252, 253, 254, 255, 240, 256 },
    { 213, 237, 237, 237, 237, 237, 237, 237, 237 },
    { 214, 237, 239, 239, 237, 239, 237, 237, 239 },
    { 215, 235, 237, 235, 237, 237, 235, 237, 235 },
    { 216, 232, 232, 237, 237, 237, 237, 232, 232 },
    { 217, 238, 247, 253, 258, 259, 260, 261, 240 },
    { 218, 239, 239, 239, 239, 239, 239, 239, 239 },
    { 219, 240, 248, 254, 259, 263, 253, 247, 246 },
    { 220, 235, 235, 235, 235, 235, 235, 235, 235 },
    { 221, 241, 240, 255, 260, 253, 265, 238, 236 },
    { 222, 232, 232, 232, 232, 232, 232, 232, 232 },
    { 223, 242, 249, 240, 261, 247, 238, 267, 233 },
    { 224, 231, 231, 231, 231, 231, 231, 231, 231 },
    { 225, 243, 250, 256, 240, 246, 236, 233, 269 },
    { 226, 244, 251, 257, 262, 264, 266, 268, 270 }
  };
};

class Life {
public:
  // methods
  virtual void clear() = 0;
  virtual void set(int x, int y, byte value) = 0;
  virtual byte get(int x, int y) = 0;
  virtual void nextGeneration() = 0;
  virtual void iterateLive(std::function<void(int x, int y, int value)> lambda) = 0;
  virtual void dump(){};
private:
};

class InfiniteLife : public Life {
public:
  InfiniteLife(byte bitsPerPixel)
    : bitsPerPixel(bitsPerPixel), treeRule(new GenerationsTreeRule()) {
    data1 = new Data(10000);
    data2 = new Data(10000);
    data = data1;
    next = data2;
    pixelsPerData = 32 / bitsPerPixel;
    mask = (1 << bitsPerPixel) - 1;
    clear();
  }
  ~InfiniteLife() {
    free(data1);
    free(data2);
  }
  virtual void clear() {
    data->clear();
  }
  // Currently we only support calling set for increasing x,y
  virtual void set(int x, int y, byte value) {
    set(this->data, x, y, value);
  }

  virtual void dump() {
    Serial.printf("Length %d\n", data->dataLength);
    //for (int i = 0; i < dataLength; i++) {
    //  int datum = data[i];
    //Serial.printf("%d %d\n", i, datum);
    //}
  }

  virtual byte get(int x, int y) {
    return 0;
  }
  virtual void nextGeneration() {
    if (data->dataLength == 0) return;
    Row prevRow(*this);   // Row at y-1
    Row currRow(*this);   // Row at y
    Row nextRow(*this);   // Row at y+1
    Row nextLive(*this);  // Next live row after nextRow

    // Add marker to avoid having to constantly check dataLength
    data->data[data->dataLength] = -1;

    nextLive.init(data->data);
    int y = -1;

    next->clear();

    // Keeps track of the neighborhood, and calls callback to set new live cells as needed
    NeighborHood neighborhood([this](int x, int y, int* neighbors) {
      //Serial.printf("x=%d y=%d, [%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", x, y, neighbors[0], neighbors[1], neighbors[2], neighbors[3], neighbors[4], neighbors[5], neighbors[6], neighbors[7], neighbors[8]);
      set(next, x, y, treeRule->transition(neighbors));
    });
    // Loop over rows
    for (;;) {
      if (currRow.wasDead() && nextRow.wasDead()) {
        if (nextLive.wasDead()) break;
        // Skip to make nextLive the next row
        prevRow.init();
        currRow.init();
        nextRow.init(nextLive);
        nextLive.advance();
        y = nextRow.getY() - 1;
      } else {
        // Move to next row
        prevRow.init(currRow);
        //Serial.printf("Advancing currRow\n");
        currRow.init(nextRow);
        y++;
        if (nextLive.getY() == y + 1) {
          nextRow.init(nextLive);
          nextLive.advance();
        } else {
          nextRow.init();
        }
      }
      // Handle current row
      neighborhood.startRow(y);
      for (;;) {
        int x = min3(prevRow.getX(), currRow.getX(), nextRow.getX());
        if (x == INT_MAX) {
          break;
        }
        // Set neighborhoood, this will call back to do the update as necessary
        neighborhood.load(x, prevRow.getAndConditionallyIncrement(x), currRow.getAndConditionallyIncrement(x), nextRow.getAndConditionallyIncrement(x));
      }
      neighborhood.endRow(y);
    }
    Data* temp = data;
    data = next;
    next = temp;
    Serial.println(data->dataLength);
  }
  virtual void iterateLive(std::function<void(int x, int y, int value)> lambda) {
    int x = 0, y = 0;
    for (int i = 0; i < data->dataLength; i++) {
      int datum = data->data[i];
      if (datum < 0) {
        y = -datum - offset;
      } else {
        x = datum - offset;
        unsigned int value = data->data[++i];
        while (value != 0) {
          if (value & mask) {
            lambda(x, y, value & mask);
          }
          x += 1;
          value >>= bitsPerPixel;
        }
      }
    }
  }
private:
  class NeighborHood {
  public:

    NeighborHood(std::function<void(int x, int y, int* neighbors)> lambda) {
      this->lambda = lambda;
    }
    void load(int x, int px, int cx, int nx) {
      if (x - this->x == 1) {
        shiftAndCall(px, cx, nx);
      } else if (x - this->x == 2) {
        shiftAndCall(0, 0, 0);
        shiftAndCall(px, cx, nx);
      } else {
        if (this->x != INT_MIN) {
          shiftAndCall(0, 0, 0);
          shiftAndCall(0, 0, 0);
        }
        this->x = x - 1;
        shiftAndCall(px, cx, nx);
      }
    }
    void endRow(int y) {
      shiftAndCall(0, 0, 0);
      shiftAndCall(0, 0, 0);
    }
    void startRow(int y) {
      this->y = y;
      this->x = INT_MIN;
      clear();
    }
  private:
    void clear() {
      memset(neighbors, 0, sizeof(neighbors[0]) * 9);
    }
    void shiftAndCall(int a, int b, int c) {
      shift(a, b, c);
      lambda(this->x, this->y, neighbors);
      this->x++;
    }
    void shift(int a, int b, int c) {
      // Use the same order as SimpleLife
      //    get(x - 1, y - 1), get(x, y - 1), get(x + 1, y - 1),
      //    get(x - 1, y), get(x + 1, y),
      //    get(x - 1, y + 1), get(x, y + 1), get(x + 1, y + 1),
      //    get(x, y)
      neighbors[0] = neighbors[1];
      neighbors[3] = neighbors[8];
      neighbors[5] = neighbors[6];
      neighbors[1] = neighbors[2];
      neighbors[8] = neighbors[4];
      neighbors[6] = neighbors[7];
      neighbors[2] = a;
      neighbors[4] = b;
      neighbors[7] = c;
    }
    std::function<void(int x, int y, int* neighbors)> lambda;
    int neighbors[9];
    int x;
    int y;
  };

  class Row {
  public:
    Row(const InfiniteLife& parent)
      : parent(parent) {
      init();
    }
    void init(const Row& other) {
      init(other.start);
    }
    void init() {
      init(0);
    }
    void init(const int* start) {
      this->start = start;
      this->end = 0;
      value = 0;
      data = 0;
      if (start) {
        data = start;
        currY = *(data++);
        if (currY == -1) {
          currX = INT_MAX;
          currY = INT_MIN;
          dead = true;
        } else {
          currY = -currY - offset;
          currX = *(data++) - offset;
          value = *(data++);
          dead = false;
        }
      } else {
        currY = INT_MIN;
        currX = INT_MAX;
        dead = true;
      }
    }
    void advance() {
      if (!end) {
        while (*data >= 0) data += 2;
        end = data;
      }
      init(end);
    }
    int getX() {
      return currX;
    }
    int getY() {
      return currY;
    }
    int nextX() {
      if (dead) return INT_MAX;

      while (value) {
        value >>= parent.bitsPerPixel;
        currX++;
        if (value & parent.mask) {
          return currX;
        }
      }
      currX = *(data++);
      if (currX < 0) {
        end = data;
        dead = true;
        currX = INT_MAX;
      } else {
        currX -= offset;
        value = *(data++);
      }
      return currX;
    }

    int getValue() {
      return value & parent.mask;
    }

    int getAndConditionallyIncrement(int x) {
      if (x == currX) {
        int rValue = getValue();
        nextX();
        return rValue;
      } else {
        return 0;
      }
    }
    int get(int x) {
      if (currX == x) {
        return value & parent.mask;
      } else if (currX > x) {

      } else {
        assert(false);
      }
    }
    bool wasDead() {
      return start == 0 || currY == INT_MIN;
    }

  private:
    const InfiniteLife& parent;
    const int* start;
    const int* data;
    const int* end;
    int currX;
    int currY;
    unsigned int value;
    bool dead;
  };

  class Data {
    public:
      Data(int allocLength) {
        this->allocLength = allocLength;
        this->data = (int*) malloc(sizeof(int) * allocLength);
        clear();
      }
      ~Data() {
        free(data);
      }
      void clear() {
        dataLength = 0;
        xCurrent = INT_MIN;
        yCurrent = INT_MIN;
      }
      int allocLength;
      int dataLength;
      int* data;
      int xCurrent;
      int yCurrent;
  };

  void set(Data* data, int x, int y, byte value) {
    if (value) {
      //Serial.printf("set %d %d %d %d\n", x, y, value, dataLength);
      if (y == data->yCurrent) {
        if (x < data->xCurrent) {
          // todo: something
        } else if (x - data->xCurrent < pixelsPerData) {
          data->data[data->dataLength - 1] |= value << (bitsPerPixel * (x - data->xCurrent));
        } else {
          data->data[data->dataLength++] = x + offset;
          data->data[data->dataLength++] = value;
          data->xCurrent = x;
        }
      } else if (y > data->yCurrent) {
        data->data[data->dataLength++] = -(y + offset);
        data->data[data->dataLength++] = x + offset;
        data->data[data->dataLength++] = value;
        data->yCurrent = y;
        data->xCurrent = x;
      } else {
        // todo: something
      }
    }
  }

  static int min3(int x, int y, int z) {
    return min(min(x, y), z);
  }
  byte bitsPerPixel;
  byte pixelsPerData;
  int mask;
  Data* data1;
  Data* data2;
  Data* data;
  Data* next;
  TreeRule* treeRule;
  const static int offset = 100000;
};

class SimpleLife : public Life {
public:
  SimpleLife(int w, int h)
    : width(w), height(h), treeRule(new NiemiecTreeRule()) {
    data1 = (byte*)malloc(sizeof(byte) * w * h);
    data2 = (byte*)malloc(sizeof(byte) * w * h);
    data = data1;
    next = data2;
    clear();
  }
  ~SimpleLife() {
    free(data1);
    free(data2);
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
    iterateNeighborhood([this](int x, int y, int* neighbors) {
      //if (neighbors[0] + neighbors[1] + neighbors[2] + neighbors[3] + neighbors[4] + neighbors[5] + neighbors[6] + neighbors[7] + neighbors[8] > 0) {
      //  Serial.printf("x=%d y=%d, [%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", x, y, neighbors[0], neighbors[1], neighbors[2], neighbors[3], neighbors[4], neighbors[5], neighbors[6], neighbors[7], neighbors[8]);
      //}
      next[x + y * width] = treeRule->transition(neighbors);
    });
    byte* temp = data;
    data = next;
    next = temp;
  }
  void iterateNeighborhood(std::function<void(int x, int y, int* neighbors)> lambda) {
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int neighbors[] = {
          get(x - 1, y - 1), get(x, y - 1), get(x + 1, y - 1),
          get(x - 1, y), get(x + 1, y),
          get(x - 1, y + 1), get(x, y + 1), get(x + 1, y + 1),
          get(x, y)
        };
        lambda(x, y, neighbors);
      }
    }
  }
  void iterateLive(std::function<void(int x, int y, int value)> lambda) {
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        byte v = get(x, y);
        if (v != 0) {
          lambda(x, y, get(x, y));
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
  TreeRule* treeRule;
};
#endif