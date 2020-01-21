#include "LightMapper.h"
#include <vector>

int indexWithoutDeadZones(int x, int y, int width) {
  if (y % 2 == 0) {
    return (y * width) + x;
  }
  return ((y + 1) * width) - 1 - x;
}

LightMapper::LightMapper(int w, int h): width(w), height(h), deadZones() {
  // Empty for now
}

int LightMapper::getPixelIndex(int x, int y) {
  int withoutDeadZones = indexWithoutDeadZones(x, y, width);
  int deadZoneModifier = 0;
  for (PixelRange range : deadZones) {
    int start = indexWithoutDeadZones(range.row, range.start, width);
    if (withoutDeadZones >= start) {
      int end = indexWithoutDeadZones(range.row, range.end, width);
      if (withoutDeadZones < end) {
        // We're in the deadzone
        return -1;
      }
      deadZoneModifier += end - start;
    }
  }
  return withoutDeadZones - deadZoneModifier;
}

LightMapper LightMapper::addDeadZone(PixelRange range) {
  deadZones.push_back(range);
}