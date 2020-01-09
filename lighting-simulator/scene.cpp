#include <stdio.h>
#include <emscripten/emscripten.h>
#include "../LayerEngine/LayerEngine.h"


/* =================================================
 * Set up your scene below in the same way you would
 * in an Adruino sketch.
 * ================================================= */

#define HEIGHT 8 // Height of the pixel grid
#define WIDTH 6 // Width of the pixel grid

// Current Palette
const Palette VEINS = {
  { 0, 0, 0 }, // background
  { 191, 0, 255 }, // foreground
  { 0, 0, 0 }, // accent
  { 86,100,39 }, // highlight
};

RGB** frameBuffer;
LayerEngine engine = LayerEngine(WIDTH, HEIGHT);

// Initialized the layers to use in the scene
Layers::Black black = Layers::Black(WIDTH, HEIGHT, VEINS);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, VEINS);
Layers::Spread spread = Layers::Spread(WIDTH, HEIGHT, 1, 1, 5, VEINS);

/**
 * Analogous to Arduino's setup() function
 * Configure the layer engine here.
 */
int main(int argc, char ** argv) {

  // initialize a blank frame buffer
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }

  // Push a few layers into the composition stack
  engine.push(&black);
  engine.push(&spread);
  engine.push(&glitch);
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
}

uint32_t EMSCRIPTEN_KEEPALIVE getPixel(int x, int y) {
  RGB rgb = frameBuffer[x][y];
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