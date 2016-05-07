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
  const int THREADHOLD = 20, S_THREADHOLD = 5;
}

unsigned char getMaskColor(CImg<float> &p, int x, int y) {
  unsigned sum = 0;
  for (int k = 0; k < p.spectrum(); k++)
    sum += p(x, y, k);
  // if the pixel appears white, then the mask is BLACK
  if (sum > THREADHOLD) return BLACK;
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
  if (v[LEN / 3 * 2] < S_THREADHOLD) return WHITE;
  else if (v[LEN / 3 * 2] < THREADHOLD) return (WHITE + BLACK) / 2;
  else return BLACK;
}

unsigned char getMaskColor3(CImg<float> &p, int x, int y) {
  int sum = 0;
  for (int i = 0; i < LEN; i++)
    for (int k = 0; k < p.spectrum(); k++)
      sum += p(x + xp[i], y + y[p], k);
  sum /= (LEN * p.spectrum());
  // if the pixel appears black, then the mask is WHITE
  if (sum < S_THREADHOLD) return WHITE;
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

void getIntensity(CImg<float> &image) {
  CImg<unsigned char> visu(500,400,1,3,0);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  CImgDisplay main_disp(image,"Click a point"), draw_disp(visu,"Intensity profile");
  while (!main_disp.is_closed() && !draw_disp.is_closed()) {
    main_disp.wait();
    if (main_disp.button() && main_disp.mouse_y()>=0) {
      const int y = main_disp.mouse_y();
      visu.fill(0).draw_graph(image.get_crop(0,y,0,0,image.width()-1,y,0,0),red,1,1,0,255,0);
      visu.draw_graph(image.get_crop(0,y,0,1,image.width()-1,y,0,1),green,1,1,0,255,0);
      visu.draw_graph(image.get_crop(0,y,0,2,image.width()-1,y,0,2),blue,1,1,0,255,0).display(draw_disp);
      }
  }
}

int main() {
  for (int i = 0; i < MAX; i++) {
    char filename[100];
    sprintf(filename, "bl%d.jpg", i);
    CImg<float> src(filename);
    CImg<float> blurImg = (src - 100).cut(BLACK, WHITE).blur(2);
    CImg<float> mask = getMask(blurImg, getMaskColor),
       mask2 = getMask(blurImg, getMaskColor2),
       mask3 = getMask(blurImg, getMaskColor3);
    getIntensity(src);
    (blurImg,
       mask.operator+(src).cut(BLACK, WHITE), 
       mask2.operator+(src).cut(BLACK, WHITE),
       mask3.operator+(src).cut(BLACK, WHITE)).display();
  }
  return 0;
}