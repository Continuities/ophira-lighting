#include "Arduino.h"
#include "LayerEngine.h"

Layers::Ether::Ether(int width, int height, Palette palette) : VisualLayer(width, height, palette) {}
void Layers::Ether::apply(HSV** frame) {
  int period = this->width - 1;
  int shift = 0;
  uint8_t variance = this->palette.background.v / 3;
  for (int x = 0; x < this->width; x++) {
    double hueShift = sin((2 * PI * x) / period);
    for (int y = 0; y < this->height; y++) {
      frame[x][y] = this->palette.background;
      frame[x][y].v += (uint8_t) (hueShift * variance);
    }
  }
}