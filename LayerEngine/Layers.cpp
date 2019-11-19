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

Layers::Splotches::Splotches(int width, int height, Palette palette): VisualLayer(width, height, palette) {
  // TODO?
}
void Layers::Splotches::apply(HSV** frame) {
  int DESIRED_SPLOTCHES = 3;
  double SPLOTCH_VELOCITY = 0.02;
  int CORNER_FADE = 4;
  int numActive = 0;

  // Draw existing splotches
  for (int i = 0; i < sizeof splotches / sizeof splotches[0]; i++) {
    PointValue splotch = splotches[i];
    HSV bg = frame[splotch.x][splotch.y];
    if (splotch.v <= 0 || splotch.v > 1) {
      continue;
    }

    if (splotch.v * palette.foreground.v >= bg.v) {
      frame[splotch.x][splotch.y] = this->palette.foreground;
      frame[splotch.x][splotch.y].v *= splotch.v;
    }
    if (splotch.x > 0 && splotch.v * palette.accent.v >= bg.v) {
      frame[splotch.x - 1][splotch.y] = this->palette.accent;
      frame[splotch.x - 1][splotch.y].v *= splotch.v;
    }
    if (splotch.x < width - 1 && splotch.v * palette.accent.v >= bg.v) {
      frame[splotch.x + 1][splotch.y] = this->palette.accent;
      frame[splotch.x + 1][splotch.y].v *= splotch.v;
    }
    if (splotch.y > 0 && splotch.v * palette.accent.v >= bg.v) {
      frame[splotch.x][splotch.y - 1] = this->palette.accent;
      frame[splotch.x][splotch.y - 1].v *= splotch.v;
    }
    if (splotch.y < height - 1 && splotch.v * palette.accent.v >= bg.v) {
      frame[splotch.x][splotch.y + 1] = this->palette.accent;
      frame[splotch.x][splotch.y + 1].v *= splotch.v;
    }
    if (splotch.x > 0 && splotch.y > 0 && (splotch.v / CORNER_FADE) * palette.accent.v >= bg.v) {
      frame[splotch.x - 1][splotch.y - 1] = this->palette.accent;
      frame[splotch.x - 1][splotch.y - 1].v *= (splotch.v / CORNER_FADE);
    }
    if (splotch.x > 0 && splotch.y < height - 1 && (splotch.v / CORNER_FADE) * palette.accent.v >= bg.v) {
      frame[splotch.x - 1][splotch.y + 1] = this->palette.accent;
      frame[splotch.x - 1][splotch.y + 1].v *= (splotch.v / CORNER_FADE);
    }
    if (splotch.x < width - 1 && splotch.y > 0 && (splotch.v / CORNER_FADE) * palette.accent.v >= bg.v) {
      frame[splotch.x + 1][splotch.y - 1] = this->palette.accent;
      frame[splotch.x + 1][splotch.y - 1].v *= (splotch.v / CORNER_FADE);
    }
    if (splotch.x < width - 1 && splotch.y < height - 1 && (splotch.v / CORNER_FADE) * palette.accent.v >= bg.v) {
      frame[splotch.x + 1][splotch.y + 1] = this->palette.accent;
      frame[splotch.x + 1][splotch.y + 1].v *= (splotch.v / CORNER_FADE);
    }

    double newV = splotch.v + splotch.velocity;
    if (newV >= 1) {
      newV = 1;
      splotches[i].velocity *= -1;
    }
    splotches[i].v = newV;

    numActive++;
  }
  
  // Make new splotches, if necessary
  int i = 0;
  while (numActive < DESIRED_SPLOTCHES && i < sizeof splotches / sizeof splotches[0]) {
    if (splotches[i].v <= 0 || splotches[i].v > 1) {
      // This splotch is unused
      splotches[i] = {
        random(width),
        random(height),
        0.01,
        (double) random(50, 300) / 10000.0
      };
      numActive++;
    }
    i++;
  }
}