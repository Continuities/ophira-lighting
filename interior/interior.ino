#include <Adafruit_NeoPixel.h>
#include <LayerEngine.h>

#define LED_PIN 6
#define FPS 30
#define WIDTH 6
#define HEIGHT 8
#define BRIGHTNESS 255 // 255
#define GLITCH_MIN_TIME 10000 // ms
#define GLITCH_MAX_TIME 60000 // ms
#define GLITCH_MIN_DURATION 500 // ms
#define GLITCH_MAX_DURATION 3000 // ms

const Palette XRAY = {
  { 8, 25, 53 }, // background
  { 156, 193, 238 }, // foreground
  { 182, 239, 194 },//{ 148, 202, 203 }, // accent
  { 251, 250, 251 } // highlight
};

uint32_t nextGlitch = 0;
uint32_t glitchOff = 0;
uint32_t lastFrame;
RGB** frameBuffer;

LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, XRAY);
Layers::Ether ether = Layers::Ether(WIDTH, HEIGHT, XRAY);
Layers::Dots dots = Layers::Dots(WIDTH, HEIGHT, XRAY);
Layers::Splotches splotches = Layers::Splotches(WIDTH, HEIGHT, XRAY);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, XRAY);

Adafruit_NeoPixel strip(WIDTH * HEIGHT, LED_PIN, NEO_BRG + NEO_KHZ800);

void setup() {
  randomSeed(analogRead(0));
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
//  while(!Serial) {}
  
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }
  
  lastFrame = millis();
//  engine.push(&black);
  engine.push(&ether);
  engine.push(&dots);
//  engine.push(&splotches);
//  engine.push(&glitch);
}

void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  Serial.println(1000 / (millis() - lastFrame)); // Log FPS
  lastFrame = millis();

  if (nextGlitch > 0 && nextGlitch <= lastFrame) {
    engine.push(&glitch);
    nextGlitch = 0;
    glitchOff = millis() + random(GLITCH_MIN_DURATION, GLITCH_MAX_DURATION);
  }
  else if (glitchOff > 0 && glitchOff <= lastFrame) {
    engine.pop();
    glitchOff = 0;
  }

  if (nextGlitch == 0 && glitchOff == 0) {
    nextGlitch = millis() + random(GLITCH_MIN_TIME, GLITCH_MAX_TIME);
  }
  
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
