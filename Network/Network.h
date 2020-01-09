#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#define EOM_CHAR '~'

enum CommandType {
  NONE,
  SET_LED
};

typedef struct {
  CommandType type;
  char* data;
} Command;

class Peripheral {
  private:
    Adafruit_BluefruitLE_SPI ble;
    bool initialized;
    bool write(char* msg);
  public:
    Peripheral();
    bool init();
    Command read();
};


#endif