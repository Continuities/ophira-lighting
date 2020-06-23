#ifndef LayerEngine_h
#define LayerEngine_h

#include <stdint.h>

#define MAX_STUFF 5

typedef struct {
  uint8_t r; // 255
  uint8_t g; // 255
  uint8_t b; // 255
} RGB;

typedef struct {
  RGB background;
  RGB foreground;
  RGB accent;
  RGB highlight;
} Palette;

typedef struct {
  int x;
  int y;
  double v; // 1
  double velocity;
} PointValue;

// Base class for Layers. You'll want to extend this for your custom effects.
class VisualLayer {
  protected:
    int height; // height in pixels
    int width; // width in pixels
    Palette palette; // colour palette
    
  public:
    VisualLayer(int width, int height, Palette palette);
    virtual void apply(RGB** frame); // called each frame to mutate the framebuffer
    VisualLayer* next;
};

// Add definitions for new Layer types in here
namespace Layers {

  // Just sets everything to black
  class Black: public VisualLayer {
    public:
      Black(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Static background
  class Background: public VisualLayer {
    public:
      Background(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Warbly background
  class Ether: public VisualLayer {
    private:
      double xSpeed;
      double ySpeed;
      double xVariance; // 1
      double yVariance; // 1
    public:
      Ether(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Little flashy dots
  class Dots: public VisualLayer {
    private:
      PointValue dots[MAX_STUFF];
    public:
      Dots(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Big flashy splotches
  class Splotches: public VisualLayer {
    private:
      PointValue splotches[MAX_STUFF];
    public:
      Splotches(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Erratic accent lines that zap around
  class Glitch: public VisualLayer {
    private:
      PointValue line;
      bool visible;
    public:
      Glitch(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Pulsing colour that spreads from a central point
  class Spread: public VisualLayer {
    private:
      int cX;
      int cY;
      int clip;
      float shift;
    public:
      Spread(int width, int height, int centerX, int centerY, int clipDistance, Palette palette);
      void apply(RGB** frame); 
  };

  // Vertical test pattern
  class VerticalStripes: public VisualLayer {
    public:
      VerticalStripes(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Horizontal test pattern
  class HorizontalStripes: public VisualLayer {
    public:
      HorizontalStripes(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

  // Strand number pattern to help put leds in order
  class StrandIdent: public VisualLayer {
    public:
      StrandIdent(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

};

// Composes layers and calculates new frames of colour
class LayerEngine {
  private:
    int width;
    int height;
    VisualLayer* first;
    VisualLayer* last();

  public:
    LayerEngine(int width, int height);
    void computeFrame(RGB** frame); // mutates the framebuffer by composing all the defined layers 
    void push(VisualLayer* layer); // pushes a new layer into the composition stack
    void pop(); // pops the top layer off the composition stack
};

#endif
