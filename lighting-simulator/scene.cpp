#include <stdio.h>
#include <emscripten/emscripten.h>
#include "../LayerEngine/LayerEngine.h"
#include "../LightMapper/LightMapper.h"


/* =================================================
 * Set up your scene below in the same way you would
 * in an Adruino sketch.
 * ================================================= */

#define HEIGHT 25 // Height of the pixel grid
#define WIDTH 11 // Width of the pixel grid
#define STRIP_LENGTH HEIGHT * WIDTH // number of pixels in the strip

// Current Palette
const Palette VEINS = {
  { 0, 0, 0 }, // background
  { 191, 0, 255 }, // foreground
  { 0, 0, 0 }, // accent
  { 86,100,39 }, // highlight
};
const Palette TEST_STRIPES = {
  { 255, 0, 0 },
  { 0, 255, 0 },
  { 0, 0, 255 },
  { 0, 0, 0}
};
const Palette XRAY = {
  { 8, 25, 53 }, // background
  { 156, 193, 238 }, // foreground
  { 182, 239, 194 },//{ 148, 202, 203 }, // accent
  { 251, 250, 251 } // highlight
};

RGB strip[STRIP_LENGTH];
RGB** frameBuffer;
LayerEngine engine = LayerEngine(WIDTH, HEIGHT);

// Initialized the layers to use in the scene
Layers::VerticalStripes testPattern = Layers::VerticalStripes(WIDTH, HEIGHT, TEST_STRIPES);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, VEINS);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, XRAY);
Layers::Spread spread = Layers::Spread(WIDTH, HEIGHT, 0, 8, 17, VEINS);
Layers::Ether ether = Layers::Ether(WIDTH, HEIGHT, XRAY);
Layers::Dots dots = Layers::Dots(WIDTH, HEIGHT, XRAY);
Layers::Splotches splotches = Layers::Splotches(WIDTH, HEIGHT, XRAY);

LightMapper lightMapper = LightMapper(WIDTH, HEIGHT);

// Wall-specific layout definitions
void addSpleenParameters() {
  lightMapper.addDeadZone({ 8, 23, 38 });
  lightMapper.addDeadZone({ 9, 23, 38 });
  lightMapper.addDeadZone({ 10, 22, 38 });
  lightMapper.addDeadZone({ 11, 22, 38 });
  lightMapper.addDeadZone({ 12, 22, 38 });
  lightMapper.addDeadZone({ 13, 22, 38 });
  lightMapper.addDeadZone({ 14, 21, 38 });
  lightMapper.addDeadZone({ 15, 21, 38 });
  lightMapper.addDeadZone({ 16, 12, 14 });
  lightMapper.addDeadZone({ 16, 21, 38 });
  lightMapper.addDeadZone({ 17, 11, 15 });
  lightMapper.addDeadZone({ 17, 21, 38 });
  lightMapper.addDeadZone({ 18, 11, 15 });
  lightMapper.addDeadZone({ 18, 21, 38 });
  lightMapper.addDeadZone({ 19, 11, 15 });
  lightMapper.addDeadZone({ 19, 20, 38 });
  lightMapper.addDeadZone({ 20, 11, 15 });
  lightMapper.addDeadZone({ 20, 20, 38 });
  lightMapper.addDeadZone({ 21, 10, 15 });
  lightMapper.addDeadZone({ 21, 20, 38 });
  lightMapper.addDeadZone({ 22, 10, 15 });
  lightMapper.addDeadZone({ 22, 19, 38 });
  lightMapper.addDeadZone({ 23, 10, 38 });
  lightMapper.addDeadZone({ 24, 10, 38 });
  lightMapper.addDeadZone({ 25, 9, 38 });


  // Pad each of the eight lines to 90 pixels
  // First four lines are 76 pixels long
  lightMapper.addPadding(76, 14);
  lightMapper.addPadding(166, 14);
  lightMapper.addPadding(256, 14);
  lightMapper.addPadding(346, 14);
  // Sixth is 86
  lightMapper.addPadding(536, 4);
  // Seventh is 69
  lightMapper.addPadding(609, 21);
}

void addPanelParameters() {
  lightMapper.addDeadZone({ 0, 0, 1 });
  lightMapper.addDeadZone({ 0, 3, 11 });
  lightMapper.addDeadZone({ 1, 0, 1 });
  lightMapper.addDeadZone({ 1, 3, 11 });
  lightMapper.addDeadZone({ 2, 0, 1 });
  lightMapper.addDeadZone({ 2, 8, 11 });
  lightMapper.addDeadZone({ 3, 0, 1 });
  lightMapper.addDeadZone({ 3, 8, 11 });
  lightMapper.addDeadZone({ 4, 0, 1 });
  lightMapper.addDeadZone({ 4, 7, 11 });
  lightMapper.addDeadZone({ 5, 7, 11 });
  lightMapper.addDeadZone({ 6, 7, 11 });
  lightMapper.addDeadZone({ 7, 8, 11 });
  lightMapper.addDeadZone({ 8, 8, 11 });
  lightMapper.addDeadZone({ 9, 8, 11 });
  lightMapper.addDeadZone({ 10, 9, 11 });
  lightMapper.addDeadZone({ 12, 0, 2 });
  lightMapper.addDeadZone({ 13, 0, 2 });
  lightMapper.addDeadZone({ 14, 0, 1 });
  lightMapper.addDeadZone({ 15, 0, 1 });
  lightMapper.addDeadZone({ 16, 0, 1 });
  lightMapper.addDeadZone({ 17, 0, 1 });
  lightMapper.addDeadZone({ 18, 0, 1 });
  lightMapper.addDeadZone({ 18, 8, 11 });
  lightMapper.addDeadZone({ 19, 0, 1 });
  lightMapper.addDeadZone({ 19, 8, 11 });
  lightMapper.addDeadZone({ 20, 0, 1 });
  lightMapper.addDeadZone({ 20, 8, 11 });
  lightMapper.addDeadZone({ 21, 0, 2 });
  lightMapper.addDeadZone({ 21, 8, 11 });
  lightMapper.addDeadZone({ 22, 0, 3 });
  lightMapper.addDeadZone({ 22, 8, 11 });
  lightMapper.addDeadZone({ 23, 0, 6 });
  lightMapper.addDeadZone({ 23, 8, 11 });
  lightMapper.addDeadZone({ 24, 0, 6 });
  lightMapper.addDeadZone({ 24, 8, 11 });
}

/**
 * Analogous to Arduino's setup() function
 * Configure the layer engine here.
 */
int main(int argc, char ** argv) {

  // initialize the virtual pixel strip
  for (int i = 0; i < STRIP_LENGTH; i++) {
    strip[i] = { 0, 0, 0 };
  }

  // Add deadzones and logical padding
  // This should mirror the "physical" strip construction
  // in index.html
  // addSpleenParameters();
  addPanelParameters();

  // initialize the frame buffer
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }

  // Push a few layers into the composition stack
  // engine.push(&testPattern);
  engine.push(&black);
  engine.push(&spread);
  // engine.push(&ether);
  // engine.push(&dots);
  // engine.push(&splotches);
  // engine.push(&glitch);
}

/* =================================================
 * The stuff below allows for communication between
 * the web browser and the compiled WASM code.
 * You shouldn't have to touch it.
 * ================================================= */

#ifdef __cplusplus
extern "C" {
#endif

void EMSCRIPTEN_KEEPALIVE computeFrame() {
  engine.computeFrame(frameBuffer);
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      int index = lightMapper.getPixelIndex(x, y);
      if (index >= 0) {
        strip[index] = frameBuffer[x][y];
      }
    }
  }
}

uint32_t EMSCRIPTEN_KEEPALIVE getPixel(int index) {
  RGB rgb = strip[index];
  return ((rgb.r & 0xff) << 16) + ((rgb.g & 0xff) << 8) + (rgb.b & 0xff);
}

int EMSCRIPTEN_KEEPALIVE getWidth() {
  return WIDTH;
}

int EMSCRIPTEN_KEEPALIVE getHeight() {
  return HEIGHT;
}

#ifdef __cplusplus
}
#endif
