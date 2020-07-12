#include <Adafruit_NeoPXL8.h>
#include <LayerEngine.h>
#include <LightMapper.h>

/* =============================
 *  Ophira Lymph Wall lighting
 * =============================
 * Ethernet wire colour mapping:
 *  1 - brown
 *  2 - green
 *  3 - blue
 *  4 - orange
 */

#define FPS 30
#define HEIGHT 10 // Height of the pixel grid
#define WIDTH 10 // Width of the pixel grid
#define STRIP_LENGTH 241 // length of each of the 8 physical led lines
#define BRIGHTNESS 128 // 255
#define GLITCH_MIN_TIME 10000 // ms
#define GLITCH_MAX_TIME 60000 // ms
#define GLITCH_MIN_DURATION 500 // ms
#define GLITCH_MAX_DURATION 3000 // ms

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

uint32_t nextGlitch = 0;
uint32_t glitchOff = 0;
uint32_t lastFrame;
RGB** frameBuffer;

LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
Layers::VerticalStripes testPattern = Layers::VerticalStripes(WIDTH, HEIGHT, TEST_STRIPES);
Layers::HorizontalStripes horizontal = Layers::HorizontalStripes(WIDTH, HEIGHT, TEST_STRIPES);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, XRAY);
Layers::Background background = Layers::Background(WIDTH, HEIGHT, XRAY);
Layers::Ether ether = Layers::Ether(WIDTH, HEIGHT, XRAY);
Layers::Dots dots = Layers::Dots(WIDTH, HEIGHT, XRAY);
Layers::Splotches splotches = Layers::Splotches(WIDTH, HEIGHT, XRAY);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, XRAY);
Layers::StrandIdent strandIdent = Layers::StrandIdent(WIDTH, HEIGHT, TEST_STRIPES);

LightMapper lightMapper = LightMapper(WIDTH, HEIGHT);

int8_t pins[8] = { PIN_SERIAL1_RX, PIN_SERIAL1_TX, 11, 13, 5, SDA, A4, A3 };
Adafruit_NeoPXL8 strip(STRIP_LENGTH, pins, NEO_BRG);

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
  lightMapper.addDeadZone({ 24, 9, 38 });
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

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  addSpleenParameters();
  
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }
  
  lastFrame = millis();
//  engine.push(&black);
//  engine.push(&background);
//  engine.push(&testPattern);
  engine.push(&ether);
//  engine.push(&dots);
  engine.push(&splotches);
}

void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  Serial.print("FPS: "); Serial.println(1000 / (millis() - lastFrame)); // Log FPS
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
      int index = lightMapper.getPixelIndex(x, y);
      if (index >= 0) {
        RGB colour = frameBuffer[x][y];
        strip.setPixelColor(index, colour.r, colour.g, colour.b);
      }
    }
  }
  
  strip.show();
}
