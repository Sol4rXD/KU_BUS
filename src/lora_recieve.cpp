#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

void setup() {
    Serial.begin(115200);
    Serial.println("LoRa Receiver");

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    Serial.println("Set up already");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(300);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(300);                       // wait for a second
        // read packet
        while (LoRa.available()) {
        Serial.println((char)LoRa.read());
        }
  }
}