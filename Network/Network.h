#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

class Peripheral {
  private:
    Adafruit_BluefruitLE_SPI ble;
    bool initialized;
  public:
    Peripheral();
    bool init();
    String read();
};


#endif