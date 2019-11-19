#ifndef LayerEngine_h
#define LayerEngine_h

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

class VisualLayer {
  protected:
    
    int height;
    Palette palette;
    
  public:
    int width;
    VisualLayer(int width, int height, Palette palette);
    virtual void apply(HSV** frame);
    VisualLayer* next;
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

namespace Layers {
  class Ether: public VisualLayer {
    public:
      Ether(int width, int height, Palette palette);
      void apply(HSV** frame);
  };
};

#endif