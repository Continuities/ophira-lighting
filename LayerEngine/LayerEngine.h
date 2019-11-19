#ifndef LayerEngine_h
#define LayerEngine_h

#define MAX_SPLOTCHES 5

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

class VisualLayer {
  protected:
    int height;
    int width;
    Palette palette;
    
  public:
    VisualLayer(int width, int height, Palette palette);
    virtual void apply(RGB** frame);
    VisualLayer* next;
};

namespace Layers {

  class Black: public VisualLayer {
    public:
      Black(int width, int height, Palette palette);
      void apply(RGB** frame);
  };

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

  class Splotches: public VisualLayer {
    private:
      PointValue splotches[MAX_SPLOTCHES];
    public:
      Splotches(int width, int height, Palette palette);
      void apply(RGB** frame);
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
    void computeFrame(RGB** frame);
    void push(VisualLayer* layer);
    void pop();
};

#endif