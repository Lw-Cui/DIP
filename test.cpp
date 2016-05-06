#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include "CImg/CImg.h"
using namespace std;
using namespace cimg_library;
namespace {
  const int WHITE = 255, BLACK = 0;
  const int MAX = 3;
  const int LEN = 9;
  const int xp[LEN] = { 1, 1, 1, 0, 0, 0, -1, -1, -1};
  const int yp[LEN] = { 1, -1, 0, 1, -1, 0, 1, -1, 0};
}

unsigned char getMaskColor(CImg<float> &p, int x, int y) {
  unsigned sum = 0;
  for (int k = 0; k < p.spectrum(); k++)
    sum += p(x, y, k);
  // if the pixel appears white, then the mask is BLACK
  if (sum > 20) return BLACK;
  else return WHITE;
}

unsigned char getMaskColor2(CImg<float> &p, int x, int y) {
  vector<int> v;
  for (int i = 0; i < LEN; i++) {
    int sum = 0;
    for (int k = 0; k < p.spectrum(); k++)
      sum += p(x + xp[i], y + y[p], k);
    v.push_back(sum);
  }
  sort(v.begin(), v.end());
  // if the pixel appears black, then the mask is WHITE
  if (v[LEN / 3 * 2] < 5) return WHITE;
  else if (v[LEN / 3 * 2] < 20) return (WHITE + BLACK) / 2;
  else return BLACK;
}

CImg<float> getMask(CImg<float> &p, 
  function<unsigned char(CImg<float> &, int, int)> c) {
  CImg<float> mask(p.width(), p.height(), 1, 1);
  for (int i = 1; i < p.width() - 1; i++)
    for (int j = 1; j < p.height() - 1; j++)
      mask(i, j) = c(p, i, j);
  return mask;
}

int main() {
  for (int i = 0; i < MAX; i++) {
    char filename[100];
    sprintf(filename, "bl%d.jpg", i);
    CImg<float> src(filename);
    CImg<float> tmp = (src - 100).cut(BLACK, WHITE).blur(2);
    CImg<float> mask = getMask(tmp, getMaskColor),
       mask2 = getMask(tmp, getMaskColor2);
    (src,
       mask.operator+(src).cut(BLACK, WHITE), 
       mask2.operator+(src).cut(BLACK, WHITE)).display();
  }
  return 0;
}