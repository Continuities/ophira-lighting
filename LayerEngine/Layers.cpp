#include "Arduino.h"
#include "LayerEngine.h"

Layers::Ether::Ether(int width, int height, Palette palette) : VisualLayer(width, height, palette) {
  xVariance = rand() / RAND_MAX;
  yVariance = rand() / RAND_MAX;
  xSpeed = 0.02;
  ySpeed = 0.03;
}
void Layers::Ether::apply(HSV** frame) {
  int period = this->width - 1;
  int shift = 0;

  xVariance = xVariance + xSpeed;
  if (xVariance >= 1) {
    xSpeed *= -1;
    xVariance = 1;
  }
  else if (xVariance <= 0) {
    xSpeed *= -1;
    xVariance = 0;
  }
  
  yVariance = yVariance + ySpeed;
  if (yVariance >= 1) {
    ySpeed *= -1;
    yVariance = 1;
  }
  else if (yVariance <= 0) {
    ySpeed *= -1;
    yVariance = 0;
  }
  uint8_t variance = this->palette.background.v / 3;
  for (int x = 0; x < this->width; x++) {
    double xShift = sin((2 * PI * x) / (this->width - 1));
    for (int y = 0; y < this->height; y++) {
      double yShift = sin((2 * PI * y) / (this->height - 1));
      frame[x][y] = this->palette.background;
      frame[x][y].v += (uint8_t) (xShift * variance * xVariance);
      frame[x][y].v += (uint8_t) (yShift * variance * yVariance);
    }
  }
}

Layers::Splotches::Splotches(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::Splotches::apply(HSV** frame) {
  // TODO
}