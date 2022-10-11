#include "LightMapper.h"
#include <vector>

bool isRowReversed(int rowIndex, const int* strips) {
  if (strips == NULL) {
    return rowIndex % 2 == 0;
  }
  int r = rowIndex;
  for (int i = 0; i < 8; i++) {
    if (r < strips[i]) {
      break;
    }
    r -= strips[i];
  }
  return r % 2 > 0;
}

int indexWithoutDeadZones(int x, int y, int width, const int* strips) {
  if (!isRowReversed(y, strips)) {
    return (y * width) + x;
  }
  return ((y + 1) * width) - 1 - x;
}

LightMapper::LightMapper(int w, int h): width(w), height(h), deadZones(), stripDefinition(NULL) {
  // Empty for now
}

LightMapper::LightMapper(int w, int h, const int* def): width(w), height(h), deadZones(), stripDefinition(def) {
  // Empty
}

int LightMapper::getPixelIndex(int x, int y) {
  int withoutDeadZones = indexWithoutDeadZones(x, y, width, stripDefinition);
  int deadZoneModifier = 0;
  for (PixelRange range : deadZones) {
    int start = indexWithoutDeadZones(range.start, range.row, width, stripDefinition);
    int end = indexWithoutDeadZones(range.end, range.row, width, stripDefinition);
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
  int finalIndex = withoutDeadZones - deadZoneModifier;

  int padIndex = 0;
  while (padIndex < padding.size() && finalIndex >= padding[padIndex].start) {
    finalIndex += padding[padIndex].number;
    padIndex++;
  }

  return finalIndex;
}

void LightMapper::addDeadZone(PixelRange range) {
  deadZones.push_back(range);
}

void LightMapper::addPadding(int start, int num) {
  padding.push_back({ start, num });
}