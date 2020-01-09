#include <Network.h>

Peripheral ble = Peripheral();

void setup() {
  ble.init();
}

void loop() {
  String data = ble.read();
  if (data.length() == 0) {
    delay(100);
    return;
  }
  Serial.print(F("[RX] ")); Serial.println(data);
}
