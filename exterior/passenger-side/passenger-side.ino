#include <Adafruit_NeoPixel.h>
#include <LayerEngine.h>
#include <LightMapper.h>

#define PANEL1_PIN 6
#define PANEL2_PIN 12
#define FPS 30

#define PANEL1_WIDTH 11
#define PANEL1_HEIGHT 25
#define PANEL2_WIDTH 10
#define PANEL2_HEIGHT 25


#define BRIGHTNESS 128

const Palette VEINS = {
  { 0, 0, 0 }, // background
  { 191, 0, 255 }, // foreground
  { 0, 0, 0 }, // accent
  { 86,100,39 }, // highlight
};

uint32_t lastFrame;
RGB** frameBuffer1;
RGB** frameBuffer2;
Adafruit_NeoPixel strip1(PANEL1_WIDTH * PANEL1_HEIGHT, PANEL1_PIN, NEO_BRG + NEO_KHZ800);
Adafruit_NeoPixel strip2(PANEL2_WIDTH * PANEL2_HEIGHT, PANEL2_PIN, NEO_BRG + NEO_KHZ800);

LayerEngine engine1 = LayerEngine(PANEL1_WIDTH, PANEL1_HEIGHT);
LayerEngine engine2 = LayerEngine(PANEL2_WIDTH, PANEL2_HEIGHT);

Layers::Black black1 = Layers::Black(PANEL1_WIDTH, PANEL1_HEIGHT, VEINS);
Layers::Spread spread1 = Layers::Spread(PANEL1_WIDTH, PANEL1_HEIGHT, 1, 1, 5, VEINS);

Layers::Black black2 = Layers::Black(PANEL2_WIDTH, PANEL2_HEIGHT, VEINS);
Layers::Spread spread2a = Layers::Spread(PANEL2_WIDTH, PANEL2_HEIGHT, 0, 0, 25, VEINS);
Layers::Spread spread2b = Layers::Spread(PANEL2_WIDTH, PANEL2_HEIGHT, 10, 6, 5, VEINS);
Layers::Mask mask = Layers::Mask(7, 2, 3, 9, VEINS);

LightMapper lightMapper_1 = LightMapper(PANEL1_WIDTH, PANEL1_HEIGHT);
LightMapper lightMapper_2 = LightMapper(PANEL2_WIDTH, PANEL2_HEIGHT);

void addPanel1Parameters() {
  lightMapper_1.addDeadZone({ 0, 0, 1 });
  lightMapper_1.addDeadZone({ 0, 3, 11 });
  lightMapper_1.addDeadZone({ 1, 0, 1 });
  lightMapper_1.addDeadZone({ 1, 3, 11 });
  lightMapper_1.addDeadZone({ 2, 0, 1 });
  lightMapper_1.addDeadZone({ 2, 8, 11 });
  lightMapper_1.addDeadZone({ 3, 0, 1 });
  lightMapper_1.addDeadZone({ 3, 8, 11 });
  lightMapper_1.addDeadZone({ 4, 0, 1 });
  lightMapper_1.addDeadZone({ 4, 7, 11 });
  lightMapper_1.addDeadZone({ 5, 7, 11 });
  lightMapper_1.addDeadZone({ 6, 7, 11 });
  lightMapper_1.addDeadZone({ 7, 8, 11 });
  lightMapper_1.addDeadZone({ 8, 8, 11 });
  lightMapper_1.addDeadZone({ 9, 8, 11 });
  lightMapper_1.addDeadZone({ 10, 9, 11 });
  lightMapper_1.addDeadZone({ 12, 0, 2 });
  lightMapper_1.addDeadZone({ 13, 0, 2 });
  lightMapper_1.addDeadZone({ 14, 0, 1 });
  lightMapper_1.addDeadZone({ 15, 0, 1 });
  lightMapper_1.addDeadZone({ 16, 0, 1 });
  lightMapper_1.addDeadZone({ 17, 0, 1 });
  lightMapper_1.addDeadZone({ 18, 0, 1 });
  lightMapper_1.addDeadZone({ 18, 8, 11 });
  lightMapper_1.addDeadZone({ 19, 0, 1 });
  lightMapper_1.addDeadZone({ 19, 8, 11 });
  lightMapper_1.addDeadZone({ 20, 0, 1 });
  lightMapper_1.addDeadZone({ 20, 8, 11 });
  lightMapper_1.addDeadZone({ 21, 0, 2 });
  lightMapper_1.addDeadZone({ 21, 8, 11 });
  lightMapper_1.addDeadZone({ 22, 0, 3 });
  lightMapper_1.addDeadZone({ 22, 8, 11 });
  lightMapper_1.addDeadZone({ 23, 0, 6 });
  lightMapper_1.addDeadZone({ 23, 8, 11 });
  lightMapper_1.addDeadZone({ 24, 0, 6 });
  lightMapper_1.addDeadZone({ 24, 8, 11 });
}

void addPanel2Parameters() {
  lightMapper_2.addDeadZone({ 0, 2, 10 });
  lightMapper_2.addDeadZone({ 1, 2, 10 });
  lightMapper_2.addDeadZone({ 2, 2, 8 });
  lightMapper_2.addDeadZone({ 2, 9, 10 });
  lightMapper_2.addDeadZone({ 3, 2, 8 });
  lightMapper_2.addDeadZone({ 3, 9, 10 });
  lightMapper_2.addDeadZone({ 4, 5, 8 });
  lightMapper_2.addDeadZone({ 5, 5, 8 });
  lightMapper_2.addDeadZone({ 6, 5, 7 });
  lightMapper_2.addDeadZone({ 7, 5, 7 });
  lightMapper_2.addDeadZone({ 8, 5, 7 });
  lightMapper_2.addDeadZone({ 9, 0, 1 });
  lightMapper_2.addDeadZone({ 9, 5, 7 });
  lightMapper_2.addDeadZone({ 10, 0, 1 });
  lightMapper_2.addDeadZone({ 10, 5, 7 });
  lightMapper_2.addDeadZone({ 10, 9, 10 });
  lightMapper_2.addDeadZone({ 11, 0, 1 });
  lightMapper_2.addDeadZone({ 11, 5, 10 });
  lightMapper_2.addDeadZone({ 12, 0, 1 });
  lightMapper_2.addDeadZone({ 12, 5, 10 });
  lightMapper_2.addDeadZone({ 13, 4, 10 });
  lightMapper_2.addDeadZone({ 14, 4, 10 });
  lightMapper_2.addDeadZone({ 15, 7, 10 });
  lightMapper_2.addDeadZone({ 16, 7, 10 });
  lightMapper_2.addDeadZone({ 17, 7, 10 });
  lightMapper_2.addDeadZone({ 18, 7, 10 });
  lightMapper_2.addDeadZone({ 19, 7, 10 });
  lightMapper_2.addDeadZone({ 20, 0, 2 });
  lightMapper_2.addDeadZone({ 20, 8, 10 });
  lightMapper_2.addDeadZone({ 21, 0, 3 });
  lightMapper_2.addDeadZone({ 21, 8, 10 });
  lightMapper_2.addDeadZone({ 22, 0, 3 });
  lightMapper_2.addDeadZone({ 22, 8, 10 });
  lightMapper_2.addDeadZone({ 23, 0, 4 });
  lightMapper_2.addDeadZone({ 23, 8, 10 });
  lightMapper_2.addDeadZone({ 24, 0, 4 });
  lightMapper_2.addDeadZone({ 24, 8, 10 });
}

void setup() {
  strip1.begin();
  strip1.show();
  strip1.setBrightness(BRIGHTNESS);
  frameBuffer1 = new RGB*[PANEL1_WIDTH];
  for (int x = 0; x < PANEL1_WIDTH; x++) {
    frameBuffer1[x] = new RGB[PANEL1_HEIGHT];
    for (int y = 0; y < PANEL1_HEIGHT; y++) {
      frameBuffer1[x][y] = { 0, 0, 0 };
    }
  }
  engine1.push(&black1);
  engine1.push(&spread1);

  strip2.begin();
  strip2.show();
  strip2.setBrightness(BRIGHTNESS);
  frameBuffer2 = new RGB*[PANEL2_WIDTH];
  for (int x = 0; x < PANEL2_WIDTH; x++) {
    frameBuffer2[x] = new RGB[PANEL2_HEIGHT];
    for (int y = 0; y < PANEL2_HEIGHT; y++) {
      frameBuffer2[x][y] = { 0, 0, 0 };
    }
  }
  engine2.push(&black2);
  engine2.push(&spread2a);
  engine2.push(&mask);
  engine2.push(&spread2b);
  
  
  lastFrame = millis();
}

void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  Serial.println(1000 / (millis() - lastFrame)); // Log FPS
  lastFrame = millis();
  
  engine1.computeFrame(frameBuffer1);
  engine2.computeFrame(frameBuffer2);
  
  for (int x = 0; x < PANEL1_WIDTH; x++) {
    for (int y = 0; y < PANEL1_HEIGHT; y++) {
      int index = lightMapper_1.getPixelIndex(x, y);
      if (index >= 0) {
        RGB colour = frameBuffer1[x][y];
        strip1.setPixelColor(index, colour.r, colour.g, colour.b);
      }
    }
  }
  for (int x = 0; x < PANEL2_WIDTH; x++) {
    for (int y = 0; y < PANEL2_HEIGHT; y++) {
      int index = lightMapper_2.getPixelIndex(x, y);
      if (index >= 0) {
        RGB colour = frameBuffer2[x][y];
        strip2.setPixelColor(index, colour.r, colour.g, colour.b);
      }
    }
  }
  
  strip1.show();
  strip2.show();
}
