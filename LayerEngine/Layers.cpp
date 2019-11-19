#include "Arduino.h"
#include "LayerEngine.h"

uint8_t clampToByte(int in) {
  if (in > 255) {
    return 255;
  }
  if (in < 0) {
    return 0;
  }
  return in;
}

RGB blend(RGB first, RGB second, double amount) {
  return {
    clampToByte(first.r + (second.r * amount)),
    clampToByte(first.g + (second.g * amount)),
    clampToByte(first.b + (second.b * amount))
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
      Serial.println(pixelVariance * palette.background.r);
      frame[x][y] = {
        variate(palette.background.r, pixelVariance),
        variate(palette.background.g, pixelVariance),
        variate(palette.background.b, pixelVariance)
      };
    }
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