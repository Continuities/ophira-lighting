#include <Adafruit_NeoPixel.h>
#include <LayerEngine.h>
#include <LightMapper.h>

#define FPS 30
#define LED_PIN     6
#define LED_COUNT   176
#define BRIGHTNESS  50
#define HEIGHT      25
#define WIDTH       11
#define LED_COUNT   HEIGHT * WIDTH
#define LOG_FPS     false

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

Layers::VerticalStripes testPattern = Layers::VerticalStripes(WIDTH, HEIGHT, TEST_STRIPES);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, VEINS);
Layers::Spread spread = Layers::Spread(WIDTH, HEIGHT, 0, 8, 17, VEINS);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_BRG + NEO_KHZ800);

uint32_t lastFrame;
RGB** frameBuffer;
LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
LightMapper lightMapper = LightMapper(WIDTH, HEIGHT);

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

void setup() {

  Serial.begin(9600);
  
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  addPanelParameters();
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }

  engine.push(&black);
  engine.push(&spread);
  lastFrame = millis();
}


void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  if (LOG_FPS) {
    Serial.println(1000 / (millis() - lastFrame)); // Log FPS
  }
  lastFrame = millis();
  
  engine.computeFrame(frameBuffer);
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      int index = lightMapper.getPixelIndex(x, y);
      if (index >= 0) {
        RGB colour = frameBuffer[x][y];
        strip.setPixelColor(index, strip.gamma32(strip.Color(colour.r, colour.g, colour.b)));
      }
    }
  }
  
  strip.show();
}
