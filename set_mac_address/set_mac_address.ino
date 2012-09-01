#include <EEPROM.h>

#define MY_ADDRESS 0xF0F0F0F0D2LL

void setup() {
  Serial.begin(115200);
  Serial.print("EEPROM address:");
  for (int b = 0; b < 5; ++b) {
    Serial.print(" ");
    Serial.print(EEPROM.read(b), HEX);
  }
  Serial.println();
  for (int b = 0; b < 5; ++b) {
    uint8_t c = (uint8_t)(MY_ADDRESS >> (8 * (4 - b)));
    Serial.print("Checking byte ");
    Serial.print(b, DEC);
    Serial.print(": ");
    Serial.print(c, HEX);
    if (EEPROM.read(b) != c) {
      EEPROM.write(b, c);
      Serial.println(" - written.");
    } else Serial.println(" - already ok.");
  }
  Serial.println("MAC address set.");
}

void loop() {
}
