#ifndef LightMapper_h
#define LightMapper_h

/**
 * Assumes that strips are wired horizontally, 
 * and are of equal length
 **/

#include <vector>

typedef struct {
  int row;
  int start; // inclusive
  int end; // non-inclusive
} PixelRange;

class LightMapper {
  private:
    int width;
    int height;
    std::vector<PixelRange> deadZones; 
  public:
    LightMapper(int width, int height);

    // Returns -1 if the pixel is in a deadzone
    int getPixelIndex(int x, int y);

    // Adds a deadzone to the grid
    LightMapper addDeadZone(PixelRange range);
};

#endif 