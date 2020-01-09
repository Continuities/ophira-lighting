#include <Network.h>

Peripheral ble = Peripheral();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  ble.init();
}

void loop() {
  Command command = ble.read();
  switch (command.type) {
    case NONE:
      delay(100);
      break;
    case SET_LED:
      Serial.print(F("[SET_LED] ")); Serial.println(command.data);
      if (strcmp(command.data, "on") == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      else if (strcmp(command.data, "off") == 0) {
        digitalWrite(LED_BUILTIN, LOW);
      }
      break;
  }
}
