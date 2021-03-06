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

typedef struct {
  int start; // inclusive
  int number;
} LogicalPadding;

class LightMapper {
  private:
    int width;
    int height;
    std::vector<PixelRange> deadZones; 
    std::vector<LogicalPadding> padding;
  public:
    LightMapper(int width, int height);

    // Returns -1 if the pixel is in a deadzone
    int getPixelIndex(int x, int y);

    // Adds a deadzone to the grid
    void addDeadZone(PixelRange range);

    // Adds logical padding to the strip
    void addPadding(int start, int number);
};

#endif 