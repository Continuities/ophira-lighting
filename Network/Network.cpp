#include <string.h>
#include "Network.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.6.6"
#define MODE_LED_BEHAVIOUR "MODE"

Peripheral::Peripheral() : 
  ble(Adafruit_BluefruitLE_SPI(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST)),
  initialized(false) {}

bool Peripheral::init() {
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if (!ble.begin(VERBOSE_MODE)) {
    Serial.println(F("Couldn't find Bluefruit, make sure it's in Command mode & check wiring?"));
    return false;
  }
  Serial.println( F("OK!") );

  if (FACTORYRESET_ENABLE) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if (!ble.factoryReset()){
      Serial.println(F("Couldn't factory reset"));
      return false;
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);
  ble.verbose(false);

  /* Wait for connection */
  while (!ble.isConnected()) {
    delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if (ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
    // Change Mode LED Activity
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  initialized = true;
  return true;
}

bool Peripheral::write(char* msg) {
  ble.print("AT+BLEUARTTX=");
  ble.print(msg);
  ble.println(EOM_CHAR);
  if (!ble.waitForOK() ) {
    Serial.print(F("Failed to send: ")); Serial.println(msg);
    return false;
  }
  return true;
}

Command Peripheral::read() {
  ble.println("AT+BLEUARTRX");
  ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return { NONE, "" };
  }
  // There's some data in the buffer
  char* message = ble.buffer;
  ble.waitForOK();

  // Strip EOM character, if it's there
  if (message[strlen(message) - 1] == EOM_CHAR) {
    message[strlen(message) - 1] = 0;
  }

  char* cmd = strtok(message, " ");
  if (cmd == NULL) {
    Serial.print(F("Bad command: ")); Serial.println(message);
    return { NONE, "" };
  }

  char* data = strtok(NULL, " ");
  if (data == NULL) {
    data = "";
  }

  if (strcmp(cmd, "SYN") == 0) {
    // Invisibly send an ACK
    write("ACK");
    return { NONE, "" };
  }

  if (strcmp(cmd, "SET_LED") == 0) {
    return { SET_LED, data };
  }

  Serial.print(F("Unrecognized command: ")); Serial.println(cmd);
  return { NONE, "" };
}

