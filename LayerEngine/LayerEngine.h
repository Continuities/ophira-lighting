#ifndef LayerEngine_h
#define LayerEngine_h

#define MAX_SPLOTCHES 5

typedef struct {
  uint16_t h; // 65535
  uint8_t s; // 255
  uint8_t v; // 255
} HSV;

typedef struct {
  HSV background;
  HSV foreground;
  HSV accent;
  HSV highlight;
} Palette;

typedef struct {
  int x;
  int y;
  double v; // 1
  double velocity;
} PointValue;

class VisualLayer {
  protected:
    int height;
    int width;
    Palette palette;
    
  public:
    VisualLayer(int width, int height, Palette palette);
    virtual void apply(HSV** frame);
    VisualLayer* next;
};

namespace Layers {
  class Ether: public VisualLayer {
    private:
      double xSpeed;
      double ySpeed;
      double xVariance; // 1
      double yVariance; // 1
    public:
      Ether(int width, int height, Palette palette);
      void apply(HSV** frame);
  };

  class Splotches: public VisualLayer {
    private:
      PointValue splotches[MAX_SPLOTCHES];
    public:
      Splotches(int width, int height, Palette palette);
      void apply(HSV** frame);
  };
};

class LayerEngine {
  private:
    int width;
    int height;
    VisualLayer* first;
    VisualLayer* last();

  public:
    LayerEngine(int width, int height);
    void computeFrame(HSV** frame);
    void push(VisualLayer* layer);
    void pop();
};

#endif