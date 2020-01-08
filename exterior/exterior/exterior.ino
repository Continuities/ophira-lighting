#include <Adafruit_NeoPixel.h>
#include <LayerEngine.h>

#define LED_PIN 6
#define FPS 30
#define WIDTH 6
#define HEIGHT 8
#define BRIGHTNESS 255

const Palette VEINS = {
  { 0, 0, 0 }, // background
  { 191, 0, 255 }, // foreground
  { 0, 0, 0 }, // accent
  { 86,100,39 }, // highlight
};

uint32_t lastFrame;
RGB** frameBuffer;
Adafruit_NeoPixel strip(WIDTH * HEIGHT, LED_PIN, NEO_BRG + NEO_KHZ800);

LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, VEINS);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, VEINS);
Layers::Spread spread = Layers::Spread(WIDTH, HEIGHT, 1, 1, 5, VEINS);
//Layers::Spread spread2 = Layers::Spread(WIDTH, HEIGHT, WIDTH - 1, HEIGHT - 1, VEINS);

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }
  
  lastFrame = millis();
  engine.push(&black);
  engine.push(&spread);
//  engine.push(&spread2);
  engine.push(&glitch);
}

void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  Serial.println(1000 / (millis() - lastFrame)); // Log FPS
  lastFrame = millis();
  engine.computeFrame(frameBuffer);
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      RGB colour = frameBuffer[x][y];
      strip.setPixelColor(getPixelIndex(x, y), colour.r, colour.g, colour.b);
    }
  }
  strip.show();
}

int getPixelIndex(int x, int y) {
  if (y % 2 == 0) {
    return (y * WIDTH) + x;
  }
  return ((y + 1) * WIDTH) - 1 - x;
}
