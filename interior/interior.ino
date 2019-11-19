#include <Adafruit_NeoPixel.h>
#include <LayerEngine.h>

#define LED_PIN 6
#define FPS 30
#define WIDTH 6
#define HEIGHT 8
#define BRIGHTNESS 85 // 255

const Palette XRAY = {
  { 39488, 255, 33 },
  { 38762, 88, 238 },
  { 32966, 69, 203 },
  { 54613, 10, 251 }
};

uint32_t lastFrame;
HSV** frameBuffer;

LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
Layers::Ether ether = Layers::Ether(WIDTH, HEIGHT, XRAY);
Layers::Splotches splotches = Layers::Splotches(WIDTH, HEIGHT, XRAY);

Adafruit_NeoPixel strip(WIDTH * HEIGHT, LED_PIN, NEO_BRG + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
//  while(!Serial) {}
  
  frameBuffer = new HSV*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new HSV[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }
  
  lastFrame = millis();
  engine.push(&ether);
  engine.push(&splotches);
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
      HSV colour = frameBuffer[x][y];
      uint32_t rgbColour = strip.ColorHSV(colour.h, colour.s, colour.v);
      strip.setPixelColor(getPixelIndex(x, y), rgbColour);
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
