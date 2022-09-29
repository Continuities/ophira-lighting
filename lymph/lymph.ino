#include <Adafruit_NeoPXL8.h>
#include <LayerEngine.h>
#include <LightMapper.h>

#define FPS 30
#define WIDTH 44
#define HEIGHT 43
#define NUM_LEDS 241
#define BRIGHTNESS 128 // 255
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

const Palette TEST = {
  { 255, 0, 0 }, // background
  { 0, 255, 0 }, // foreground
  { 0, 0, 255 },//{ 148, 202, 203 }, // accent
  { 0, 0, 0 } // highlight
};

uint32_t nextGlitch = 0;
uint32_t glitchOff = 0;
uint32_t lastFrame;
RGB** frameBuffer;

LayerEngine engine = LayerEngine(WIDTH, HEIGHT);
Layers::VerticalStripes test = Layers::VerticalStripes(WIDTH, HEIGHT, TEST);
Layers::HorizontalStripes test2 = Layers::HorizontalStripes(WIDTH, HEIGHT, TEST);
Layers::Black black = Layers::Black(WIDTH, HEIGHT, XRAY);
Layers::Ether ether = Layers::Ether(WIDTH, HEIGHT, XRAY);
Layers::Dots dots = Layers::Dots(WIDTH, HEIGHT, XRAY);
Layers::Splotches splotches = Layers::Splotches(WIDTH, HEIGHT, XRAY);
Layers::Glitch glitch = Layers::Glitch(WIDTH, HEIGHT, XRAY);

LightMapper lightMapper = LightMapper(WIDTH, HEIGHT);

int8_t pins[8] = { 13, 12, 11, 10, SCK, 5, 9, 6 };
//Adafruit_NeoPXL8 strip(WIDTH * HEIGHT, pins, NEO_BRG);
Adafruit_NeoPXL8 strip(NUM_LEDS, pins, NEO_BRG);


void addLymphParameters() {

  // Fuuuuuuuuuuuuuck. The lightmapper assumes always alternating directions in the led strips.
  // This is not true of the lymph wall. Fuuuuuuuuuck. TODO.
  
  // lightMapper.addDeadZone({ 8, 23, 38 });

  // TODO Pad each of the eight lines to 241 pixels
  // lightMapper.addPadding(684, 16);

  // First three strips are 220

  // Fourth is 241

  // Five, six, and seven???
  
  
}

void setup() {
  randomSeed(analogRead(0));
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
//  while(!Serial) {}

  addLymphParameters();
  
  frameBuffer = new RGB*[WIDTH];
  for (int x = 0; x < WIDTH; x++) {
    frameBuffer[x] = new RGB[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
      frameBuffer[x][y] = { 0, 0, 0 };
    }
  }
  
  lastFrame = millis();
//  engine.push(&test);
//  engine.push(&black);
  engine.push(&ether);
  engine.push(&dots);
  engine.push(&splotches);
//  engine.push(&glitch);

}

bool blinking = false;

void loop() {
  if (millis() - lastFrame < 1000 / FPS) {
    return;
  }
  digitalWrite(LED_BUILTIN, blinking ? LOW : HIGH);
  blinking = !blinking;
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
      int index = lightMapper.getPixelIndex(x, y);
      if (index >= 0) {
        RGB colour = frameBuffer[x][y];
        strip.setPixelColor(lightMapper.getPixelIndex(x, y), colour.r, colour.g, colour.b);
      }
    }
  }

//  int testX = 0;
//  int testY = 0;
//  for (int x = 0; x < WIDTH; x++) {
//    for (int y = 0; y < HEIGHT; y++) {
//      int index = lightMapper.getPixelIndex(x, y);
//      if (index >= 0) {
//        RGB colour = y == testY ? TEST.background : TEST.highlight;
//        strip.setPixelColor(lightMapper.getPixelIndex(x, y), colour.r, colour.g, colour.b);
//      }
//    }
//  }
  

//  for(int i = 0; i < WIDTH * HEIGHT; i++) {
//    strip.setPixelColor(i, 0, 0, 255);
//  }
  
  strip.show();
}
