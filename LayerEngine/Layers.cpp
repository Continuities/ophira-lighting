#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "LayerEngine.h"

#define PI 3.1415926535897932384626433832795

extern long rnd(long howbig) {
  if ( howbig == 0 ) {
    return 0 ;
  }
  return rand() % howbig;
}

extern long rnd(long howsmall, long howbig) {
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return rnd(diff) + howsmall;
}

uint8_t clampToByte(int in) {
  if (in > 255) {
    return 255;
  }
  if (in < 0) {
    return 0;
  }
  return in;
}

float wrapPercent(float in) {
  while (in < 0) {
    in += 1;
  }
  if (in > 1) {
    in = fmod(in, 1);
  }
  return in;
}

RGB blend(RGB first, RGB second, double amount) {
  return {
    clampToByte(first.r * (1.0 - amount) + (second.r * amount)),
    clampToByte(first.g * (1.0 - amount) + (second.g * amount)),
    clampToByte(first.b * (1.0 - amount) + (second.b * amount))
  };
}

uint8_t variate(uint8_t value, double amount) {
  int delta = value * amount;
  if (value + delta > 255) {
    return 255;
  }
  if (value + delta < 0) {
    return 0;
  }
  return value + delta;
}

Layers::Black::Black(int width, int height, Palette palette) : VisualLayer(width, height, palette) {}
void Layers::Black::apply(RGB** frame) {
  for (int x = 0; x < this->width; x++) {
    for (int y = 0; y < this->height; y++) {
      frame[x][y] = { 0, 0, 0 };
    }
  }
}

Layers::Ether::Ether(int width, int height, Palette palette) : VisualLayer(width, height, palette) {
  xVariance = rand() / RAND_MAX;
  yVariance = rand() / RAND_MAX;
  xSpeed = 0.02;
  ySpeed = 0.03;
}
void Layers::Ether::apply(RGB** frame) {
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
  double variance = 0.3;
  for (int x = 0; x < this->width; x++) {
    double xShift = sin((2 * PI * x) / (this->width - 1));
    for (int y = 0; y < this->height; y++) {
      double yShift = sin((2 * PI * y) / (this->height - 1));
      double pixelVariance = (xShift * variance * xVariance) + (yShift * variance * yVariance);
      frame[x][y] = {
        variate(palette.background.r, pixelVariance),
        variate(palette.background.g, pixelVariance),
        variate(palette.background.b, pixelVariance)
      };
    }
  }
}

Layers::Dots::Dots(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::Dots::apply(RGB** frame) {
  int DESIRED_DOTS = 3;
  double DOT_VELOCITY = 0.02;
  int numActive = 0;

  // Draw existing dots
  for (int i = 0; i < sizeof dots / sizeof dots[0]; i++) {
    PointValue dot = dots[i];
    if (dot.v <= 0 || dot.v > 1) {
      continue;
    }

    frame[dot.x][dot.y] = blend(frame[dot.x][dot.y], palette.foreground, dot.v);

    double newV = dot.v + dot.velocity;
    if (newV >= 1) {
      newV = 1;
      dots[i].velocity *= -1;
    }
    dots[i].v = newV;

    numActive++;
  }
  
  // Make new dots, if necessary
  int i = 0;
  while (numActive < DESIRED_DOTS && i < sizeof dots / sizeof dots[0]) {
    if (dots[i].v <= 0 || dots[i].v > 1) {
      // This dot is unused
      dots[i] = {
        rnd(width),
        rnd(height),
        0.01,
        (double) rnd(50, 300) / 10000.0
      };
      numActive++;
    }
    i++;
  }
}

Layers::Splotches::Splotches(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::Splotches::apply(RGB** frame) {
  int DESIRED_SPLOTCHES = 3;
  double SPLOTCH_VELOCITY = 0.02;
  int CORNER_FADE = 4;
  int numActive = 0;

  // Draw existing splotches
  for (int i = 0; i < sizeof splotches / sizeof splotches[0]; i++) {
    PointValue splotch = splotches[i];
    if (splotch.v <= 0 || splotch.v > 1) {
      continue;
    }

    frame[splotch.x][splotch.y] = blend(frame[splotch.x][splotch.y], palette.foreground, splotch.v);
    if (splotch.x > 0 && splotch.v) {
      frame[splotch.x - 1][splotch.y] = blend(frame[splotch.x - 1][splotch.y], palette.accent, splotch.v);
    }
    if (splotch.x < width - 1) {
      frame[splotch.x + 1][splotch.y] = blend(frame[splotch.x + 1][splotch.y], palette.accent, splotch.v);
    }
    if (splotch.y > 0) {
      frame[splotch.x][splotch.y - 1] = blend(frame[splotch.x][splotch.y - 1], palette.accent, splotch.v);
    }
    if (splotch.y < height - 1) {
      frame[splotch.x][splotch.y + 1] = blend(frame[splotch.x][splotch.y + 1], palette.accent, splotch.v);
    }
    if (splotch.x > 0 && splotch.y > 0) {
      frame[splotch.x - 1][splotch.y - 1] = blend(frame[splotch.x - 1][splotch.y - 1], palette.accent, splotch.v / CORNER_FADE);
    }
    if (splotch.x > 0 && splotch.y < height - 1) {
      frame[splotch.x - 1][splotch.y + 1] = blend(frame[splotch.x - 1][splotch.y + 1], palette.accent, splotch.v / CORNER_FADE);
    }
    if (splotch.x < width - 1 && splotch.y > 0) {
      frame[splotch.x + 1][splotch.y - 1] = blend(frame[splotch.x + 1][splotch.y - 1], palette.accent, splotch.v / CORNER_FADE);
    }
    if (splotch.x < width - 1 && splotch.y < height - 1) {
      frame[splotch.x + 1][splotch.y + 1] = blend(frame[splotch.x + 1][splotch.y + 1], palette.accent, splotch.v / CORNER_FADE);
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
        rnd(width),
        rnd(height),
        0.01,
        (double) rnd(50, 300) / 10000.0
      };
      numActive++;
    }
    i++;
  }
}

Layers::Glitch::Glitch(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::Glitch::apply(RGB** frame) {
  int GLITCH_CHANCE = 10;
  if (!visible) {
    if (rnd(0, 100) <= GLITCH_CHANCE) {
      // Generate a new glitch line
      line = {
        rnd(0, width),
        rnd(0, height),
        (double) rnd(0, 101) / 100.0,
        0
      };
      visible = true;
    }
    return;
  }
  else if (rnd(0, 100) <= GLITCH_CHANCE) {
    visible = false;
    return;
  }

  double x = line.x;
  double y = line.y;
  double deltaX = line.v;
  double deltaY = 1 - line.v;
  while (x < width && y < height) {
    frame[(int)x][(int)y] = palette.highlight;
    if (deltaX > deltaY) {
      x += 1;
      y += (1 / deltaX) * deltaY;
    } else {
      x += (1 / deltaY) * deltaX;
      y += 1;
    }
  }
  x = line.x;
  y = line.y;
  while (x >= 0 && y >= 0) {
    frame[(int)x][(int)y] = palette.highlight;
    if (deltaX > deltaY) {
      x -= 1;
      y -= (1 / deltaX) * deltaY;
    } else {
      x -= (1 / deltaY) * deltaX;
      y -= 1;
    }
  }
}

Layers::Spread::Spread(int width, int height, int centerX, int centerY, int clipDistance, Palette palette): VisualLayer(width, height, palette) {
  shift = 0;
  cX = centerX;
  cY = centerY;
  clip = clipDistance;
}
void Layers::Spread::apply(RGB** frame) {
  shift = wrapPercent(shift + 0.03);
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      float deltaX = x - cX;
      float deltaY = y - cY;
      float dist = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
      if (dist < clip) {
        float offset = wrapPercent(1 - (dist / clip) + shift);
        frame[x][y] = blend(frame[x][y], palette.foreground, offset);
      }
    }
  }
}

Layers::VerticalStripes::VerticalStripes(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::VerticalStripes::apply(RGB** frame) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      switch (x % 3) {
        case 0:
          frame[x][y] = palette.background;
          break;
        case 1:
          frame[x][y] = palette.foreground;
          break;
        case 2:
          frame[x][y] = palette.accent;
          break;
      }
    }
  }
}

Layers::HorizontalStripes::HorizontalStripes(int width, int height, Palette palette): VisualLayer(width, height, palette) {}
void Layers::HorizontalStripes::apply(RGB** frame) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      switch (y % 3) {
        case 0:
          frame[x][y] = palette.background;
          break;
        case 1:
          frame[x][y] = palette.foreground;
          break;
        case 2:
          frame[x][y] = palette.accent;
          break;
      }
    }
  }
}