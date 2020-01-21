#include <stdio.h>
#include <emscripten/emscripten.h>
#include "../LayerEngine/LayerEngine.h"
#include "../LightMapper/LightMapper.h"


/* =================================================
 * Set up your scene below in the same way you would
 * in an Adruino sketch.
 * ================================================= */

#define HEIGHT 8 // Height of the pixel grid
#define WIDTH 12 // Width of the pixel grid
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

RGB strip[STRIP_LENGTH];
RGB** frameBuffer;
LayerEngine engine = LayerEngine(WIDTH, HEIGHT);

// Initialized the layers to use in the scene
Layers::VerticalStripes testPattern = Layers::VerticalStripes(WIDTH, HEIGHT, TEST_STRIPES);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, VEINS);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, VEINS);
Layers::Spread spread = Layers::Spread(WIDTH, HEIGHT, WIDTH / 2, HEIGHT / 2, 8, VEINS);

LightMapper lightMapper = LightMapper(WIDTH, HEIGHT);

/**
 * Analogous to Arduino's setup() function
 * Configure the layer engine here.
 */
int main(int argc, char ** argv) {

  // initialize the virtual pixel strip
  for (int i = 0; i < STRIP_LENGTH; i++) {
    strip[i] = { 0, 0, 0 };
  }

  // Add deadzones
  // This should mirror the "physical" strip construction
  // in index.html
  lightMapper.addDeadZone({ 0, 1, 3 });
  lightMapper.addDeadZone({ 0, 7, 9 });
  lightMapper.addDeadZone({ 1, 1, 3 });
  lightMapper.addDeadZone({ 1, 7, 9 });
  lightMapper.addDeadZone({ 2, 1, 3 });
  lightMapper.addDeadZone({ 2, 7, 9 });
  lightMapper.addDeadZone({ 3, 1, 3 });
  lightMapper.addDeadZone({ 3, 7, 9 });

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