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
    int start = indexWithoutDeadZones(range.start, range.row, width);
    int end = indexWithoutDeadZones(range.end, range.row, width);
    int firstIndex = end > start ? start : end + 1;
    int lastIndex = end > start ? end : start + 1;
    if (withoutDeadZones >= firstIndex) {
      if (withoutDeadZones < lastIndex) {
        // We're in the deadzone
        return -1;
      }
      deadZoneModifier += lastIndex - firstIndex;
    }
  }
  return withoutDeadZones - deadZoneModifier;
}

void LightMapper::addDeadZone(PixelRange range) {
  deadZones.push_back(range);
}